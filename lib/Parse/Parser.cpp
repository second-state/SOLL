#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/Lex/Lexer.h"
#include <iostream>

using namespace std;

namespace soll {

static int indent(int update) {
  static int _indent = 0;
  _indent += update;
  return _indent >= 0 ? _indent : 0;
}

Parser::Parser(Lexer &lexer) : TheLexer(lexer) {}

shared_ptr<AST> Parser::parse() {
  llvm::Optional<Token> CurTok;
  vector<shared_ptr<AST>> Nodes;

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
  return nullptr;
}

shared_ptr<AST> Parser::parsePragmaDirective() {
  // pragma anything* ;
  // Currently supported:
  // pragma solidity ^0.4.0 || ^0.3.0;
  vector<string> Literals;
  vector<llvm::Optional<Token>> Tokens;
  TheLexer.CachedLex();
  do {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::unknown)
      assert("Solidity Error: Token incompatible with Solidity parser as part "
             "of pragma directive.");
    else if (Kind == tok::caret) {
      // [TODO] Fix tok::caret no literal, but not sure what means
      // Pattern not match Solidity : Solidity | ^ | 0.5 | .0 |
      //                   Soll     : Solidity | 0.5.0
      TheLexer.CachedLex();
    } else {
      string literal = getLiteralAndAdvance(TheLexer.LookAhead(0)).str();
      // [Integration TODO] printf("%*s%s\n", indent(0), "", literal.c_str());
      Literals.push_back(literal);
      Tokens.push_back(TheLexer.LookAhead(0));
    }
  } while (!TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::eof));
  TheLexer.CachedLex();

  // [TODO] Implement version recognize and compare. ref: parsePragmaVersion
  return nullptr;
}

shared_ptr<AST> Parser::parseContractDefinition() {
  TheLexer.CachedLex(); // contract
  shared_ptr<string> Name = nullptr;
  vector<shared_ptr<AST>> SubNodes;
  Name = make_shared<string>(
      TheLexer.CachedLex()->getIdentifierInfo()->getName().str());

  // [Integration TODO] printf("%*sContract:%s\n", indent(0), "", Name->c_str());

  if (TheLexer.LookAhead(0)->is(tok::kw_is)) {
    do {
      TheLexer.CachedLex();
      TheLexer.CachedLex();
      // [Integration TODO] printf("%*sInheritance:%s\n", indent(0), "", TheLexer.LookAhead(0)->getIdentifierInfo()->getName().str().c_str());
      // [TODO] Update vector<InheritanceSpecifier> baseContracts
    } while ((TheLexer.LookAhead(0))->is(tok::comma));
  }

  TheLexer.CachedLex();
  while (true) {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::r_brace) {
      break;
    }
    // [TODO] < Parse all Types in contract's context >
    if (Kind == tok::kw_function) {
      // [Integration TODO] indent(2);
      SubNodes.push_back(parseFunctionDefinitionOrFunctionTypeStateVariable());
      // [Integration TODO] indent(-2);
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
      // [TODO]  contract tok::kw_using
    } else
      assert("Solidity Error: Function, variable, struct or modifier "
             "declaration expected.");
  }
  return nullptr;
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
    Result.Name = make_shared<string>();
  else if (ForceEmptyName || CurTok->is(tok::l_paren))
    Result.Name = make_shared<string>();
  else
    Result.Name = make_shared<string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName().str());
  // [Integration TODO] printf("%*sFunction:%s\n", indent(0), "", Result.Name->c_str());

  VarDeclParserOptions Options;
  Options.AllowLocationSpecifier = true;

  // [Integration TODO] indent(2);
  Result.Parameters = parseParameterList(Options);
  // [Integration TODO] indent(-2);

  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (AllowModifiers && CurTok->is(tok::identifier)) {
      // [TODO] Function Modifier
    } else if (CurTok->isOneOf(tok::kw_public, tok::kw_private,
                               tok::kw_internal, tok::kw_external)) {
      // [TODO] Special case of a public state variable of function Type.
      Result.Visibility = TheLexer.CachedLex()->getName();
      // [Integration TODO] printf("%*sVisibility:%s\n", indent(0), "", Result.Visibility);
    } else if (CurTok->isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                               tok::kw_payable)) {
      Result.StateMutability = TheLexer.CachedLex()->getName();
      // [Integration TODO] printf("%*sStateMutability:%s\n", indent(0), "", Result.StateMutability);
    } else {
      break;
    }
  }

  if (TheLexer.LookAhead(0)->is(tok::kw_returns)) {
    bool const PermitEmptyParameterList = false;
    TheLexer.CachedLex();
    // [Integration TODO] indent(2);
    Result.ReturnParameters =
        parseParameterList(Options, PermitEmptyParameterList);
    // [Integration TODO] indent(-2);
  } else {
    Result.ReturnParameters.clear();
  }

  return Result;
}

