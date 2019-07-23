#pragma once
#include "soll/Basic/SourceManager.h"

using namespace std;

namespace soll {

class Token;
class Lexer;
class AST;
class PragmaDirective;
class ContractDecl;
class Type;
class FunctionDecl;
class VarDecl;
class ParamList;
class ModifierInvocation;

class Parser {
  Lexer &TheLexer;

public:
  Parser(Lexer &);
  unique_ptr<AST> parse();

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
    string Name;
    const char *Visibility;
    const char *StateMutability;
    unique_ptr<ParamList> Parameters;
    vector<unique_ptr<ModifierInvocation>> Modifiers;
    unique_ptr<ParamList> ReturnParameters;
  };

  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // vector<Token> const& _tokens, vector<string> const&
  // _literals);
  unique_ptr<PragmaDirective> parsePragmaDirective();
  unique_ptr<ContractDecl> parseContractDefinition();
  FunctionHeaderParserResult parseFunctionHeader(bool ForceEmptyName,
                                                 bool AllowModifiers);
  unique_ptr<FunctionDecl> parseFunctionDefinitionOrFunctionTypeStateVariable();
  unique_ptr<VarDecl> parseVariableDeclaration(
      VarDeclParserOptions const &Options = {},
      unique_ptr<Type> const &LookAheadArrayType = nullptr);
  unique_ptr<Type> parseTypeNameSuffix(unique_ptr<Type> T);
  unique_ptr<Type> parseTypeName(bool AllowVar);
  unique_ptr<ParamList>
  parseParameterList(VarDeclParserOptions const &Options = {},
                     bool AllowEmpty = true);
  shared_ptr<AST> parseBlock();
  shared_ptr<AST> parseStatement();
  shared_ptr<AST> parseIfStatement();
  shared_ptr<AST> parseSimpleStatement();
  shared_ptr<AST> parseVariableDeclarationStatement(
      unique_ptr<Type> const &LookAheadArrayType = make_unique<Type>());
  shared_ptr<AST> parseExpressionStatement(
      shared_ptr<AST> const &PartiallyParsedExpression =
          shared_ptr<AST>());
  shared_ptr<AST>
  parseExpression(shared_ptr<AST> const &PartiallyParsedExpression =
                      shared_ptr<AST>());
  shared_ptr<AST>
  parseBinaryExpression(int MinPrecedence = 4,
                        shared_ptr<AST> const &PartiallyParsedExpression =
                            shared_ptr<AST>());
  shared_ptr<AST>
  parseUnaryExpression(shared_ptr<AST> const &PartiallyParsedExpression =
                           shared_ptr<AST>());
  shared_ptr<AST> parseLeftHandSideExpression(
      shared_ptr<AST> const &PartiallyParsedExpression =
          shared_ptr<AST>());
  shared_ptr<AST> parsePrimaryExpression();
  vector<shared_ptr<AST>> parseFunctionCallListArguments();
  pair<vector<shared_ptr<AST>>,
            vector<shared_ptr<string>>>
  parseFunctionCallArguments();

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
    vector<shared_ptr<AST>> Path;
  };

  pair<LookAheadInfo, IndexAccessedPath> tryParseIndexAccessedPath();

  /// Performs limited look-ahead to distinguish between variable declaration
  /// and expression statement. For source code of the form "a[][8]"
  /// ("IndexAccessStructure"), this is not possible to decide with constant
  /// look-ahead.
  LookAheadInfo peekStatementType() const;

  /// @returns a typename parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  unique_ptr<Type>
  typeNameFromIndexAccessStructure(IndexAccessedPath const &PathAndIndices);
  /// @returns an expression parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  shared_ptr<AST>
  expressionFromIndexAccessStructure(IndexAccessedPath const &PathAndIndices);
  shared_ptr<string> expectIdentifierToken();
  shared_ptr<string> getLiteralAndAdvance();
  /// Creates an empty ParameterList at the current location (used if parameters
  /// can be omitted).
  shared_ptr<AST> createEmptyParameterList();
  llvm::StringRef getLiteralAndAdvance(llvm::Optional<Token> Tok);
};

} // namespace soll
