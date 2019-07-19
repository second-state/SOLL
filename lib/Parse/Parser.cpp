#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/Lex/Lexer.h"
#include <iostream>

namespace soll {

static llvm::StringRef getLiteral(llvm::Optional<Token> Tok) {
  return llvm::StringRef(Tok->getLiteralData(), Tok->getLength());
}

Parser::Parser(Lexer &lexer) : TheLexer(lexer) {}

std::shared_ptr<AST> Parser::parse() {
  llvm::Optional<Token> CurTok;
  std::vector<std::shared_ptr<AST>> Nodes;

  while ((CurTok = TheLexer.PeekAhead(1))->isNot(tok::eof)) {
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
      printf("Solidity Error: Token incompatible with Solidity parser as part "
             "of pragma directive.");
    else if (Kind == tok::caret) {
      // [TODO] Fix tok::caret no literal, but not sure what means
      // Pattern not match Solidity : Solidity | ^ | 0.5 | .0 |
      //                   Soll     : Solidity | 0.5.0
    } else {
      std::string literal = CurTok->isLiteral()
                                ? getLiteral(CurTok).str()
                                : CurTok->getIdentifierInfo()->getName().str();
      printf("%s\n", literal.c_str());
      Literals.push_back(literal);
      Tokens.push_back(CurTok);
    }
  } while (!TheLexer.PeekAhead(1)->isOneOf(tok::semi, tok::eof));
  TheLexer.CachedLex();

  // [TODO] Implement version recognize and compare. ref: parsePragmaVersion
  return nullptr;
}

std::shared_ptr<AST> Parser::parseContractDefinition() {
  llvm::Optional<Token> CurTok;

  printf("Contract kind: %s\n", TheLexer.CachedLex()->getName());

  std::shared_ptr<std::string> name = nullptr;
  std::vector<std::shared_ptr<AST>> SubNodes;
  name = std::make_shared<std::string>(
      TheLexer.CachedLex()->getIdentifierInfo()->getName().str());
  printf("Contract: %s\n", name->c_str());

  if (TheLexer.PeekAhead(1)->is(tok::kw_is)) {
    do {
      TheLexer.CachedLex();
      printf(
          "Inheritance: %s\n",
          TheLexer.CachedLex()->getIdentifierInfo()->getName().str().c_str());
      // [TODO] Update vector<InheritanceSpecifier> baseContracts
    } while ((TheLexer.PeekAhead(1))->is(tok::comma));
  }
  TheLexer.CachedLex();

  tok::TokenKind Kind = TheLexer.PeekAhead(1)->getKind();
  while (true) {
    if (Kind == tok::r_brace) {
      break;
    }
    // [TODO] < Parse all types in contract's context >
    if (Kind == tok::kw_function) {
      SubNodes.push_back(parseFunctionDefinitionOrFunctionTypeStateVariable());
    } else if (Kind == tok::kw_struct) {
      // [TODO] contract tok::kw_struct
    } else if (Kind == tok::kw_enum) {
      // [TODO] contract tok::kw_enum
    } else if (Kind == tok::identifier || Kind == tok::kw_mapping ||
               true // TokenTraits::isElementaryTypeName(currentTokenValue)
               // [TODO] Need recognize ElementaryTypeName rule
    ) {
      // [TODO] contract tok::identifier, tok::kw_mapping, tok::/type keywords/
    } else if (Kind == tok::kw_modifier) {
      // [TODO] contract tok::kw_modifier
    } else if (Kind == tok::kw_event) {
      // [TODO] contract tok::kw_event
    } else if (Kind == tok::kw_using) {
      // [TODO]  contract tok::kw_using
    } else
      printf("Solidity Error: Function, variable, struct or modifier "
             "declaration expected.");
    Kind = TheLexer.PeekAhead(1)->getKind();
  }
  return nullptr;
}

Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool _forceEmptyName, bool _allowModifiers) {

  FunctionHeaderParserResult result;

  result.isConstructor = false;
  if (TheLexer.CachedLex()->is(tok::kw_constructor)) {
    result.isConstructor = true;
  }

  llvm::Optional<Token> CurTok = TheLexer.PeekAhead(1);
  if (result.isConstructor)
    result.name = std::make_shared<std::string>();
  else if (_forceEmptyName || CurTok->is(tok::l_paren))
    result.name = std::make_shared<std::string>();
  else
    result.name = std::make_shared<std::string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName().str());
  printf("Function: %s\n", result.name->c_str());

  VarDeclParserOptions options;
  options.allowLocationSpecifier = true;
  result.parameters = parseParameterList(options);

  while (true) {
    CurTok = TheLexer.PeekAhead(1);
    if (_allowModifiers && CurTok->is(tok::identifier)) {
      // [TODO] Function Modifier
    } else if (CurTok->isOneOf(tok::kw_public, tok::kw_private,
                               tok::kw_internal, tok::kw_external)) {
      // [TODO] Special case of a public state variable of function type.
      result.visibility = TheLexer.CachedLex()->getName();
      printf("Visibility: %s\n", result.visibility);
    } else if (CurTok->isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                               tok::kw_payable)) {
      result.stateMutability = TheLexer.CachedLex()->getName();
      printf("StateMutability: %s\n", result.stateMutability);
    } else {
      break;
    }
  }

  if (TheLexer.PeekAhead(1)->is(tok::kw_returns)) {
    bool const permitEmptyParameterList = false;
    TheLexer.CachedLex();
    result.returnParameters =
        parseParameterList(options, permitEmptyParameterList);
  } else {
    result.returnParameters.clear();
  }

  return result;
}

