#pragma once
#include "soll/Basic/SourceManager.h"
#include "soll/AST/AST.h"

using namespace std;

namespace soll {

class Token;
class Lexer;
class AST;

class Decl;
class PragmaDirective;
class ContractDecl;
class Type;
class FunctionDecl;
class VarDecl;
class ParamList;
class ModifierInvocation;

class Stmt;
class Block;
class IfStmt;
class DeclStmt;
class ExprStmt;
class Expr;

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
    llvm::StringRef Name;
    Decl::Visibility Vsblty = Decl::Visibility::Default;
    StateMutability SM = StateMutability::NonPayable;
    unique_ptr<ParamList> Parameters;
    vector<unique_ptr<ModifierInvocation>> Modifiers;
    unique_ptr<ParamList> ReturnParameters;
  };

  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // vector<Token> const& _tokens, vector<string> const&
  // _literals);
  unique_ptr<PragmaDirective> parsePragmaDirective();
  ContractDecl::ContractKind parseContractKind();
  unique_ptr<ContractDecl> parseContractDefinition();
  Decl::Visibility parseVisibilitySpecifier();
  StateMutability parseStateMutability();
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
  unique_ptr<Block> parseBlock();
  unique_ptr<Stmt> parseStatement();
  unique_ptr<IfStmt> parseIfStatement();
  unique_ptr<Stmt> parseSimpleStatement();
  unique_ptr<DeclStmt> parseVariableDeclarationStatement(
      unique_ptr<Type> const &LookAheadArrayType = make_unique<Type>());
  unique_ptr<ExprStmt> parseExpressionStatement(
      unique_ptr<Expr>  &&PartiallyParsedExpression = nullptr);
  unique_ptr<Expr>
  parseExpression(unique_ptr<Expr> &&PartiallyParsedExpression =
                      nullptr);
  unique_ptr<Expr>
  parseBinaryExpression(int MinPrecedence = 4,
                        unique_ptr<Expr> &&PartiallyParsedExpression =
                            nullptr);
  unique_ptr<Expr>
  parseUnaryExpression(unique_ptr<Expr> &&PartiallyParsedExpression =
                           nullptr);
  unique_ptr<Expr> parseLeftHandSideExpression(
      unique_ptr<Expr> &&PartiallyParsedExpression =
          nullptr);
  unique_ptr<Expr> parsePrimaryExpression();
  vector<unique_ptr<Expr>> parseFunctionCallListArguments();
  pair<vector<unique_ptr<Expr>>,
            vector<unique_ptr<string>>>
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
    vector<unique_ptr<AST>> Path;
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
  unique_ptr<Expr>
  expressionFromIndexAccessStructure(IndexAccessedPath const &PathAndIndices);
  unique_ptr<string> expectIdentifierToken();
  unique_ptr<string> getLiteralAndAdvance();
  /// Creates an empty ParameterList at the current location (used if parameters
  /// can be omitted).
  unique_ptr<AST> createEmptyParameterList();
  llvm::StringRef getLiteralAndAdvance(llvm::Optional<Token> Tok);
};

} // namespace soll
