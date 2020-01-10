// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/DiagnosticParse.h"
#include "soll/Parse/Parser.h"

namespace {

template <typename TO, typename FROM>
std::unique_ptr<TO> dynamic_unique_pointer_cast(std::unique_ptr<FROM> &&old) {
  // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  if (auto P = dynamic_cast<TO *>(old.get())) {
    old.release();
    return std::unique_ptr<TO>{P};
  }
  return nullptr;
}

} // namespace

namespace soll {

std::unique_ptr<Block> Parser::parseAsmBlock(bool HasScope) {
  const SourceLocation Begin = Tok.getLocation();
  std::unique_ptr<ParseScope> BlockScope;
  if (!HasScope) {
    BlockScope = std::make_unique<ParseScope>(this, 0);
  }

  ExpectAndConsume(tok::l_brace);
  std::vector<std::unique_ptr<Stmt>> Statements;
  while (Tok.isNot(tok::r_brace) && Tok.isNot(tok::eof)) {
    Statements.emplace_back(parseAsmStatement());
  }
  const SourceLocation End = Tok.getEndLoc();
  ExpectAndConsume(tok::r_brace);
  return std::make_unique<Block>(SourceRange(Begin, End),
                                 std::move(Statements));
}

std::unique_ptr<Stmt> Parser::parseAsmStatement() {
  switch (Tok.getKind()) {
  case tok::kw_let:
    return parseAsmVariableDeclarationStatement();
  case tok::kw_function:
    return parseAsmFunctionDefinitionStatement();
  case tok::l_brace:
    return parseAsmBlock();
  case tok::kw_if:
    return parseAsmIfStatement();
  case tok::kw_switch:
    return parseAsmSwitchStatement();
  case tok::kw_for:
    return parseAsmForStatement();
  case tok::kw_break: {
    const SourceRange Range = Tok.getRange();
    ConsumeToken(); // 'break'
    return std::make_unique<BreakStmt>(Range);
  }
  case tok::kw_continue: {
    const SourceRange Range = Tok.getRange();
    ConsumeToken(); // 'continue'
    return std::make_unique<ContinueStmt>(Range);
  }
  // case tok::equalcolon:
  // stack assignment, assembly only
  default:
    break;
  }

  const SourceLocation Begin = Tok.getLocation();
  std::unique_ptr<Expr> Elementary = parseElementaryOperation();
  switch (Tok.getKind()) {
  case tok::l_paren: {
    return parseAsmCall(std::move(Elementary));
  }
  case tok::comma:
  case tok::colonequal: {
    std::vector<std::unique_ptr<AsmIdentifier>> Variables;
    if (auto P =
            dynamic_unique_pointer_cast<AsmIdentifier>(std::move(Elementary))) {
      Variables.emplace_back(std::move(P));
    }
    while (TryConsumeToken(tok::comma)) {
      Elementary = parseElementaryOperation();
      if (auto P = dynamic_unique_pointer_cast<AsmIdentifier>(
              std::move(Elementary))) {
        Variables.emplace_back(std::move(P));
      }
    }
    ExpectAndConsume(tok::colonequal);
    auto Value = parseAsmExpression();
    const SourceLocation End = Value->getLocation().getEnd();
    return std::make_unique<AsmAssignmentStmt>(
        SourceRange(Begin, End),
        std::make_unique<AsmIdentifierList>(std::move(Variables)),
        std::move(Value));
  }
  default:
    break;
  }
  return Elementary;
}

std::unique_ptr<IfStmt> Parser::parseAsmIfStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'if'
  std::unique_ptr<Expr> Condition = std::make_unique<ImplicitCastExpr>(
      parseAsmExpression(), CastKind::TypeCast,
      std::make_shared<BooleanType>());
  std::unique_ptr<Stmt> TrueBody = parseAsmBlock();
  std::unique_ptr<Stmt> FalseBody;
  const SourceLocation End = TrueBody->getLocation().getEnd();
  return std::make_unique<IfStmt>(SourceRange(Begin, End), std::move(Condition),
                                  std::move(TrueBody), std::move(FalseBody));
}