shared_ptr<AST>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult Header = parseFunctionHeader(false, true);
  if (Header.IsConstructor || !Header.Modifiers.empty() ||
      !Header.Name->empty() ||
      TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function
    shared_ptr<AST> block = shared_ptr<AST>();
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      // [Integration TODO] indent(2);
      block = parseBlock();
      // [Integration TODO] indent(-2);
    }
  } else {
    // [TODO] State Variable case.
  }
  return nullptr;
}

shared_ptr<AST> Parser::parseVariableDeclaration(
    VarDeclParserOptions const &Options,
    shared_ptr<AST> const &LookAheadArrayType) {
  shared_ptr<AST> Type;
  if (LookAheadArrayType) {
    Type = LookAheadArrayType;
  } else {
    Type = parseTypeName(Options.AllowVar);
  }

  bool IsIndexed = false;
  bool IsDeclaredConst = false;
  const char *Visibility = "default";
  const char *Location = "Unspecified";
  shared_ptr<string> Identifier;

  llvm::Optional<Token> CurTok;
  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (Options.IsStateVariable &&
        CurTok->isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal)) {
      Visibility = TheLexer.CachedLex()->getName();
    } else {
      if (Options.AllowIndexed && CurTok->is(tok::kw_indexed))

        IsIndexed = true;
      else if (CurTok->is(tok::kw_constant))
        IsDeclaredConst = true;
      else if (Options.AllowLocationSpecifier &&
               CurTok->isOneOf(tok::kw_memory, tok::kw_storage,
                               tok::kw_calldata)) {
        Location = CurTok->getName();
      } else {
        break;
      }
    }
  }

  if (Options.AllowEmptyName && TheLexer.LookAhead(0)->isNot(tok::identifier)) {
    Identifier = make_shared<string>("");
  } else {
    Identifier = make_shared<string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName());
  }
  // [Integration TODO] printf("%*sVariableName:%s\n", indent(0), "", Identifier->c_str());

  // [TODO] Handle variable with init value
  return nullptr;
}

shared_ptr<AST> Parser::parseTypeNameSuffix(shared_ptr<AST> Type) {
  while (TheLexer.LookAhead(0)->is(tok::l_square)) {
    TheLexer.CachedLex();
    shared_ptr<AST> Length;
    // [Integration TODO] printf("[");
    Length = parseExpression();
    // [Integration TODO] printf("]");
    TheLexer.CachedLex();
  }
  return Type;
}

// [TODO] < Need complete all Types >
shared_ptr<AST> Parser::parseTypeName(bool AllowVar) {
  shared_ptr<AST> Type;
  bool HaveType = false;
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  tok::TokenKind Kind = CurTok->getKind();
  if (true) // (TokenTraits::isElementaryTypeName(token))
  {
    // [TODO] parseTypeName handle address case
    // [Integration TODO] printf("%*sType:%s", indent(0), "", CurTok->getName());
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
    assert("Expected Type Name");

  if (HaveType) {
    Type = parseTypeNameSuffix(Type);
    // [Integration TODO] printf("\n");
  }

  return Type;
}

vector<shared_ptr<AST>>
Parser::parseParameterList(VarDeclParserOptions const &_Options,
                           bool AllowEmpty) {
  vector<shared_ptr<AST>> Parameters;

  VarDeclParserOptions Options(_Options);
  Options.AllowEmptyName = true;
  // [Integration TODO] printf("%*sParameters:%s\n", indent(0), "", "");
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    do {
      TheLexer.CachedLex();
      // [Integration TODO] indent(2);
      Parameters.push_back(parseVariableDeclaration(Options));
      // [Integration TODO] indent(-2);
    } while (TheLexer.LookAhead(0)->is(tok::comma));
    TheLexer.CachedLex();
  }
  return Parameters;
}

