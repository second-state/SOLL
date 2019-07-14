#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/Lex/Lexer.h"

namespace soll {

static void convert_tok(llvm::Optional<Token> &Tok) {
  if (Tok->getKind() == tok::identifier) {
    if (Tok->getIdentifierInfo()->getName().find("contract") !=
        llvm::StringLiteral::npos) {
      Tok->setKind(tok::kw_contract);
    } else if (Tok->getIdentifierInfo()->getName().find("function") !=
               llvm::StringLiteral::npos) {
      Tok->setKind(tok::kw_function);
    } else if (Tok->getIdentifierInfo()->getName().find("is") !=
               llvm::StringLiteral::npos) {
      Tok->setKind(tok::kw_is);
    }
  }
}

Parser::Parser(Lexer &lexer) : TheLexer(lexer) {}

std::shared_ptr<AST> Parser::parse() {
  llvm::Optional<Token> CurTok;
  std::vector<std::shared_ptr<AST>> Nodes;

  while ((CurTok = TheLexer.Lex())->isNot(tok::eof)) {

    convert_tok(CurTok);

    tok::TokenKind Kind = CurTok->getKind();
    switch (Kind) {
    case tok::kw_pragma:
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

std::shared_ptr<AST> Parser::parseContractDefinition(tok::TokenKind Kind) {
  const char *name = nullptr;
  std::vector<std::shared_ptr<AST>> SubNodes;

  llvm::Optional<Token> CurTok = TheLexer.Lex();
  name = CurTok->getIdentifierInfo()->getName().str().c_str();
  printf("Contract: %s\n", name);

  CurTok = TheLexer.Lex();
  convert_tok(CurTok);
  if (CurTok->getKind() == tok::kw_is) {
    do {
      CurTok = TheLexer.Lex();
      printf("Inheritance: %s\n",
             CurTok->getIdentifierInfo()->getName().str().c_str());
      // baseContracts.push_back(parseInheritanceSpecifier());
    } while ((CurTok = TheLexer.Lex())->is(tok::comma));
  }

  while (true) {
    //if (CurTok->getKind() == tok::identifier)
    //  printf("%s %s %s\n", tok::getTokenName(CurTok->getKind()),
    //         CurTok->getIdentifierInfo()->getName().str().c_str(), CurTok->getLiteralData());
    convert_tok(CurTok);
    tok::TokenKind Kind = CurTok->getKind();
    if (Kind == tok::r_brace) {
      break;
    }
    if (Kind == tok::kw_function) {
      SubNodes.push_back(parseFunctionDefinitionOrFunctionTypeStateVariable());
    }

    CurTok = TheLexer.Lex();
  }
  return nullptr;
}


Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool _forceEmptyName, bool _allowModifiers) {
  FunctionHeaderParserResult result;

  result.isConstructor = false;

  // now lexer not support contructor keyword

  llvm::Optional<Token> CurTok = TheLexer.Lex();

  if (result.isConstructor)
    result.name = std::make_shared<std::string>();
  else if (_forceEmptyName || CurTok->getKind() == tok::l_paren)
    result.name = std::make_shared<std::string>();
  else
    result.name = std::make_shared<std::string>(CurTok->getIdentifierInfo()->getName().str());
  printf("Function: %s\n", result.name->c_str());
  return result;
}


std::shared_ptr<AST>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  FunctionHeaderParserResult header = parseFunctionHeader(false, true);
  return nullptr;
}

std::shared_ptr<AST> Parser::parseBlock() { return nullptr; }

} // namespace soll
