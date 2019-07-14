#pragma once
#include "soll/Basic/SourceManager.h"

namespace soll {

class Lexer;
class AST;

class Parser {
  Lexer &TheLexer;

public:
  Parser(Lexer &);
  std::shared_ptr<AST> parse();

private:
  struct FunctionHeaderParserResult {
    bool isConstructor;
    std::shared_ptr<std::string> name;
  };

public:
  std::shared_ptr<AST> parseContractDefinition(tok::TokenKind);
  FunctionHeaderParserResult parseFunctionHeader(bool _forceEmptyName,
                                                 bool _allowModifiers);
  std::shared_ptr<AST> parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::shared_ptr<AST> parseBlock();
};

} // namespace soll