shared_ptr<AST> Parser::parseBlock() {
  vector<shared_ptr<AST>> Statements;
  TheLexer.CachedLex();
  // [Integration TODO] printf("%*sBlock:%s\n", indent(0), "", "");
  while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
    Statements.push_back(parseStatement());
  }
  TheLexer.CachedLex();
  return nullptr;
}

// [TODO] < Parse all statements >
shared_ptr<AST> Parser::parseStatement() {
  shared_ptr<AST> statement;
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
    // [TODO] parseStatement kw_do
    break;
  case tok::l_brace:
    return parseBlock();
    break;
  case tok::kw_continue:
    // [TODO] parseStatement kw_do
    break;
  case tok::kw_break:
    // [TODO] parseStatement kw_do
    break;
  case tok::kw_return:
    // [Integration TODO] indent(2);
    // [Integration TODO] printf("%*sReturn statement:%s\n", indent(0), "", "");
    TheLexer.CachedLex();
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      // [Integration TODO] indent(2);
      parseExpression();
    }
    // [Integration TODO] indent(-4);
    break;
  case tok::kw_assembly:
    // [TODO] parseStatement kw_do
    break;
  case tok::kw_emit:
    // [TODO] parseStatement kw_do
    break;
  case tok::identifier:
  default:
    // [Integration TODO] indent(2);
    // [Integration TODO] printf("%*sSimple statement:%s\n", indent(0), "", "");
    statement = parseSimpleStatement();
    // [Integration TODO] indent(-2);
    break;
  }
  TheLexer.CachedLex();
  return statement;
}

shared_ptr<AST> Parser::parseIfStatement() {
  // [Integration TODO] printf("%*sIf Statement:%s\n", indent(0), "", "");
  TheLexer.CachedLex(); // if
  TheLexer.CachedLex(); //(
  // [Integration TODO] indent(2);
  // [Integration TODO] printf("%*sIf:%s\n", indent(0), "", "");
  // [Integration TODO] indent(2);
  shared_ptr<AST> Condition = parseExpression();
  // [Integration TODO] indent(-2);
  shared_ptr<AST> TrueBody = parseStatement();
  // [Integration TODO] indent(-2);

  shared_ptr<AST> FalseBody;
  if (TheLexer.LookAhead(0)->is(tok::kw_else)) {
    TheLexer.CachedLex();
    // [Integration TODO] indent(2);
    // [Integration TODO] printf("%*sElse%s\n", indent(0), "", "");
    FalseBody = parseStatement();
    // [Integration TODO] indent(-2);
  }
  // [Integration TODO] indent(-2);
  return nullptr;
}

shared_ptr<AST> Parser::parseSimpleStatement() {
  llvm::Optional<Token> CurTok;
  LookAheadInfo StatementType;
  IndexAccessedPath Iap;
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    // [TODO] parseSimpleStatement a simple statement begin with '{'
    TheLexer.CachedLex();
    size_t EmptyComponents = 0;
    // First consume all empty components.
    while (TheLexer.LookAhead(0)->is(tok::comma)) {
      TheLexer.CachedLex();
      EmptyComponents++;
    }
    return nullptr;
  } else {
    tie(StatementType, Iap) = tryParseIndexAccessedPath();
    switch (StatementType) {
    case LookAheadInfo::VariableDeclaration:
      return parseVariableDeclarationStatement(
          typeNameFromIndexAccessStructure(Iap));
    case LookAheadInfo::Expression:
      return parseExpressionStatement(expressionFromIndexAccessStructure(Iap));
    default:
      assert("Unhandle statement.");
    }
  }
  return nullptr;
}

