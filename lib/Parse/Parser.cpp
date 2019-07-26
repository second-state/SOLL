#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/AST/OperationKinds.h"
#include "soll/AST/Stmt.h"
#include "soll/AST/Type.h"
#include "soll/Lex/Lexer.h"

using namespace std;

namespace soll {

// [PrePOC] need a token to binary op mapping table
static BinaryOperatorKind token2bop(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::caret:
    return BO_Exp;
  case tok::star:
    return BO_Mul;
  case tok::slash:
    return BO_Div;
  case tok::plus:
    return BO_Add;
  case tok::minus:
    return BO_Sub;
  case tok::equal:
    return BO_Assign;
  default:
    return BO_Undefined;
  }
}

// [PrePOC] need a token to unary op mapping table
static UnaryOperatorKind token2uop(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::exclaim:
    return UO_Not;
  default:
    return UO_Undefined;
  }
}

Parser::Parser(Lexer &lexer) : TheLexer(lexer) {}

unique_ptr<SourceUnit> Parser::parse() {
  llvm::Optional<Token> CurTok;
  vector<unique_ptr<Decl>> Nodes;

  while ((CurTok = TheLexer.LookAhead(0))->isNot(tok::eof)) {
    switch (CurTok->getKind()) {
    case tok::kw_pragma:
      Nodes.push_back(parsePragmaDirective());
      break;
    case tok::kw_import:
      TheLexer.CachedLex();
      break;
    case tok::kw_interface:
    case tok::kw_library:
    case tok::kw_contract:
      Nodes.push_back(parseContractDefinition());
      break;
    default:
      TheLexer.CachedLex();
      break;
    }
  }
  return make_unique<SourceUnit>(std::move(Nodes));
}

unique_ptr<PragmaDirective> Parser::parsePragmaDirective() {
  // pragma anything* ;
  // Currently supported:
  // pragma solidity ^0.4.0 || ^0.3.0;
  vector<string> Literals;
  vector<llvm::Optional<Token>> Tokens;
  TheLexer.CachedLex();
  do {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::unknown)
      assert(false && "Solidity Error: Token incompatible with Solidity parser "
                      "as part of pragma directive.");
    else if (Kind == tok::caret) {
      // [TODO] Fix tok::caret no literal, but not sure what means
      // Pattern not match Solidity : Solidity | ^ | 0.5 | .0 |
      //                   Soll     : Solidity | 0.5.0
      TheLexer.CachedLex();
    } else {
      string literal = getLiteralAndAdvance(TheLexer.LookAhead(0)).str();
      Literals.push_back(literal);
      Tokens.push_back(TheLexer.LookAhead(0));
    }
  } while (!TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::eof));
  TheLexer.CachedLex();

  // [TODO] Implement version recognize and compare. ref: parsePragmaVersion
  return std::make_unique<PragmaDirective>();
}

ContractDecl::ContractKind Parser::parseContractKind() {
  ContractDecl::ContractKind Kind;
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_interface:
    Kind = ContractDecl::ContractKind::Interface;
    break;
  case tok::kw_contract:
    Kind = ContractDecl::ContractKind::Contract;
    break;
  case tok::kw_library:
    Kind = ContractDecl::ContractKind::Library;
    break;
  default:
    assert(false && "Invalid contract kind.");
  }
  TheLexer.CachedLex();
  return Kind;
}

Decl::Visibility Parser::parseVisibilitySpecifier() {
  Decl::Visibility Vsblty(Decl::Visibility::Default);
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_public:
    Vsblty = Decl::Visibility::Public;
    break;
  case tok::kw_internal:
    Vsblty = Decl::Visibility::Internal;
    break;
  case tok::kw_private:
    Vsblty = Decl::Visibility::Private;
    break;
  case tok::kw_external:
    Vsblty = Decl::Visibility::External;
    break;
  default:
    assert(false && "Invalid visibility specifier.");
  }
  TheLexer.CachedLex();
  return Vsblty;
}

StateMutability Parser::parseStateMutability() {
  StateMutability stateMutability(StateMutability::NonPayable);
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_payable:
    stateMutability = StateMutability::Payable;
    break;
  case tok::kw_view:
    stateMutability = StateMutability::View;
    break;
  case tok::kw_pure:
    stateMutability = StateMutability::Pure;
    break;
  case tok::kw_constant:
    stateMutability = StateMutability::View;
    assert(false && "The state mutability modifier \"constant\" was removed in "
                    "version 0.5.0. "
                    "Use \"view\" or \"pure\" instead.");
    break;
  default:
    assert(false && "Invalid state mutability specifier.");
  }
  TheLexer.CachedLex();
  return stateMutability;
}

