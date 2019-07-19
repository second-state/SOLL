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
  struct VarDeclParserOptions {
    // This is actually not needed, but due to a defect in the C++ standard, we
    // have to. https://stackoverflow.com/questions/17430377
    VarDeclParserOptions() {}

    bool allowVar = false;
    bool isStateVariable = false;
    bool allowIndexed = false;
    bool allowEmptyName = false;
    bool allowInitialValue = false;
    bool allowLocationSpecifier = false;
  };

  struct FunctionHeaderParserResult {
    bool isConstructor;
    std::shared_ptr<std::string> name;
    const char *visibility;
    const char *stateMutability;
    std::vector<std::shared_ptr<AST>> parameters;
    std::vector<std::shared_ptr<AST>> modifiers;
    std::vector<std::shared_ptr<AST>> returnParameters;
  };

public:
  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // std::vector<Token> const& _tokens, std::vector<std::string> const&
  // _literals);
  std::shared_ptr<AST> parsePragmaDirective();
  std::shared_ptr<AST> parseContractDefinition();
  FunctionHeaderParserResult parseFunctionHeader(bool _forceEmptyName,
                                                 bool _allowModifiers);
  std::shared_ptr<AST> parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::shared_ptr<AST> parseVariableDeclaration(
      VarDeclParserOptions const &_options = {},
      std::shared_ptr<AST> const &_lookAheadArrayType = nullptr);
  std::shared_ptr<AST>  parseTypeNameSuffix(std::shared_ptr<AST> type);
  std::shared_ptr<AST> parseTypeName(bool _allowVar);
  std::vector<std::shared_ptr<AST>>
  parseParameterList(VarDeclParserOptions const &_options = {},
                     bool _allowEmpty = true);
  std::shared_ptr<AST> parseBlock();
  std::shared_ptr<AST> parseStatement();
  std::shared_ptr<std::string> parseExpression(
    std::shared_ptr<AST> const& _partiallyParsedExpression = std::shared_ptr<AST>()
  );
};

} // namespace soll