std::shared_ptr<AST>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult header = parseFunctionHeader(false, true);

  // [PrePOC] Unhandle parse function body now. Hardcode drop for pair brace.
  unsigned int brace_cnt = 0;
  if (TheLexer.PeekAhead(1)->is(tok::l_brace)){
    TheLexer.CachedLex();
    brace_cnt ++;
    while(brace_cnt > 0){
      while (TheLexer.PeekAhead(1)->isNot(tok::r_brace)) {
        if (TheLexer.PeekAhead(1)->is(tok::l_brace))
          brace_cnt ++;
        TheLexer.CachedLex();
      }
      brace_cnt--;
      TheLexer.CachedLex();
    }
  }
  return nullptr;

  if (header.isConstructor || !header.modifiers.empty() ||
      !header.name->empty() ||
      TheLexer.PeekAhead(1)->isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function
    std::shared_ptr<AST> block = std::shared_ptr<AST>();
    if (TheLexer.CachedLex()->isNot(tok::semi)) {
      block = parseBlock();
      // nodeFactory.setEndPositionFromNode(block);
    }

    /*
    return nodeFactory.createNode<FunctionDefinition>(
      header.name,
      header.visibility,
      header.stateMutability,
      header.isConstructor,
      docstring,
      header.parameters,
      header.modifiers,
      header.returnParameters,
      block
    );
    */
  } else {
    // [TODO] State Variable case.
  }
  return nullptr;
}

std::shared_ptr<AST> Parser::parseVariableDeclaration(
    VarDeclParserOptions const &_options,
    std::shared_ptr<AST> const &_lookAheadArrayType) {
  std::shared_ptr<AST> type;
  if (_lookAheadArrayType) {
    type = _lookAheadArrayType;
  } else {
    type = parseTypeName(_options.allowVar);
  }

  bool isIndexed = false;
  bool isDeclaredConst = false;
  const char *visibility = "default";
  const char *location = "Unspecified";
  std::shared_ptr<std::string> identifier;

  llvm::Optional<Token> CurTok;
  while (true) {
    CurTok = TheLexer.PeekAhead(1);
    if (_options.isStateVariable &&
        CurTok->isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal)) {
      visibility = TheLexer.CachedLex()->getName();
    } else {
      if (_options.allowIndexed && CurTok->is(tok::kw_indexed))

        isIndexed = true;
      else if (CurTok->is(tok::kw_constant))
        isDeclaredConst = true;
      else if (_options.allowLocationSpecifier &&
               CurTok->isOneOf(tok::kw_memory, tok::kw_storage,
                               tok::kw_calldata)) {
        location = CurTok->getName();
      } else {
        break;
      }
    }
  }

  if (_options.allowEmptyName &&
      TheLexer.PeekAhead(1)->isNot(tok::identifier)) {
    identifier = std::make_shared<std::string>("");
  } else {
    identifier = std::make_shared<std::string>(
        TheLexer.CachedLex()->getIdentifierInfo()->getName());
  }
  printf("Variable: %s\n", identifier->c_str());

  // [TODO] Handle init value, need rearrange code flow. <pending>
  /*
  std::shared_ptr<std::string> value = nullptr;
  if (_options.allowInitialValue)
  {
    if (TheLexer.CachedLex()->is(tok::equal))
    {
      value = parseExpression();
      printf("Initial Value: %s\n", value->c_str());
    }
  }
  */

  /*
  return nodeFactory.createNode<VariableDeclaration>(
    type,
    identifier,
    value,
    visibility,
    _options.isStateVariable,
    isIndexed,
    isDeclaredConst,
    location
  );
  */
  return nullptr;
}