unique_ptr<ContractDecl> Parser::parseContractDefinition() {
  ContractDecl::ContractKind CtKind = parseContractKind();
  llvm::StringRef Name;
  vector<unique_ptr<InheritanceSpecifier>> BaseContracts;
  vector<unique_ptr<Decl>> SubNodes;
  Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();

  if (TheLexer.LookAhead(0)->is(tok::kw_is)) {
    do {
      TheLexer.CachedLex();
      TheLexer.CachedLex();
      // [TODO] Update vector<InheritanceSpecifier> baseContracts
      BaseContracts.push_back(std::move(std::make_unique<InheritanceSpecifier>(
          TheLexer.LookAhead(0)->getIdentifierInfo()->getName().str(),
          vector<std::unique_ptr<Expr>>())));
    } while ((TheLexer.LookAhead(0))->is(tok::comma));
  }

  TheLexer.CachedLex(); // (
  while (true) {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::r_brace) {
      break;
    }
    // [TODO] < Parse all Types in contract's context >
    if (Kind == tok::kw_function) {
      SubNodes.push_back(
          std::move(parseFunctionDefinitionOrFunctionTypeStateVariable()));
    } else if (Kind == tok::kw_struct) {
      // [TODO] contract tok::kw_struct
    } else if (Kind == tok::kw_enum) {
      // [TODO] contract tok::kw_enum
    } else if (Kind == tok::identifier || Kind == tok::kw_mapping ||
               TheLexer.LookAhead(0)->isElementaryTypeName()) {
      // [TODO] contract tok::identifier, tok::kw_mapping, tok::/Type keywords/
    } else if (Kind == tok::kw_modifier) {
      // [TODO] contract tok::kw_modifier
    } else if (Kind == tok::kw_event) {
      // [TODO] contract tok::kw_event
    } else if (Kind == tok::kw_using) {
      // [TODO] contract tok::kw_using
    } else
      assert(false && "Solidity Error: Function, variable, struct or modifier "
                      "declaration expected.");
  }
  return std::make_unique<ContractDecl>(Name, std::move(BaseContracts),
                                        std::move(SubNodes), CtKind);
}

Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool ForceEmptyName, bool AllowModifiers) {

  FunctionHeaderParserResult Result;

  Result.IsConstructor = false;
  if (TheLexer.CachedLex()->is(tok::kw_constructor)) {
    Result.IsConstructor = true;
  }

  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  if (Result.IsConstructor)
    Result.Name = llvm::StringRef();
  else if (ForceEmptyName || CurTok->is(tok::l_paren))
    Result.Name = llvm::StringRef();
  else
    Result.Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();

  VarDeclParserOptions Options;
  Options.AllowLocationSpecifier = true;

  Result.Parameters = parseParameterList(Options);

  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (AllowModifiers && CurTok->is(tok::identifier)) {
      // [TODO] Function Modifier
    } else if (CurTok->isOneOf(tok::kw_public, tok::kw_private,
                               tok::kw_internal, tok::kw_external)) {
      // [TODO] Special case of a public state variable of function Type.
      Result.Vsblty = parseVisibilitySpecifier();
    } else if (CurTok->isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                               tok::kw_payable)) {
      Result.SM = parseStateMutability();
    } else {
      break;
    }
  }

  if (TheLexer.LookAhead(0)->is(tok::kw_returns)) {
    bool const PermitEmptyParameterList = false;
    TheLexer.CachedLex();
    Result.ReturnParameters =
        parseParameterList(Options, PermitEmptyParameterList);
  } else {
    // [PrePOC] Need return empty parameter list.
    Result.ReturnParameters = nullptr;
  }

  return Result;
}

unique_ptr<FunctionDecl>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult Header = parseFunctionHeader(false, true);
  if (Header.IsConstructor || !Header.Modifiers.empty() ||
      !Header.Name.empty() ||
      TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function
    unique_ptr<Block> block;
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      block = parseBlock();
    }
    return std::make_unique<FunctionDecl>(
        Header.Name, Header.Vsblty, Header.SM, Header.IsConstructor,
        std::move(Header.Parameters), std::move(Header.Modifiers),
        std::move(Header.ReturnParameters), std::move(block));
  } else {
    // [TODO] State Variable case.
    return nullptr;
  }
}