shared_ptr<AST> Parser::parseVariableDeclarationStatement(
    shared_ptr<AST> const &LookAheadArrayType) {
  // [Integration TODO] printf("%*sVariableDeclarationStatement:%s\n", indent(0), "", "");
  // [Integration TODO] indent(2);
  // This does not parse multi variable declaration statements starting directly
  // with
  // `(`, they are parsed in parseSimpleStatement, because they are hard to
  // distinguish from tuple expressions.
  vector<shared_ptr<AST>> Variables;
  shared_ptr<AST> Value;
  if (!LookAheadArrayType && TheLexer.LookAhead(0)->is(tok::kw_var) &&
      TheLexer.LookAhead(0)->is(tok::l_paren)) {
    // [0.4.20] The var keyword has been deprecated for security reasons.
    // https://github.com/ethereum/solidity/releases/tag/v0.4.20
    assert("The var keyword has been deprecated for security reasons.");
  } else {
    VarDeclParserOptions Options;
    Options.AllowVar = false;
    Options.AllowLocationSpecifier = true;
    Variables.push_back(parseVariableDeclaration(Options, LookAheadArrayType));
  }
  if (TheLexer.LookAhead(0)->is(tok::equal)) {
    TheLexer.CachedLex();
    // [Integration TODO] printf("%*s%s\n", indent(0), "", "binary op (=)");
    // [Integration TODO] indent(2);
    Value = parseExpression();
    // [Integration TODO] indent(-2);
  }
  // [Integration TODO] indent(-2);
  return nullptr;
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
shared_ptr<AST>
Parser::typeNameFromIndexAccessStructure(Parser::IndexAccessedPath const &Iap) {
  return {};
}

// [TODO] IAP relative function
shared_ptr<AST> Parser::expressionFromIndexAccessStructure(
    Parser::IndexAccessedPath const &Iap) {
  return {};
}

shared_ptr<AST> Parser::parseExpressionStatement(
    shared_ptr<AST> const &PartialParserResult) {
  // [Integration TODO] printf("%*sExpressionStatement:%s\n", indent(0), "", "");
  // [Integration TODO] indent(2);
  shared_ptr<AST> Exps = parseExpression(PartialParserResult);
  // [Integration TODO] indent(-2);
  return Exps;
}

shared_ptr<AST>
Parser::parseExpression(shared_ptr<AST> const &PartiallyParsedExpression) {
  shared_ptr<AST> expression =
      parseBinaryExpression(4, PartiallyParsedExpression);
  // TheLexer.CachedLex();
  if (TheLexer.LookAhead(0)->is(tok::equal)) {
    // [Integration TODO] printf("%*s%s\n", indent(0), "", "binary op (=)");
    shared_ptr<AST> rightHandSide = parseExpression();
    // [AST] Create BinaryExpression
    return nullptr;
  } else if (TheLexer.LookAhead(0)->is(tok::question)) {
    // [Integration TODO] printf("%*s%s\n", indent(0), "", "condition op (?)");
    TheLexer.CachedLex();
    shared_ptr<AST> trueExpression = parseExpression();
    TheLexer.CachedLex();
    shared_ptr<AST> falseExpression = parseExpression();
    // [AST] Create ConditionExpression
    return nullptr;
  } else
    return nullptr;
}

shared_ptr<AST> Parser::parseBinaryExpression(
    int MinPrecedence, shared_ptr<AST> const &PartiallyParsedExpression) {
  shared_ptr<AST> Exps = parseUnaryExpression(PartiallyParsedExpression);
  // [PrePOC] Need op precedence. Now assume all op precedence left's = right's
  // - 1 and minimal is 5 (bigger than default 4).
  if (TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::comma, tok::r_paren))
    return Exps;

  int Precedence = 5;
  for (; Precedence >= MinPrecedence; --Precedence) {
    while (
        !TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::comma, tok::r_paren)) {
      llvm::Optional<Token> Op = TheLexer.LookAhead(0);
      // [Integration TODO] printf("%*s%s (%s)\n", indent(0), "", "binary op", Op->getName());
      TheLexer.CachedLex();
      // [Integration TODO] indent(2);
      shared_ptr<AST> right = parseBinaryExpression(Precedence + 1);
      // [Integration TODO] indent(-2);
      // [AST] Create BinaryExpression
    }
  }
  return Exps;
}

