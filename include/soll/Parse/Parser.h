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
  std::shared_ptr<AST> parseIfStatement();
  std::shared_ptr<AST> parseSimpleStatement();
  std::shared_ptr<AST> parseVariableDeclarationStatement(
      std::shared_ptr<AST> const &LookAheadArrayType = std::shared_ptr<AST>());
  std::shared_ptr<AST> parseExpressionStatement(
      std::shared_ptr<AST> const &PartiallyParsedExpression =
          std::shared_ptr<AST>());
  std::shared_ptr<std::string>
  parseExpression(std::shared_ptr<AST> const &PartiallyParsedExpression =
                      std::shared_ptr<AST>());

  /// Used as return value of @see peekStatementType.
  enum class LookAheadInfo {
    IndexAccessStructure,
    VariableDeclaration,
    Expression
  };

  /// Structure that represents a.b.c[x][y][z]. Can be converted either to an
  /// expression or to a type name. For this to be valid, path cannot be empty,
  /// but indices can be empty.
  struct IndexAccessedPath {
    std::vector<std::shared_ptr<AST>> Path;
  };

  std::pair<LookAheadInfo, IndexAccessedPath> tryParseIndexAccessedPath();

  /// Performs limited look-ahead to distinguish between variable declaration
  /// and expression statement. For source code of the form "a[][8]"
  /// ("IndexAccessStructure"), this is not possible to decide with constant
  /// look-ahead.
  LookAheadInfo peekStatementType() const;

  /// @returns a typename parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  std::shared_ptr<AST>
  typeNameFromIndexAccessStructure(IndexAccessedPath const &PathAndIndices);
  /// @returns an expression parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  std::shared_ptr<AST>
  expressionFromIndexAccessStructure(IndexAccessedPath const &PathAndIndices);
};

} // namespace soll