unique_ptr<VarDecl>
Parser::parseVariableDeclaration(VarDeclParserOptions const &Options,
                                 unique_ptr<Type> const &LookAheadArrayType) {
  unique_ptr<Type> T;
  if (LookAheadArrayType) {
    // [TODO] need bug fix below line
    // T = LookAheadArrayType;
  } else {
    T = parseTypeName(Options.AllowVar);
  }

  bool IsIndexed = false;
  bool IsDeclaredConst = false;
  Decl::Visibility Vsblty = Decl::Visibility::Default;
  VarDecl::Location Loc = VarDecl::Location::Unspecified;
  llvm::StringRef Name;
  llvm::Optional<Token> CurTok;
  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (Options.IsStateVariable &&
        CurTok->isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal)) {
      Vsblty = parseVisibilitySpecifier();
    } else {
      if (Options.AllowIndexed && CurTok->is(tok::kw_indexed))
        IsIndexed = true;
      else if (CurTok->is(tok::kw_constant))
        IsDeclaredConst = true;
      else if (Options.AllowLocationSpecifier &&
               CurTok->isOneOf(tok::kw_memory, tok::kw_storage,
                               tok::kw_calldata)) {
        if (Loc != VarDecl::Location::Unspecified)
          assert(false && "Location already specified.");
        else if (!T)
          assert(false && "Location specifier needs explicit type name.");
        else {
          switch (TheLexer.LookAhead(0)->getKind()) {
          case tok::kw_storage:
            Loc = VarDecl::Location::Storage;
            break;
          case tok::kw_memory:
            Loc = VarDecl::Location::Memory;
            break;
          case tok::kw_calldata:
            Loc = VarDecl::Location::CallData;
            break;
          default:
            assert(false && "Unknown data location.");
          }
        }
      } else
        break;
      TheLexer.CachedLex();
    }
  }

  if (Options.AllowEmptyName && TheLexer.LookAhead(0)->isNot(tok::identifier)) {
    Name = llvm::StringRef("");
  } else {
    Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();
  }

  // [TODO] Handle variable with init value
  return std::make_unique<VarDecl>(std::move(T), Name, nullptr, Vsblty,
                                   Options.IsStateVariable, IsIndexed,
                                   IsDeclaredConst);
}

unique_ptr<Type> Parser::parseTypeNameSuffix(unique_ptr<Type> T) {
  while (TheLexer.LookAhead(0)->is(tok::l_square)) {
    TheLexer.CachedLex();
    unique_ptr<Expr> Length;
    Length = parseExpression();
    TheLexer.CachedLex();
  }
  return T;
}

// [TODO] < Need complete all Types >
unique_ptr<Type> Parser::parseTypeName(bool AllowVar) {
  unique_ptr<Type> T;
  bool HaveType = false;
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  tok::TokenKind Kind = CurTok->getKind();
  if (true) // (TokenTraits::isElementaryTypeName(token))
  {
    // [TODO] parseTypeName handle address case
    HaveType = true;
  } else if (Kind == tok::kw_var) {
    // [TODO] parseTypeName tok::kw_var (var is deprecated)
  } else if (Kind == tok::kw_function) {
    // [TODO] parseTypeName tok::kw_function
  } else if (Kind == tok::kw_mapping) {
    // [TODO] parseTypeName tok::kw_mapping
  } else if (Kind == tok::identifier) {
    // [TODO] parseTypeName tok::identifier
  } else
    assert(false && "Expected Type Name");

  if (HaveType) {
    T = parseTypeNameSuffix(move(T));
  }
  return T;
}

unique_ptr<ParamList>
Parser::parseParameterList(VarDeclParserOptions const &_Options,
                           bool AllowEmpty) {
  vector<unique_ptr<VarDecl>> Parameters;
  VarDeclParserOptions Options(_Options);
  Options.AllowEmptyName = true;
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    do {
      TheLexer.CachedLex();
      Parameters.push_back(parseVariableDeclaration(Options));
    } while (TheLexer.LookAhead(0)->is(tok::comma));
    TheLexer.CachedLex();
  }
  return std::make_unique<ParamList>(std::move(Parameters));
  ;
}

