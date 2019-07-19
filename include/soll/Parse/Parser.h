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

    bool AllowVar = false;
    bool IsStateVariable = false;
    bool AllowIndexed = false;
    bool AllowEmptyName = false;
    bool AllowInitialValue = false;
    bool AllowLocationSpecifier = false;
  };

  struct FunctionHeaderParserResult {
    bool IsConstructor;
    std::shared_ptr<std::string> Name;
    const char *Visibility;
    const char *StateMutability;
    std::vector<std::shared_ptr<AST>> Parameters;
    std::vector<std::shared_ptr<AST>> Modifiers;
    std::vector<std::shared_ptr<AST>> ReturnParameters;
  };

public:
  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // std::vector<Token> const& _tokens, std::vector<std::string> const&
  // _literals);
  std::shared_ptr<AST> parsePragmaDirective();
  std::shared_ptr<AST> parseContractDefinition();
  FunctionHeaderParserResult parseFunctionHeader(bool ForceEmptyName,
                                                 bool AllowModifiers);
  std::shared_ptr<AST> parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::shared_ptr<AST> parseVariableDeclaration(
      VarDeclParserOptions const &Options = {},
      std::shared_ptr<AST> const &LookAheadArrayType = nullptr);
  std::shared_ptr<AST> parseTypeNameSuffix(std::shared_ptr<AST> Type);
  std::shared_ptr<AST> parseTypeName(bool AllowVar);
  std::vector<std::shared_ptr<AST>>
  parseParameterList(VarDeclParserOptions const &Options = {},
                     bool AllowEmpty = true);
  std::shared_ptr<AST> parseBlock();
  std::shared_ptr<AST> parseStatement();
  std::shared_ptr<std::string>
  parseExpression(std::shared_ptr<AST> const &PartiallyParsedExpression =
                      std::shared_ptr<AST>());
};

} // namespace soll