std::unique_ptr<AsmSwitchStmt> Parser::parseAsmSwitchStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ParseScope SwitchScope{this, 0};

  ConsumeToken(); // 'switch'
  std::unique_ptr<Expr> Condition = parseAsmExpression();
  std::vector<std::unique_ptr<AsmSwitchCase>> Cases;
  SourceLocation End = Condition->getLocation().getEnd();
  while (Tok.is(tok::kw_case)) {
    Cases.push_back(parseAsmCaseStatement());
  }
  if (Tok.is(tok::kw_default)) {
    Cases.push_back(parseAsmDefaultStatement());
  }
  if (Cases.empty()) {
    Diag(Begin, diag::err_switch_without_case);
  } else {
    End = Cases.back()->getLocation().getEnd();
  }
  return std::make_unique<AsmSwitchStmt>(SourceRange(Begin, Tok.getEndLoc()),
                                         std::move(Condition),
                                         std::move(Cases));
}

std::unique_ptr<AsmCaseStmt> Parser::parseAsmCaseStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'case'
  std::unique_ptr<Expr> Value = parseElementaryOperation();
  std::unique_ptr<Block> Body = parseAsmBlock();
  const SourceLocation End = Body->getLocation().getEnd();
  return std::make_unique<AsmCaseStmt>(SourceRange(Begin, End),
                                       std::move(Value), std::move(Body));
}

std::unique_ptr<AsmDefaultStmt> Parser::parseAsmDefaultStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'default'
  std::unique_ptr<Block> Body = parseAsmBlock();
  return std::make_unique<AsmDefaultStmt>(SourceRange(Begin, Tok.getEndLoc()),
                                          std::move(Body));
}

std::unique_ptr<AsmForStmt> Parser::parseAsmForStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ParseScope ForScope{this, 0};

  ConsumeToken(); // 'for'
  std::unique_ptr<Block> Init = parseAsmBlock(true);
  std::unique_ptr<Expr> Condition = std::make_unique<ImplicitCastExpr>(
      parseAsmExpression(), CastKind::TypeCast,
      std::make_shared<BooleanType>());
  std::unique_ptr<Block> Loop = parseAsmBlock(true);
  std::unique_ptr<Block> Body;
  {
    ParseScope ForScope{this, Scope::BreakScope | Scope::ContinueScope};
    Body = parseAsmBlock(true);
  }
  return std::make_unique<AsmForStmt>(SourceRange(Begin, Tok.getEndLoc()),
                                      std::move(Init), std::move(Condition),
                                      std::move(Loop), std::move(Body));
}

std::unique_ptr<Expr> Parser::parseAsmExpression() {
  std::unique_ptr<Expr> Elementary = parseElementaryOperation();
  if (Tok.is(tok::l_paren)) {
    return parseAsmCall(std::move(Elementary));
  }
  return Elementary;
}

std::unique_ptr<Expr> Parser::parseElementaryOperation() {
  std::unique_ptr<Expr> Expression;

  switch (Tok.getKind()) {
  case tok::kw_address:
  case tok::identifier: {
    bool IsCall = false;
    if (auto Next = TheLexer.LookAhead(0); Next && Next->is(tok::l_paren)) {
      IsCall = true;
    }
    Expression = Actions.CreateAsmIdentifier(Tok, IsCall);
    ConsumeToken(); // identifier
    break;
  }
  // case tok::kw_address:
  case tok::kw_return:
  case tok::kw_byte: {
    Expression = Actions.CreateAsmIdentifier(Tok, false);
    ConsumeToken(); // SpecialIdentifier
    break;
  }
  case tok::string_literal: {
    llvm::StringRef StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = std::make_unique<StringLiteral>(Tok, stringUnquote(StrValue));
    ConsumeStringToken(); // string literal
    break;
  }
  case tok::hex_string_literal: {
    llvm::StringRef StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = std::make_unique<StringLiteral>(Tok, hexUnquote(StrValue));
    ConsumeStringToken(); // hex string literal
    break;
  }
  case tok::numeric_constant: {
    llvm::StringRef NumValue(Tok.getLiteralData(), Tok.getLength());
    auto [Signed, Value] = numericParse(NumValue);
    if (Signed) {
      Value = Value.sext(256);
    } else {
      Value = Value.zext(256);
    }
    Expression = std::make_unique<NumberLiteral>(Tok, Signed, Value);
    ConsumeToken(); // numeric constant
    break;
  }
  case tok::kw_true: {
    Expression = std::make_unique<BooleanLiteral>(Tok, true);
    ConsumeToken(); // 'true'
    break;
  }
  case tok::kw_false: {
    Expression = std::make_unique<BooleanLiteral>(Tok, false);
    ConsumeToken(); // 'false'
    break;
  }
  default:
    Diag(diag::err_unimplemented_token) << Tok.getKind();
    ConsumeToken();
  }
  return Expression;
}