unique_ptr<Block> Parser::parseBlock() {
  vector<unique_ptr<Stmt>> Statements;
  TheLexer.CachedLex();
  while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
    Statements.push_back(parseStatement());
  }
  TheLexer.CachedLex();
  return std::make_unique<Block>(std::move(Statements));
}

// [TODO] < Parse all statements >
unique_ptr<Stmt> Parser::parseStatement() {
  unique_ptr<Stmt> Statement;
  llvm::Optional<Token> CurTok;
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_if:
    return parseIfStatement();
  case tok::kw_while:
    // [TODO] parseStatement kw_while
    break;
  case tok::kw_do:
    // [TODO] parseStatement kw_do
    break;
  case tok::kw_for:
    // [TODO] parseStatement kw_for
    break;
  case tok::l_brace:
    return parseBlock();
    break;
  case tok::kw_continue:
    // [TODO] parseStatement kw_continue
    break;
  case tok::kw_break:
    // [TODO] parseStatement kw_break
    break;
  case tok::kw_return:
    TheLexer.CachedLex();
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      Statement = std::make_unique<ReturnStmt>(std::move(parseExpression()));
    }
    break;
  case tok::kw_assembly:
    // [TODO] parseStatement kw_assembly
    break;
  case tok::kw_emit:
    // [TODO] parseStatement kw_emit
    break;
  case tok::identifier:
  default:
    Statement = parseSimpleStatement();
    break;
  }
  TheLexer.CachedLex();
  return Statement;
}

unique_ptr<IfStmt> Parser::parseIfStatement() {
  TheLexer.CachedLex(); // if
  TheLexer.CachedLex(); // (
  unique_ptr<Expr> Condition = parseExpression();
  TheLexer.CachedLex(); // )
  unique_ptr<Stmt> TrueBody = parseStatement();
  unique_ptr<Stmt> FalseBody;
  if (TheLexer.LookAhead(0)->is(tok::kw_else)) {
    TheLexer.CachedLex();
    FalseBody = parseStatement();
  }
  return std::make_unique<IfStmt>(std::move(Condition), std::move(TrueBody),
                                  std::move(FalseBody));
}

unique_ptr<Stmt> Parser::parseSimpleStatement() {
  llvm::Optional<Token> CurTok;
  LookAheadInfo StatementType;
  IndexAccessedPath Iap;
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    // [TODO] parseSimpleStatement a simple statement begin with '{'
  } else {
    tie(StatementType, Iap) = tryParseIndexAccessedPath();
    switch (StatementType) {
    case LookAheadInfo::VariableDeclaration:
      return parseVariableDeclarationStatement(
          typeNameFromIndexAccessStructure(Iap));
    case LookAheadInfo::Expression:
      return std::move(parseExpression(
          std::move(expressionFromIndexAccessStructure(Iap))));
    default:
      assert(false && "Unhandle statement.");
    }
  }
  return nullptr;
}

unique_ptr<DeclStmt> Parser::parseVariableDeclarationStatement(
    unique_ptr<Type> const &LookAheadArrayType) {
  // This does not parse multi variable declaration statements starting directly
  // with
  // `(`, they are parsed in parseSimpleStatement, because they are hard to
  // distinguish from tuple expressions.
  vector<unique_ptr<Decl>> Variables;
  unique_ptr<Expr> Value;
  if (!LookAheadArrayType && TheLexer.LookAhead(0)->is(tok::kw_var) &&
      TheLexer.LookAhead(0)->is(tok::l_paren)) {
    // [0.4.20] The var keyword has been deprecated for security reasons.
    // https://github.com/ethereum/solidity/releases/tag/v0.4.20
    assert(false &&
           "The var keyword has been deprecated for security reasons.");
  } else {
    VarDeclParserOptions Options;
    Options.AllowVar = false;
    Options.AllowLocationSpecifier = true;
    Variables.push_back(parseVariableDeclaration(Options, LookAheadArrayType));
  }
  if (TheLexer.LookAhead(0)->is(tok::equal)) {
    TheLexer.CachedLex();
    Value = parseExpression();
  }

  // [TEMP] Need fix.
  //return std::move(std::make_unique<DeclStmt>());
  return std::move(
    std::make_unique<DeclStmt>(std::move(Variables), std::move(Value)));
}

