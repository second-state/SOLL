// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/Parser.h"

using namespace std;

namespace {

template <typename TO, typename FROM>
unique_ptr<TO> dynamic_unique_pointer_cast(unique_ptr<FROM> &&old) {
  // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
  if (auto P = dynamic_cast<TO *>(old.get())) {
    old.release();
    return unique_ptr<TO>{P};
  }
  return nullptr;
}

} // namespace

namespace soll {

std::unique_ptr<Block> Parser::parseAsmBlock(bool HasScope) {
  if (!HasScope) {
    ParseScope BlockScope{this, 0};
  }

  ExpectAndConsume(tok::l_brace);
  vector<unique_ptr<Stmt>> Statements;
  while (Tok.isNot(tok::r_brace)) {
    Statements.emplace_back(parseAsmStatement());
  }
  ExpectAndConsume(tok::r_brace);
  return std::make_unique<Block>(std::move(Statements));
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
  case tok::kw_break:
    ConsumeToken(); // 'break'
    return std::make_unique<BreakStmt>();
  case tok::kw_continue:
    ConsumeToken(); // 'continue'
    return std::make_unique<ContinueStmt>();
  // case tok::equalcolon:
  // stack assignment, assembly only
  default:
    break;
  }

  unique_ptr<Expr> Elementary = parseElementaryOperation();
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
    return make_unique<AsmAssignmentStmt>(
        make_unique<AsmIdentifierList>(std::move(Variables)),
        parseAsmExpression());
  }
  default:
    break;
  }
  return Elementary;
}

unique_ptr<IfStmt> Parser::parseAsmIfStatement() {
  ConsumeToken(); // 'if'
  unique_ptr<Expr> Condition = parseAsmExpression();
  unique_ptr<Stmt> TrueBody = parseAsmBlock();
  unique_ptr<Stmt> FalseBody;
  return std::make_unique<IfStmt>(std::move(Condition), std::move(TrueBody),
                                  std::move(FalseBody));
}

std::unique_ptr<AsmSwitchStmt> Parser::parseAsmSwitchStatement() {
  ParseScope SwitchScope{this, 0};

  ConsumeToken(); // 'switch'
  unique_ptr<Expr> Condition = parseAsmExpression();
  unique_ptr<AsmSwitchCase> FirstCase;
  std::unique_ptr<AsmSwitchStmt> SwitchStmt =
      make_unique<AsmSwitchStmt>(std::move(Condition), std::move(FirstCase));
  while (TryConsumeToken(tok::kw_case)) {
    unique_ptr<Expr> Value = parseElementaryOperation();
    unique_ptr<Block> Body = parseAsmBlock();
    unique_ptr<AsmSwitchCase> Case =
        make_unique<AsmCaseStmt>(std::move(Value), std::move(Body));
    SwitchStmt->addSwitchCase(std::move(Case));
  }
  if (TryConsumeToken(tok::kw_default)) {
    unique_ptr<Block> Body = parseAsmBlock();
    unique_ptr<AsmDefaultStmt> Default =
        make_unique<AsmDefaultStmt>(std::move(Body));
    SwitchStmt->addSwitchCase(std::move(Default));
  }
  return SwitchStmt;
}

std::unique_ptr<AsmForStmt> Parser::parseAsmForStatement() {
  ParseScope ForScope{this, 0};

  ConsumeToken(); // 'for'
  unique_ptr<Block> Init = parseAsmBlock(true);
  unique_ptr<Expr> Condition = parseAsmExpression();
  unique_ptr<Block> Loop = parseAsmBlock(true);
  unique_ptr<Block> Body;
  {
    ParseScope ForScope{this, Scope::BreakScope | Scope::ContinueScope};
    Body = parseAsmBlock(true);
  }
  return std::make_unique<AsmForStmt>(std::move(Init), std::move(Condition),
                                      std::move(Loop), std::move(Body));
}

std::unique_ptr<Expr> Parser::parseAsmExpression() {
  unique_ptr<Expr> Elementary = parseElementaryOperation();
  if (Tok.is(tok::l_paren)) {
    return parseAsmCall(std::move(Elementary));
  }
  return Elementary;
}