shared_ptr<AST> Parser::parseUnaryExpression(
    shared_ptr<AST> const &PartiallyParsedExpression) {
  llvm::Optional<Token> Op = TheLexer.LookAhead(0);
  if (!PartiallyParsedExpression && (Op->isUnaryOp() || Op->isCountOp())) {
    // [Integration TODO] printf("%*sUnaryExpression:%s\n", indent(0), "", Op->getName());
    // prefix expression
    TheLexer.CachedLex();
    // [Integration TODO] indent(2);
    shared_ptr<AST> SubExps = parseUnaryExpression();
    // [AST] Create UnaryExpression
    // [Integration TODO] indent(-2);
    return nullptr;
  } else {
    // potential postfix expression
    shared_ptr<AST> SubExps =
        parseLeftHandSideExpression(PartiallyParsedExpression);
    llvm::Optional<Token> Op = TheLexer.LookAhead(0);
    if (!Op->isOneOf(tok::plusplus, tok::minusminus))
      return nullptr;
    // [Integration TODO] printf("%*sPostfixExpression:%s\n", indent(0), "", Op->getName());
    TheLexer.CachedLex();
    return nullptr;
  }
}

shared_ptr<AST> Parser::parseLeftHandSideExpression(
    shared_ptr<AST> const &PartiallyParsedExpression) {
  shared_ptr<AST> Exps;
  if (PartiallyParsedExpression)
    Exps = PartiallyParsedExpression;
  else if (TheLexer.LookAhead(0)->is(tok::kw_new)) {
    // [Integration TODO] printf("%*sNewExpression:%s\n", indent(0), "", "");
    TheLexer.CachedLex();
    shared_ptr<AST> typeName;
    // [Integration TODO] indent(2);
    parseTypeName(false);
    // [Integration TODO] indent(-2);
    // [AST] create NewExpression
  } else
    Exps = parsePrimaryExpression();

  while (true) {
    switch (TheLexer.LookAhead(0)->getKind()) {
    case tok::l_square: {
      // [Integration TODO] printf("%*sIndexAccessExpression:%s\n", indent(0), "", "");
      TheLexer.CachedLex();
      shared_ptr<AST> Index;
      if (TheLexer.LookAhead(0)->isNot(tok::r_square))
        Index = parseExpression();
      TheLexer.CachedLex();
      // [AST] Create IndexAccess Expression
      break;
    }
    case tok::period: {
      // [Integration TODO] printf("%*sMemberAccessExpression:%s\n", indent(0), "", "");
      TheLexer.CachedLex();
      // [Integration TODO] indent(2);
      // [Integration TODO] printf("%*sIdentifier:%s\n", indent(0), "", getLiteralAndAdvance(TheLexer.LookAhead(0)).str().c_str());
      // [Integration TODO] indent(-2);
      // [AST] Create MemberAccess Expression
      break;
    }
    case tok::l_paren: {
      // [Integration TODO] printf("%*sFunctionCallExpression:%s\n", indent(0), "", "");
      TheLexer.CachedLex();
      vector<shared_ptr<AST>> Arguments;
      vector<shared_ptr<string>> Names;
      // [Integration TODO] indent(2);
      tie(Arguments, Names) = parseFunctionCallArguments();
      // [Integration TODO] indent(-2);
      TheLexer.CachedLex();
      // [AST] Create FunctionCall Expression
      break;
    }
    default:
      return Exps;
    }
  }
}