pair<Parser::LookAheadInfo, Parser::IndexAccessedPath>
Parser::tryParseIndexAccessedPath() {
  // These two cases are very hard to distinguish:
  // x[7 * 20 + 3] a;     and     x[7 * 20 + 3] = 9;
  // In the first case, x is a type name, in the second it is the name of a
  // variable. As an extension, we can even have: `x.y.z[1][2] a;` and
  // `x.y.z[1][2] = 10;` Where in the first, x.y.z leads to a type name where in
  // the second, it accesses structs.

  auto StatementType = peekStatementType();

  switch (StatementType) {
  case LookAheadInfo::VariableDeclaration:
  case LookAheadInfo::Expression:
    return make_pair(StatementType, IndexAccessedPath());
  default:
    break;
  }

  // [TODO] IAP complex case. ex. name[idx], name.name2, typename [3]
  /*
  // At this point, we have 'Identifier "["' or 'Identifier "." Identifier' or
  'ElementoryTypeName "["'.
  // We parse '(Identifier ("." Identifier)* |ElementaryTypeName) ( "["
  Expression "]" )*'
  // until we can decide whether to hand this over to ExpressionStatement or
  create a
  // VariableDeclarationStatement out of it.
  IndexAccessedPath Iap = parseIndexAccessedPath();

  if (m_scanner->currentToken() == Token::Identifier ||
  TokenTraits::isLocationSpecifier(m_scanner->currentToken())) return
  make_pair(LookAheadInfo::VariableDeclaration, move(Iap)); else return
  make_pair(LookAheadInfo::Expression, move(Iap));
  */
  return make_pair(StatementType, IndexAccessedPath());
}

Parser::LookAheadInfo Parser::peekStatementType() const {
  // Distinguish between variable declaration (and potentially assignment) and
  // expression statement (which include assignments to other expressions and
  // pre-declared variables). We have a variable declaration if we get a keyword
  // that specifies a type name. If it is an identifier or an elementary type
  // name followed by an identifier or a mutability specifier, we also have a
  // variable declaration. If we get an identifier followed by a "[" or ".", it
  // can be both ("lib.type[9] a;" or "variable.el[9] = 7;"). In all other
  // cases, we have an expression statement.
  llvm::Optional<Token> CurTok, NextTok;
  CurTok = TheLexer.LookAhead(0);
  bool MightBeTypeName =
      CurTok->isElementaryTypeName() || CurTok->is(tok::identifier);
  if (CurTok->isOneOf(tok::kw_mapping, tok::kw_function, tok::kw_var))
    return LookAheadInfo::VariableDeclaration;

  if (MightBeTypeName) {
    NextTok = TheLexer.LookAhead(1);
    // So far we only allow ``address payable`` in variable declaration
    // statements and in no other kind of statement. This means, for example,
    // that we do not allow type expressions of the form
    // ``address payable;``.
    // If we want to change this in the future, we need to consider another
    // scanner token here.
    if (CurTok->isElementaryTypeName() &&
        NextTok->isOneOf(tok::kw_pure, tok::kw_view, tok::kw_payable)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok->is(tok::identifier) ||
        NextTok->isOneOf(tok::kw_memory, tok::kw_storage, tok::kw_calldata)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok->isOneOf(tok::l_square, tok::period)) {
      return LookAheadInfo::IndexAccessStructure;
    }
  }
  return LookAheadInfo::Expression;
}

// [TODO] IAP relative function
unique_ptr<Type>
Parser::typeNameFromIndexAccessStructure(Parser::IndexAccessedPath const &Iap) {
  return {};
}

// [TODO] IAP relative function
unique_ptr<Expr> Parser::expressionFromIndexAccessStructure(
    Parser::IndexAccessedPath const &Iap) {
  return {};
}

unique_ptr<Expr>
Parser::parseExpression(unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Expression =
      parseBinaryExpression(4, std::move(PartiallyParsedExpression));
  if (TheLexer.LookAhead(0)->is(tok::equal)) {
    unique_ptr<Expr> RightHandSide = parseExpression();
    return std::make_unique<BinaryOperator>(std::move(Expression),
                                            std::move(RightHandSide),
                                            BinaryOperatorKind::BO_Assign);
  } else if (TheLexer.LookAhead(0)->is(tok::question)) {
    TheLexer.CachedLex();
    unique_ptr<Expr> trueExpression = parseExpression();
    TheLexer.CachedLex();
    unique_ptr<Expr> falseExpression = parseExpression();
    // [TODO] Create ConditionExpression
    return nullptr;
  } else
    return Expression;
}