std::unique_ptr<Expr> Parser::parseElementaryOperation() {
  unique_ptr<Expr> Expression;

  switch (Tok.getKind()) {
  case tok::identifier: {
    Expression =
        Actions.CreateAsmIdentifier(Tok.getIdentifierInfo()->getName());
    ConsumeToken(); // identifier
    break;
  }
  // case tok::kw_address:
  case tok::kw_return:
  case tok::kw_byte: {
    Expression = Actions.CreateAsmIdentifier(Tok.getName());
    ConsumeToken(); // SpecialIdentifier
    break;
  }
  case tok::string_literal: {
    std::string StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = make_unique<StringLiteral>(stringUnquote(std::move(StrValue)));
    ConsumeStringToken(); // string literal
    break;
  }
  case tok::hex_string_literal: {
    std::string StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = make_unique<StringLiteral>(hexUnquote(std::move(StrValue)));
    ConsumeStringToken(); // hex string literal
    break;
  }
  case tok::numeric_constant: {
    int NumValue;
    if (llvm::StringRef(Tok.getLiteralData(), Tok.getLength())
            .getAsInteger(0, NumValue)) {
      assert(false && "invalid numeric constant");
      __builtin_unreachable();
    }
    Expression = std::make_unique<NumberLiteral>(NumValue);
    ConsumeToken(); // numeric constant
    break;
  }
  case tok::kw_true: {
    ConsumeToken(); // 'true'
    Expression = std::make_unique<BooleanLiteral>(true);
    break;
  }
  case tok::kw_false: {
    ConsumeToken(); // 'false'
    Expression = std::make_unique<BooleanLiteral>(false);
    break;
  }
  default:
    assert(false && "Literal or identifier expected.");
    __builtin_unreachable();
  }
  return Expression;
}

std::unique_ptr<AsmVarDecl> Parser::parseAsmVariableDeclaration() {
  llvm::StringRef Name;
  Name = Tok.getIdentifierInfo()->getName();
  ConsumeToken();
  TypePtr T = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
  unique_ptr<Expr> Value;
  auto VD = std::make_unique<AsmVarDecl>(std::move(T), Name, std::move(Value));
  Actions.addDecl(VD.get());
  return VD;
}

vector<unique_ptr<VarDeclBase>> Parser::parseAsmVariableDeclarationList() {
  vector<unique_ptr<VarDeclBase>> Variables;
  while (true) {
    Variables.emplace_back(parseAsmVariableDeclaration());
    if (!TryConsumeToken(tok::comma)) {
      break;
    }
  }
  return Variables;
}

std::unique_ptr<DeclStmt> Parser::parseAsmVariableDeclarationStatement() {
  ConsumeToken(); // let
  vector<unique_ptr<VarDeclBase>> Variables = parseAsmVariableDeclarationList();
  unique_ptr<Expr> Value;
  if (TryConsumeToken(tok::colonequal)) {
    Value = parseAsmExpression();
  }
  return std::make_unique<DeclStmt>(std::move(Variables), std::move(Value));
}

std::unique_ptr<AsmFunctionDeclStmt>
Parser::parseAsmFunctionDefinitionStatement() {
  ConsumeToken(); // function
  auto Name = Tok.getIdentifierInfo()->getName();
  ConsumeToken();
  ConsumeParen(); // (
  vector<unique_ptr<VarDeclBase>> Parameters =
      parseAsmVariableDeclarationList();
  ConsumeParen(); // )
  vector<unique_ptr<VarDeclBase>> ReturnParams;
  if (TryConsumeToken(tok::arrow)) {
    ReturnParams = parseAsmVariableDeclarationList();
  }
  unique_ptr<Block> Body = parseAsmBlock();
  return std::make_unique<AsmFunctionDeclStmt>(
      Name, make_unique<ParamList>(std::move(Parameters)),
      make_unique<ParamList>(std::move(ReturnParams)), std::move(Body));
  return nullptr;
}

std::unique_ptr<Expr> Parser::parseAsmCall(std::unique_ptr<Expr> &&E) {
  ConsumeParen(); // (
  if (auto ID = dynamic_unique_pointer_cast<AsmIdentifier>(std::move(E))) {
    std::vector<std::unique_ptr<Expr>> Args;
    while (!isTokenParen()) {
      Args.emplace_back(parseAsmExpression());
      TryConsumeToken(tok::comma);
    }
    ConsumeParen(); // )
    return Actions.CreateAsmCallExpr(std::move(ID), std::move(Args));
  } else {
    assert(false && "Callee is not an valid Identifier");
    __builtin_unreachable();
  }
}

} // namespace soll
