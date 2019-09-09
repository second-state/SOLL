// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/AST.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Sema/Sema.h"

namespace soll {

class Token;
class Lexer;
class AST;

class Decl;
class SourceUint;
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
class Expr;

class Parser {
  Lexer &TheLexer;
  Sema &Actions;

public:
  Parser(Lexer &, Sema &);
  std::unique_ptr<SourceUnit> parse();

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
    bool IsFallback;
    llvm::StringRef Name;
    Decl::Visibility Vsblty = Decl::Visibility::Default;
    StateMutability SM = StateMutability::NonPayable;
    std::unique_ptr<ParamList> Parameters;
    std::vector<std::unique_ptr<ModifierInvocation>> Modifiers;
    std::unique_ptr<ParamList> ReturnParameters;
  };

  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // std::vector<Token> const& _tokens, std::vector<std::string> const&
  // _literals);
  std::unique_ptr<PragmaDirective> parsePragmaDirective();
  ContractDecl::ContractKind parseContractKind();
  std::unique_ptr<ContractDecl> parseContractDefinition();
  Decl::Visibility parseVisibilitySpecifier();
  StateMutability parseStateMutability();
  FunctionHeaderParserResult parseFunctionHeader(bool ForceEmptyName,
                                                 bool AllowModifiers);
  std::unique_ptr<FunctionDecl>
  parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::unique_ptr<VarDecl>
  parseVariableDeclaration(VarDeclParserOptions const &Options = {},
                           TypePtr &&LookAheadArrayType = nullptr);
  std::unique_ptr<AST> parseEventDefinition();
  TypePtr parseTypeNameSuffix(TypePtr T);
  TypePtr parseTypeName(bool AllowVar);
  std::shared_ptr<MappingType> parseMapping();
  std::unique_ptr<ParamList>
  parseParameterList(VarDeclParserOptions const &Options = {},
                     bool AllowEmpty = true);
  std::unique_ptr<Block> parseBlock();
  std::unique_ptr<Stmt> parseStatement();
  std::unique_ptr<IfStmt> parseIfStatement();
  std::unique_ptr<WhileStmt> parseWhileStatement();
  std::unique_ptr<WhileStmt> parseDoWhileStatement();
  std::unique_ptr<ForStmt> parseForStatement();
  std::unique_ptr<Stmt> parseSimpleStatement();
  std::unique_ptr<DeclStmt>
  parseVariableDeclarationStatement(TypePtr &&LookAheadArrayType = nullptr);
  std::unique_ptr<Expr>
  parseExpression(std::unique_ptr<Expr> &&PartiallyParsedExpression = nullptr);
  std::unique_ptr<Expr> parseBinaryExpression(
      int MinPrecedence = 4,
      std::unique_ptr<Expr> &&PartiallyParsedExpression = nullptr);
  std::unique_ptr<Expr> parseUnaryExpression(
      std::unique_ptr<Expr> &&PartiallyParsedExpression = nullptr);
  std::unique_ptr<Expr> parseLeftHandSideExpression(
      std::unique_ptr<Expr> &&PartiallyParsedExpression = nullptr);
  std::unique_ptr<Expr> parsePrimaryExpression();
  std::vector<std::unique_ptr<Expr>> parseFunctionCallListArguments();
  std::pair<std::vector<std::unique_ptr<Expr>>, std::vector<llvm::StringRef>>
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
    TypePtr ElementaryType;
    std::vector<std::unique_ptr<Identifier>> Path;
    std::vector<std::unique_ptr<Expr>> Indices;
    bool empty() const;
  };

  std::pair<LookAheadInfo, IndexAccessedPath> tryParseIndexAccessedPath();

  /// Performs limited look-ahead to distinguish between variable declaration
  /// and expression statement. For source code of the form "a[][8]"
  /// ("IndexAccessStructure"), this is not possible to decide with constant
  /// look-ahead.
  LookAheadInfo peekStatementType() const;
  /// @returns an IndexAccessedPath as a prestage to parsing a variable
  /// declaration (type name) or an expression;
  IndexAccessedPath parseIndexAccessedPath();
  /// @returns a typename parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  TypePtr typeNameFromIndexAccessStructure(IndexAccessedPath &PathAndIndices);
  /// @returns an expression parsed in look-ahead fashion from something like
  /// "a.b[8][2**70]", or an empty pointer if an empty @a _pathAndIncides has
  /// been supplied.
  std::unique_ptr<Expr>
  expressionFromIndexAccessStructure(IndexAccessedPath &PathAndIndices);
  llvm::StringRef getLiteralAndAdvance(llvm::Optional<Token> Tok);
  /// Creates an empty ParameterList at the current location (used if parameters
  /// can be omitted).
  std::unique_ptr<AST> createEmptyParameterList();
};

} // namespace soll