unique_ptr<Expr>
Parser::parseBinaryExpression(int MinPrecedence,
                              unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Expression =
      parseUnaryExpression(std::move(PartiallyParsedExpression));
  // [PrePOC] Need op precedence. Now assume all op precedence = 5, little
  // bigger than default 4
  if (TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::comma, tok::r_paren))
    return Expression;

  int Precedence = 5;
  for (; Precedence >= MinPrecedence; --Precedence) {
    while (
        !TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::comma, tok::r_paren)) {
      // [PrePOC] Fix this token recognition method
      BinaryOperatorKind Op = token2bop(TheLexer.LookAhead(0));
      TheLexer.CachedLex();
      unique_ptr<Expr> RightHandSide = parseBinaryExpression(Precedence + 1);
      Expression = std::make_unique<BinaryOperator>(
          std::move(Expression), std::move(RightHandSide), Op);
    }
  }
  return Expression;
}

unique_ptr<Expr>
Parser::parseUnaryExpression(unique_ptr<Expr> &&PartiallyParsedExpression) {
  UnaryOperatorKind Op = token2uop(TheLexer.LookAhead(0));

  // [PrePOC] Fix this token recognition method
  // if (!PartiallyParsedExpression && (Op->isUnaryOp() || Op->isCountOp())) {
  if (!PartiallyParsedExpression && (Op != UnaryOperatorKind::UO_Undefined)) {
    // prefix expression
    TheLexer.CachedLex();
    unique_ptr<Expr> SubExps = parseUnaryExpression();
    return std::make_unique<UnaryOperator>(std::move(SubExps), Op);
  } else {
    // potential postfix expression
    unique_ptr<Expr> SubExps =
        parseLeftHandSideExpression(std::move(PartiallyParsedExpression));
    Op = token2uop(TheLexer.LookAhead(0));
    if (!(Op == UnaryOperatorKind::UO_PostInc ||
          Op == UnaryOperatorKind::UO_PostDec))
      return SubExps;
    TheLexer.CachedLex();
    return std::make_unique<UnaryOperator>(std::move(SubExps), Op);
  }
}

unique_ptr<Expr> Parser::parseLeftHandSideExpression(
    unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Exps;
  if (PartiallyParsedExpression)
    Exps = std::move(PartiallyParsedExpression);
  else if (TheLexer.LookAhead(0)->is(tok::kw_new)) {
    TheLexer.CachedLex();
    unique_ptr<Type> typeName = parseTypeName(false);
    // [AST] create NewExpression
  } else
    Exps = std::move(parsePrimaryExpression());

  while (true) {
    switch (TheLexer.LookAhead(0)->getKind()) {
    case tok::l_square: {
      TheLexer.CachedLex();
      unique_ptr<Expr> Index;
      if (TheLexer.LookAhead(0)->isNot(tok::r_square))
        Index = std::move(parseExpression());
      TheLexer.CachedLex();
      // [AST] Create IndexAccess Expression
      break;
    }
    case tok::period: {
      TheLexer.CachedLex();
      // [AST] Create MemberAccess Expression
      break;
    }
    case tok::l_paren: {
      TheLexer.CachedLex();
      vector<unique_ptr<Expr>> Arguments;
      vector<unique_ptr<string>> Names;
      tie(Arguments, Names) = parseFunctionCallArguments();
      TheLexer.CachedLex();
      // [TODO] Fix passs arguments' name fail.
      Exps = std::make_unique<CallExpr>(std::move(Exps), std::move(Arguments));
      break;
    }
    default:
      return Exps;
    }
  }
}

