#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/Lex/Lexer.h"

namespace soll {

static llvm::StringRef getLiteral(llvm::Optional<Token> Tok) {
  return llvm::StringRef(Tok->getLiteralData(), Tok->getLength());
}

Parser::Parser(Lexer &lexer) : TheLexer(lexer) {}

std::shared_ptr<AST> Parser::parse() {
  llvm::Optional<Token> CurTok;
  std::vector<std::shared_ptr<AST>> Nodes;

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

std::shared_ptr<AST> Parser::parsePragmaDirective() {
  // pragma anything* ;
  // Currently supported:
  // pragma solidity ^0.4.0 || ^0.3.0;
  std::vector<std::string> Literals;
  std::vector<llvm::Optional<Token>> Tokens;
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  do {
    CurTok = TheLexer.CachedLex();
    tok::TokenKind Kind = CurTok->getKind();
    if (Kind == tok::unknown)
      assert("Solidity Error: Token incompatible with Solidity parser as part "
             "of pragma directive.");
    else if (Kind == tok::caret) {
      // [TODO] Fix tok::caret no literal, but not sure what means
      // Pattern not match Solidity : Solidity | ^ | 0.5 | .0 |
      //                   Soll     : Solidity | 0.5.0
    } else {
      std::string literal = CurTok->isLiteral()
                                ? getLiteral(CurTok).str()
                                : CurTok->getIdentifierInfo()->getName().str();
      // [Integration TODO] printf("%s\n", literal.c_str());
      Literals.push_back(literal);
      Tokens.push_back(CurTok);
    }
  } while (!TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::eof));
  TheLexer.CachedLex();

  // [TODO] Implement version recognize and compare. ref: parsePragmaVersion
  return nullptr;
}

std::shared_ptr<AST> Parser::parseContractDefinition() {
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();

  // [Integration TODO] printf("Contract kind: %s\n", CurTok->getName());

  std::shared_ptr<std::string> Name = nullptr;
  std::vector<std::shared_ptr<AST>> SubNodes;
  Name = std::make_shared<std::string>(
      TheLexer.CachedLex()->getIdentifierInfo()->getName().str());
  // [Integration TODO] printf("Contract: %s\n", Name->c_str());

  if (TheLexer.LookAhead(0)->is(tok::kw_is)) {
    do {
      TheLexer.CachedLex();
      CurTok = TheLexer.CachedLex();
      // [Integration TODO] printf("Inheritance: %s\n", CurTok->getIdentifierInfo()->getName().str().c_str());
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
      SubNodes.push_back(parseFunctionDefinitionOrFunctionTypeStateVariable());
    } else if (Kind == tok::kw_struct) {
      // [TODO] contract tok::kw_struct
    } else if (Kind == tok::kw_enum) {
      // [TODO] contract tok::kw_enum
    } else if (Kind == tok::identifier || Kind == tok::kw_mapping ||
               true /*TokenTraits::isElementaryTypeName(currentTokenValue)*/) {
      // [TODO] Need recognize ElementaryTypeName rule
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
    Result.Name = std::make_shared<std::string>();
  else if (ForceEmptyName || CurTok->is(tok::l_paren))
    Result.Name = std::make_shared<std::string>();
  else
    Result.Name = std::make_shared<std::string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName().str());
  // [Integration TODO] printf("Function: %s\n", Result.Name->c_str());

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
      Result.Visibility = TheLexer.CachedLex()->getName();
      // [Integration TODO] printf("Visibility: %s\n", Result.Visibility);
    } else if (CurTok->isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                               tok::kw_payable)) {
      Result.StateMutability = TheLexer.CachedLex()->getName();
      // [Integration TODO] printf("StateMutability: %s\n", Result.StateMutability);
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
    Result.ReturnParameters.clear();
  }

  return Result;
}

std::shared_ptr<AST>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult Header = parseFunctionHeader(false, true);
  if (Header.IsConstructor || !Header.Modifiers.empty() ||
      !Header.Name->empty() ||
      TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function
    std::shared_ptr<AST> block = std::shared_ptr<AST>();
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      block = parseBlock();
    }
  } else {
    // [TODO] State Variable case.
  }
  return nullptr;
}

std::shared_ptr<AST> Parser::parseVariableDeclaration(
    VarDeclParserOptions const &Options,
    std::shared_ptr<AST> const &LookAheadArrayType) {
  std::shared_ptr<AST> Type;
  if (LookAheadArrayType) {
    Type = LookAheadArrayType;
  } else {
    Type = parseTypeName(Options.AllowVar);
  }

  bool IsIndexed = false;
  bool IsDeclaredConst = false;
  const char *Visibility = "default";
  const char *Location = "Unspecified";
  std::shared_ptr<std::string> Identifier;

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
    Identifier = std::make_shared<std::string>("");
  } else {
    Identifier = std::make_shared<std::string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName());
  }
  // [Integration TODO] printf("Variable: %s\n", Identifier->c_str());

  // [TODO] Handle variable with init value
  return nullptr;
}

