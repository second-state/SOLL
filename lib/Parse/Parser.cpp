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

  while ((CurTok = TheLexer.Lex())->isNot(tok::eof)) {
    // printf("%s\n", CurTok->getName());
    tok::TokenKind Kind = CurTok->getKind();
    switch (Kind) {
    case tok::kw_pragma:
      Nodes.push_back(parsePragmaDirective());
      break;
    case tok::kw_import:
      break;
    case tok::kw_interface:
    case tok::kw_library:
    case tok::kw_contract:
      Nodes.push_back(parseContractDefinition(Kind));
      break;
    default:
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
  llvm::Optional<Token> CurTok = TheLexer.Lex();
  do {
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
    CurTok = TheLexer.Lex();
  } while (!CurTok->isOneOf(tok::semi, tok::eof));
  // [TODO] Implement version recognize and compare. ref: parsePragmaVersion
  return nullptr;
}

std::shared_ptr<AST> Parser::parseContractDefinition(tok::TokenKind Kind) {

  printf("Contract kind: %s\n", tok::getTokenName(Kind));

  std::shared_ptr<std::string> name = nullptr;
  std::vector<std::shared_ptr<AST>> SubNodes;
  llvm::Optional<Token> CurTok = TheLexer.Lex();
  name = std::make_shared<std::string>(
      CurTok->getIdentifierInfo()->getName().str());
  printf("Contract: %s\n", name->c_str());

  CurTok = TheLexer.Lex();
  if (CurTok->getKind() == tok::kw_is) {
    do {
      CurTok = TheLexer.Lex();
      printf("Inheritance: %s\n",
             CurTok->getIdentifierInfo()->getName().str().c_str());
      // [TODO] update vector<InheritanceSpecifier> baseContracts
    } while ((CurTok = TheLexer.Lex())->is(tok::comma));
  }

  while (true) {
    tok::TokenKind Kind = CurTok->getKind();
    if (Kind == tok::r_brace) {
      break;
    }
    if (Kind == tok::kw_function) {
      SubNodes.push_back(parseFunctionDefinitionOrFunctionTypeStateVariable());
    } else if (Kind == tok::kw_struct) {
      // [TODO] contract tok::kw_struct
    } else if (Kind == tok::kw_enum) {
      // [TODO] contract tok::kw_enum
    } else if (Kind == tok::identifier || Kind == tok::kw_mapping ||
               true // TokenTraits::isElementaryTypeName(currentTokenValue)
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
    CurTok = TheLexer.Lex();
  }
  return nullptr;
}

Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool _forceEmptyName, bool _allowModifiers) {
  FunctionHeaderParserResult result;

  result.isConstructor = false;

  // [TODO] lexer not support contructor keyword, only pure function case

  llvm::Optional<Token> CurTok = TheLexer.Lex();

  if (result.isConstructor)
    result.name = std::make_shared<std::string>();
  else if (_forceEmptyName || CurTok->getKind() == tok::l_paren)
    result.name = std::make_shared<std::string>();
  else
    result.name = std::make_shared<std::string>(
        CurTok->getIdentifierInfo()->getName().str());
  printf("Function: %s\n", result.name->c_str());

  /*
  while (true){
    CurTok = TheLexer.Lex();
    if (_allowModifiers && token == tok::identifier) {
      // If the name is empty (and this is not a constructor),
      // then this can either be a modifier (fallback function declaration)
      // or the name of the state variable (function type name plus variable).
      if ((result.name->empty() && !result.isConstructor) &&
          (m_scanner->peekNextToken() == Token::Semicolon ||
           m_scanner->peekNextToken() == Token::Assign))
        // Variable declaration, break here.
        break;
      else
        result.modifiers.push_back(parseModifierInvocation());
    }
  }
  */

  return result;
}

std::shared_ptr<AST>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult header = parseFunctionHeader(false, true);
  return nullptr;
}

std::shared_ptr<AST> Parser::parseBlock() { return nullptr; }

} // namespace soll