unique_ptr<Expr> Parser::parsePrimaryExpression() {
  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  unique_ptr<Expr> Exps;

  switch (CurTok->getKind()) {
  case tok::kw_true:
    Exps = std::make_unique<BooleanLiteral>(true);
    break;
  case tok::kw_false:
    Exps = std::make_unique<BooleanLiteral>(false);
    break;
  case tok::numeric_constant: {
    int NumValue;
    getLiteralAndAdvance(CurTok).getAsInteger(0, NumValue);
    Exps = std::make_unique<NumberLiteral>(NumValue);
    break;
  }
  case tok::string_literal: {
    string StrValue = getLiteralAndAdvance(CurTok).str();
    Exps = make_unique<StringLiteral>(std::move(StrValue));
    break;
  }
  case tok::identifier: {
    string Name = getLiteralAndAdvance(CurTok).str();
    Exps = make_unique<Identifier>(std::move(Name));
    break;
  }
  case tok::kw_type:
    // [TODO] Type expression is globally-avariable function
    TheLexer.CachedLex();
    break;
  case tok::l_paren:
  case tok::l_square: {
    // [TODO] Tuple case
    // Tuple/parenthesized expression or inline array/bracketed expression.
    // Special cases: ()/[] is empty tuple/array type, (x) is not a real tuple,
    // (x,) is one-dimensional tuple, elements in arrays cannot be left out,
    // only in tuples.
    TheLexer.CachedLex();
    vector<unique_ptr<Expr>> Components;
    tok::TokenKind OppositeToken =
        (CurTok->is(tok::l_paren) ? tok::r_paren : tok::r_square);
    bool IsArray = (OppositeToken == tok::l_square);

    if (TheLexer.LookAhead(0)->isNot(OppositeToken))

      while (true) {
        if (!TheLexer.LookAhead(0)->isOneOf(tok::comma, OppositeToken)) {
          Components.push_back(parseExpression());
        } else if (IsArray)
          assert(
              false &&
              "Expected expression (inline array elements cannot be omitted).");
        else
          Components.push_back(nullptr);

        if (TheLexer.LookAhead(0)->is(OppositeToken)) {
          break;
        }
        TheLexer.CachedLex();
      }
    TheLexer.CachedLex();
    // [AST] Create TupleExpression
    break;
  }
  case tok::unknown:
    assert(false && "Unknown token");
    break;
  default:
    // [TODO] Type MxN case
    /*
    if (CurTok->isElementaryTypeName())
    {
      //used for casts
      unsigned FirstSize;
      unsigned SecondSize;
      tie(FirstSize, SecondSize) = m_scanner->currentTokenInfo();
      ElementaryTypeNameToken elementaryExpression(m_scanner->currentToken(),
    firstSize, secondSize); expression =
    nodeFactory.createNode<ElementaryTypeNameExpression>(elementaryExpression);
      m_scanner->next();
    }
    else
      assert(false && "Expected primary expression.");
    */
    assert(false && "Expected primary expression.");
    break;
  }
  return Exps;
}

vector<unique_ptr<Expr>> Parser::parseFunctionCallListArguments() {
  vector<unique_ptr<Expr>> Arguments;
  if (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
    Arguments.push_back(parseExpression());
    while (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
      TheLexer.CachedLex();
      Arguments.push_back(parseExpression());
    }
  }
  return Arguments;
}

pair<vector<unique_ptr<Expr>>, vector<unique_ptr<string>>>
Parser::parseFunctionCallArguments() {
  pair<vector<unique_ptr<Expr>>, vector<unique_ptr<string>>> Ret;
  if (TheLexer.LookAhead(0)->is(tok::l_brace)) {
    // [TODO] Unverified function parameters case
    // call({arg1 : 1, arg2 : 2 })
    TheLexer.CachedLex();
    bool First = true;
    while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
      if (!First)
        TheLexer.CachedLex();

      Ret.second.push_back(expectIdentifierToken());
      TheLexer.CachedLex();
      Ret.first.push_back(parseExpression());

      if (TheLexer.LookAhead(0)->is(tok::comma) &&
          TheLexer.LookAhead(1)->is(tok::r_brace)) {
        assert(false && "Unexpected trailing comma.");
        TheLexer.CachedLex();
      }
      First = false;
    }
    TheLexer.CachedLex();
  } else
    Ret.first = std::move(parseFunctionCallListArguments());
  return Ret;
}

unique_ptr<AST> Parser::createEmptyParameterList() { return nullptr; }

unique_ptr<string> Parser::expectIdentifierToken() { return nullptr; }

llvm::StringRef Parser::getLiteralAndAdvance(llvm::Optional<Token> Tok) {
  TheLexer.CachedLex();
  if (Tok->is(tok::identifier))
    return Tok->getIdentifierInfo()->getName();
  else
    return llvm::StringRef(Tok->getLiteralData(), Tok->getLength());
}

} // namespace soll