std::shared_ptr<AST> Parser::parseTypeNameSuffix(std::shared_ptr<AST> type) {
  while (TheLexer.PeekAhead(1)->is(tok::l_square)) {
    TheLexer.CachedLex();
    std::shared_ptr<std::string> length;
    length = parseExpression();
    printf("[%s]", length->c_str());
    TheLexer.CachedLex();
  }
  return type;
}

// [TODO] < Need complete all types >
std::shared_ptr<AST> Parser::parseTypeName(bool _allowVar) {
  std::shared_ptr<AST> type;
  bool haveType = false;
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  tok::TokenKind Kind = CurTok->getKind();
  if (true) // (TokenTraits::isElementaryTypeName(token))
  {
    // [TODO] parseTypeName handle address case
    printf("Type: %s", CurTok->getName());
    haveType = true;
  } else if (Kind == tok::kw_var) {
    // [TODO] parseTypeName tok::kw_var
  } else if (Kind == tok::kw_function) {
    // [TODO] parseTypeName tok::kw_function
  } else if (Kind == tok::kw_mapping) {
    // [TODO] parseTypeName tok::kw_mapping
  } else if (Kind == tok::identifier) {
    // [TODO] parseTypeName tok::var
  } else
    printf("Solidity Error: Expected type name");

  if (haveType) {
    type = parseTypeNameSuffix(type);
    printf("\n");
  }

  return type;
}

std::vector<std::shared_ptr<AST>>
Parser::parseParameterList(VarDeclParserOptions const &_options,
                           bool _allowEmpty) {
  std::vector<std::shared_ptr<AST>> parameters;

  VarDeclParserOptions options(_options);
  options.allowEmptyName = true;
  printf("Parameters:\n");
  if (TheLexer.PeekAhead(1)->is(tok::l_paren)) {
    do {
      TheLexer.CachedLex();
      parameters.push_back(parseVariableDeclaration(options));
    } while (TheLexer.PeekAhead(1)->is(tok::comma));
    TheLexer.CachedLex();
  }
  return parameters;
}

std::shared_ptr<AST> Parser::parseBlock() {
  std::vector<std::shared_ptr<AST>> statements;
  llvm::Optional<Token> CurTok;
  while ((CurTok = TheLexer.CachedLex())->isNot(tok::r_brace))
    statements.push_back(parseStatement());
  return nullptr;
}

// [TODO] < Parse all statements >
std::shared_ptr<AST> Parser::parseStatement() {
  std::shared_ptr<AST> statement;
  switch (TheLexer.CachedLex()->getKind()) {
  case tok::kw_if:
    // return parseIfStatement();
  case tok::kw_while:
    // return parseWhileStatement(docString);
  case tok::kw_do:
    // return parseDoWhileStatement(docString);
  case tok::kw_for:
    // return parseForStatement(docString);
  case tok::l_brace:
    // return parseBlock(docString);
    // starting from here, all statements must be terminated by a semicolon
  case tok::kw_continue:
    // statement = ASTNodeFactory(*this).createNode<Continue>(docString);
    // m_scanner->next();
    break;
  case tok::kw_break:
    // statement = ASTNodeFactory(*this).createNode<Break>(docString);
    // m_scanner->next();
    // break;
  case tok::kw_return:
    /*
    {
    ASTNodeFactory nodeFactory(*this);
    ASTPointer<Expression> expression;
    if (m_scanner->next() != tok::Semicolon)
    {
      expression = parseExpression();
      nodeFactory.setEndPositionFromNode(expression);
    }
    statement = nodeFactory.createNode<Return>(docString, expression);
      break;
    }
    */
  case tok::kw_throw:
    /*
    {
      statement = ASTNodeFactory(*this).createNode<Throw>(docString);
      m_scanner->next();
      break;
    }
    */
  case tok::kw_assembly:
    // return parseInlineAssembly(docString);
  case tok::kw_emit:
    // statement = parseEmitStatement(docString);
    // break;
  case tok::identifier:
    /*
    if (m_insideModifier && m_scanner->currentLiteral() == "_")
      {
        statement =
    ASTNodeFactory(*this).createNode<PlaceholderStatement>(docString);
        m_scanner->next();
      }
    else
      statement = parseSimpleStatement(docString);
    break;
    */
  default:
    // statement = parseSimpleStatement(docString);
    break;
  }

  return statement;
}

std::shared_ptr<std::string> Parser::parseExpression(
    std::shared_ptr<AST> const &_partiallyParsedExpression) {
  llvm::Optional<Token> CurTok = TheLexer.CachedLex();
  return std::make_shared<std::string>(getLiteral(CurTok));
}

} // namespace soll