shared_ptr<AST> Parser::parsePrimaryExpression() {
  // [Integration TODO] printf("%*sPrimaryExpression:%s\n", indent(0), "", "");
  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  shared_ptr<AST> Exps;

  // [Integration TODO] indent(2);
  switch (CurTok->getKind()) {
  case tok::kw_true:
  case tok::kw_false:
    // [Integration TODO] printf("%*sLiteral:%s\n", indent(0), "", CurTok->getName());
    // [AST] Create Literal Expression
    break;
  case tok::numeric_constant:
    // [TODO] Handle EtherSubdenomination, TimeSubdenomination number case.
    // [Integration TODO] printf("%*sLiteral:%s\n", indent(0), "", getLiteralAndAdvance(CurTok).str().c_str());
    // [AST] Create Literal Expression
    break;
  case tok::string_literal:
    // [Integration TODO] printf("%*sString Literal:%s\n", indent(0), "", getLiteralAndAdvance(CurTok).str().c_str());
    // [AST] Create StringLiteral Expression
    break;
  case tok::identifier:
    // [Integration TODO] printf("%*sIdentifier:%s\n", indent(0), "", getLiteralAndAdvance(CurTok).str().c_str());
    // [AST] Create sIdentifier Expression
    break;
  case tok::kw_type:
    // [TODO] Type expression is globally-avariable function
    TheLexer.CachedLex();
    break;
  case tok::l_paren:
  case tok::l_square: {
    // Tuple/parenthesized expression or inline array/bracketed expression.
    // Special cases: ()/[] is empty tuple/array type, (x) is not a real tuple,
    // (x,) is one-dimensional tuple, elements in arrays cannot be left out,
    // only in tuples.
    TheLexer.CachedLex();
    vector<shared_ptr<AST>> Components;
    tok::TokenKind OppositeToken =
        (CurTok->is(tok::l_paren) ? tok::r_paren : tok::r_square);
    bool IsArray = (OppositeToken == tok::l_square);

    // [Integration TODO] printf("%*s%s:\n", indent(0), "", IsArray ? "Array" : "Touple");
    // [Integration TODO] indent(2);
    if (TheLexer.LookAhead(0)->isNot(OppositeToken))

      while (true) {
        if (!TheLexer.LookAhead(0)->isOneOf(tok::comma, OppositeToken)) {
          Components.push_back(parseExpression());
        } else if (IsArray)
          assert(
              "Expected expression (inline array elements cannot be omitted).");
        else
          Components.push_back(shared_ptr<AST>());

        if (TheLexer.LookAhead(0)->is(OppositeToken)) {
          break;
        }
        TheLexer.CachedLex();
      }
    TheLexer.CachedLex();
    // [Integration TODO] indent(-2);
    // [AST] Create TupleExpression
    break;
  }
  case tok::unknown:
    assert("Unknown token");
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
      assert(string("Expected primary expression."));
    */
    assert(string("Expected primary expression."));
    break;
  }
  // [Integration TODO] indent(-2);
  return nullptr;
}

vector<shared_ptr<AST>> Parser::parseFunctionCallListArguments() {
  vector<shared_ptr<AST>> Arguments;
  if (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
    Arguments.push_back(parseExpression());
    while (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
      TheLexer.CachedLex();
      Arguments.push_back(parseExpression());
    }
  }
  return Arguments;
}

pair<vector<shared_ptr<AST>>,
          vector<shared_ptr<string>>>
Parser::parseFunctionCallArguments() {
  pair<vector<shared_ptr<AST>>,
            vector<shared_ptr<string>>>
      Ret;
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
        assert("Unexpected trailing comma.");
        TheLexer.CachedLex();
      }
      First = false;
    }
    TheLexer.CachedLex();
  } else
    Ret.first = parseFunctionCallListArguments();
  return Ret;
}

shared_ptr<AST> Parser::createEmptyParameterList() { return nullptr; }

shared_ptr<string> Parser::expectIdentifierToken() { return nullptr; }

llvm::StringRef Parser::getLiteralAndAdvance(llvm::Optional<Token> Tok) {
  TheLexer.CachedLex();
  if (Tok->is(tok::identifier))
    return Tok->getIdentifierInfo()->getName();
  else
    return llvm::StringRef(Tok->getLiteralData(), Tok->getLength());
}

} // namespace soll