std::unique_ptr<AsmVarDecl> Parser::parseAsmVariableDeclaration() {
  const SourceLocation Begin = Tok.getLocation();
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  ConsumeToken();
  TypePtr T = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
  std::unique_ptr<Expr> Value;
  auto VD = std::make_unique<AsmVarDecl>(SourceRange(Begin, Tok.getEndLoc()),
                                         Name, std::move(T), std::move(Value));
  Actions.addDecl(VD.get());
  return VD;
}

std::vector<std::unique_ptr<VarDeclBase>>
Parser::parseAsmVariableDeclarationList() {
  std::vector<std::unique_ptr<VarDeclBase>> Variables;
  while (Tok.isAnyIdentifier()) {
    Variables.emplace_back(parseAsmVariableDeclaration());
    if (!TryConsumeToken(tok::comma)) {
      break;
    }
  }
  return Variables;
}

std::unique_ptr<DeclStmt> Parser::parseAsmVariableDeclarationStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // let
  std::vector<std::unique_ptr<VarDeclBase>> Variables =
      parseAsmVariableDeclarationList();
  std::unique_ptr<Expr> Value;
  if (TryConsumeToken(tok::colonequal)) {
    Value = parseAsmExpression();
  }
  return std::make_unique<DeclStmt>(SourceRange(Begin, Tok.getEndLoc()),
                                    std::move(Variables), std::move(Value));
}

std::unique_ptr<AsmFunctionDeclStmt>
Parser::parseAsmFunctionDefinitionStatement() {
  const SourceLocation Begin = Tok.getLocation();
  std::unique_ptr<AsmFunctionDecl> AFD;
  {
    ParseScope ArgumentScope{this, 0};
    ConsumeToken(); // function
    auto Name = Tok.getIdentifierInfo()->getName();
    ConsumeToken();
    ConsumeParen(); // (
    std::vector<std::unique_ptr<VarDeclBase>> Parameters =
        parseAsmVariableDeclarationList();
    ConsumeParen(); // )
    std::vector<std::unique_ptr<VarDeclBase>> ReturnParams;
    if (TryConsumeToken(tok::arrow)) {
      ReturnParams = parseAsmVariableDeclarationList();
      if (ReturnParams.size() > 1) {
        Diag(ReturnParams.front()->getLocation().getBegin(),
             diag::err_unimplemented_tuple_return);
      }
    }
    std::unique_ptr<Block> Body;
    {
      ParseScope FunctionScope{this, Scope::FunctionScope};
      Body = parseAsmBlock(true);
    }
    const SourceLocation End = Body->getLocation().getEnd();
    const SourceRange L(Begin, End);
    AFD = std::make_unique<AsmFunctionDecl>(
        L, Name, std::make_unique<ParamList>(std::move(Parameters)),
        std::make_unique<ParamList>(std::move(ReturnParams)), std::move(Body));
  }
  Actions.addDecl(AFD.get());
  return std::make_unique<AsmFunctionDeclStmt>(AFD->getLocation(),
                                               std::move(AFD));
}

std::unique_ptr<Expr> Parser::parseAsmCall(std::unique_ptr<Expr> &&E) {
  ConsumeParen(); // (
  if (auto ID = dynamic_unique_pointer_cast<AsmIdentifier>(std::move(E))) {
    const SourceLocation Begin = ID->getLocation().getBegin();
    std::vector<std::unique_ptr<Expr>> Args;
    while (!isTokenParen()) {
      Args.emplace_back(parseAsmExpression());
      TryConsumeToken(tok::comma);
    }
    const SourceLocation End = Tok.getEndLoc();
    ConsumeParen(); // )
    return Actions.CreateAsmCallExpr(SourceRange(Begin, End), std::move(ID),
                                     std::move(Args));
  } else {
    assert(false && "Callee is not an valid Identifier");
    __builtin_unreachable();
  }
}

} // namespace soll