std::shared_ptr<AST> Parser::parseTypeNameSuffix(std::shared_ptr<AST> Type) {
  while (TheLexer.LookAhead(0)->is(tok::l_square)) {
    TheLexer.CachedLex();
    std::shared_ptr<std::string> Length;
    Length = parseExpression();
    // [Integration TODO] printf("[%s]", Length->c_str());
    TheLexer.CachedLex();
  }
  return Type;
}

// [TODO] < Need complete all Types >
std::shared_ptr<AST> Parser::parseTypeName(bool AllowVar) {
  std::shared_ptr<AST> Type;
  bool HaveType = false;
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  tok::TokenKind Kind = CurTok->getKind();
  if (true) // (TokenTraits::isElementaryTypeName(token))
  {
    // [TODO] parseTypeName handle address case
    // [Integration TODO] printf("Type: %s", CurTok->getName());
    HaveType = true;
  } else if (Kind == tok::kw_var) {
    // [TODO] parseTypeName tok::kw_var
  } else if (Kind == tok::kw_function) {
    // [TODO] parseTypeName tok::kw_function
  } else if (Kind == tok::kw_mapping) {
    // [TODO] parseTypeName tok::kw_mapping
  } else if (Kind == tok::identifier) {
    // [TODO] parseTypeName tok::var
  } else
    assert("Expected Type Name");

  if (HaveType) {
    Type = parseTypeNameSuffix(Type);
    // [Integration TODO] printf("\n");
  }

  return Type;
}

std::vector<std::shared_ptr<AST>>
Parser::parseParameterList(VarDeclParserOptions const &options,
                           bool AllowEmpty) {
  std::vector<std::shared_ptr<AST>> Parameters;

  VarDeclParserOptions Options(options);
  Options.AllowEmptyName = true;
  // [Integration TODO] printf("Parameters:\n");
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    do {
      TheLexer.CachedLex();
      Parameters.push_back(parseVariableDeclaration(Options));
    } while (TheLexer.LookAhead(0)->is(tok::comma));
    TheLexer.CachedLex();
  }
  return Parameters;
}

std::shared_ptr<AST> Parser::parseBlock() {
  std::vector<std::shared_ptr<AST>> Statements;
  TheLexer.CachedLex();
  while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
    Statements.push_back(parseStatement());
  }
  TheLexer.CachedLex();
  return nullptr;
}

// [TODO] < Parse all statements >
std::shared_ptr<AST> Parser::parseStatement() {
  std::shared_ptr<AST> statement;
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
    // [Integration TODO] printf("Return statement: ");
    // [PrePOC] Parse expression after return. Wait for parseExpression() ready.
    while (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      CurTok = TheLexer.CachedLex();
      // [Integration TODO] printf("%s ", CurTok->getName());
    }
    TheLexer.CachedLex();
    // [Integration TODO] printf("\n");
    break;
  case tok::kw_assembly:
    // [TODO] parseStatement kw_do
    break;
  case tok::kw_emit:
    // [TODO] parseStatement kw_do
    break;
  case tok::identifier:
  default:
    statement = parseSimpleStatement();
    break;
  }
  return statement;
}

std::shared_ptr<AST> Parser::parseIfStatement() {
  // [Integration TODO] printf("If statement:\n{\n");
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  // [Integration TODO] printf("%s ( ", CurTok->getName());

  // [PrePOC] Parse condition expression wait for parseExpression() ready.
  TheLexer.CachedLex();
  while (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
    CurTok = TheLexer.CachedLex();
    // [Integration TODO] printf("%s ", CurTok->getName());
  }
  TheLexer.CachedLex();
  // [Integration TODO] printf(" )\n");

  std::shared_ptr<AST> TrueBody = parseStatement();
  std::shared_ptr<AST> FalseBody;
  if (TheLexer.LookAhead(0)->is(tok::kw_else)) {
    CurTok = TheLexer.CachedLex();
    // [Integration TODO] printf("%s\n", CurTok->getName());

    FalseBody = parseStatement();
  }
  // [Integration TODO] printf("}\n");
  return nullptr;
}

std::shared_ptr<AST> Parser::parseSimpleStatement() {
  // [Integration TODO] printf("Simple statement: ");
  llvm::Optional<Token> CurTok;
  while (TheLexer.LookAhead(0)->isNot(tok::semi)) {
    CurTok = TheLexer.CachedLex();
    // [Integration TODO] printf("%s ", CurTok->getName());
  }
  TheLexer.CachedLex();
  // [Integration TODO] printf("\n");
  return nullptr;
}

std::shared_ptr<std::string>
Parser::parseExpression(std::shared_ptr<AST> const &PartiallyParsedExpression) {
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  return std::make_shared<std::string>(getLiteral(CurTok));
}

} // namespace soll