// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/AST.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Lex/Lexer.h"
#include "soll/Lex/Token.h"
#include "soll/Sema/Sema.h"

namespace soll {

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
  DiagnosticsEngine &Diags;
  Token Tok;
  const llvm::StringMap<llvm::APInt> LibrariesAddressMap;
  unsigned short ParenCount = 0, BracketCount = 0, BraceCount = 0;

public:
  Parser(Lexer &TheLexer, Sema &Actions, DiagnosticsEngine &Diags,
         const std::vector<std::string> &LibrariesAddressMaps);
  std::unique_ptr<SourceUnit> parse();
  std::unique_ptr<SourceUnit> parseYul();

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
    bool IsVirtual;
    llvm::StringRef Name;
    Decl::Visibility Vsblty = Decl::Visibility::Default;
    StateMutability SM = StateMutability::NonPayable;
    std::unique_ptr<ParamList> Parameters;
    std::vector<std::unique_ptr<ModifierInvocation>> Modifiers;
    std::unique_ptr<ParamList> ReturnParameters;
    std::unique_ptr<OverrideSpecifier> Overrides;
  };

  // void parsePragmaVersion(langutil::SourceLocation const& _location,
  // std::vector<Token> const& _tokens, std::vector<std::string> const&
  // _literals);
  std::unique_ptr<PragmaDirective> parsePragmaDirective();
  std::pair<ContractDecl::ContractKind, bool> parseContractKind();
  std::unique_ptr<ContractDecl> parseContractDefinition();
  std::unique_ptr<InheritanceSpecifier> parseInheritanceSpecifier();
  Decl::Visibility parseVisibilitySpecifier();
  std::unique_ptr<OverrideSpecifier> parseOverrideSpecifier();
  StateMutability parseStateMutability();
  DataLocation parseDataLocation();
  std::unique_ptr<UsingFor> parseUsingFor();

  FunctionHeaderParserResult parseFunctionHeader(bool ForceEmptyName,
                                                 bool AllowModifiers);
  std::unique_ptr<FunctionDecl>
  parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::unique_ptr<StructDecl> parseStructDeclaration();
  std::unique_ptr<VarDecl>
  parseVariableDeclaration(VarDeclParserOptions const &Options = {},
                           TypePtr &&LookAheadArrayType = nullptr);
  std::unique_ptr<EventDecl> parseEventDefinition();
  void parseUserDefinedTypeName();
  std::unique_ptr<IdentifierPath>
  parseIdentifierPath(tok::TokenKind SplitTok = tok::period);
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
  std::unique_ptr<EmitStmt> parseEmitStatement();
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

  std::unique_ptr<YulObject> parseYulObject();
  std::unique_ptr<YulCode> parseYulCode();
  std::unique_ptr<YulData> parseYulData();

  std::unique_ptr<Block> parseAsmBlock(bool HasScope = false);
  std::unique_ptr<Stmt> parseAsmStatement();
  std::unique_ptr<IfStmt> parseAsmIfStatement();
  std::unique_ptr<AsmSwitchStmt> parseAsmSwitchStatement();
  std::unique_ptr<AsmCaseStmt> parseAsmCaseStatement();
  std::unique_ptr<AsmDefaultStmt> parseAsmDefaultStatement();
  std::unique_ptr<AsmForStmt> parseAsmForStatement();
  std::unique_ptr<Expr> parseAsmExpression();
  std::unique_ptr<Expr> parseElementaryOperation();
  std::unique_ptr<AsmVarDecl> parseAsmVariableDeclaration();
  std::vector<std::unique_ptr<VarDeclBase>> parseAsmVariableDeclarationList();
  std::unique_ptr<DeclStmt> parseAsmVariableDeclarationStatement();
  std::unique_ptr<AsmFunctionDeclStmt> parseAsmFunctionDefinitionStatement();
  std::unique_ptr<Expr> parseAsmCall(std::unique_ptr<Expr> &&E);
  TypePtr parseAsmType();

  std::string stringUnquote(llvm::StringRef Quoted);
  std::string hexUnquote(llvm::StringRef Quoted);
  std::string simplifyIntegerLiteral(llvm::StringRef Literal);
  std::pair<bool, llvm::APInt> numericParse(llvm::StringRef Literal,
                                            uint64_t Unit = 1);
  llvm::StringMap<llvm::APInt>
  extractLibraries(const std::vector<std::string> &LibrariesAddressMaps);

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
    std::vector<Token> Path;
    std::vector<std::pair<std::unique_ptr<Expr>, SourceLocation>> Indices;
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

  Token GetLookAheadToken(unsigned N) {
    if (N == 0 || Tok.is(tok::eof))
      return Tok;
    return *TheLexer.LookAhead(N - 1);
  }

  Token NextToken() const { return *TheLexer.LookAhead(0); }

  bool isTokenParen() const { return Tok.isOneOf(tok::l_paren, tok::r_paren); }
  bool isTokenBracket() const {
    return Tok.isOneOf(tok::l_square, tok::r_square);
  }
  bool isTokenBrace() const { return Tok.isOneOf(tok::l_brace, tok::r_brace); }
  bool isTokenStringLiteral() const {
    return tok::isStringLiteral(Tok.getKind());
  }
  bool isTokenSpecial() const {
    return isTokenStringLiteral() || isTokenParen() || isTokenBracket() ||
           isTokenBrace();
  }

  SourceLocation ConsumeToken() {
    assert(!isTokenSpecial() &&
           "Should consume special tokens with Consume*Token");
    auto Location = Tok.getLocation();
    Tok = *TheLexer.CachedLex();
    return Location;
  }

  bool TryConsumeToken(tok::TokenKind Expected) {
    if (Tok.isNot(Expected))
      return false;
    assert(!isTokenSpecial() &&
           "Should consume special tokens with Consume*Token");
    Tok = *TheLexer.CachedLex();
    return true;
  }

  SourceLocation ConsumeParen() {
    assert(isTokenParen() && "wrong consume method");
    if (Tok.getKind() == tok::l_paren)
      ++ParenCount;
    else if (ParenCount) {
      --ParenCount; // Don't let unbalanced )'s drive the count negative.
    }
    auto Location = Tok.getLocation();
    Tok = *TheLexer.CachedLex();
    return Location;
  }

  SourceLocation ConsumeBracket() {
    assert(isTokenBracket() && "wrong consume method");
    if (Tok.getKind() == tok::l_square)
      ++BracketCount;
    else if (BracketCount) {
      --BracketCount; // Don't let unbalanced ]'s drive the count negative.
    }
    auto Location = Tok.getLocation();
    Tok = *TheLexer.CachedLex();
    return Location;
  }

  SourceLocation ConsumeBrace() {
    assert(isTokenBrace() && "wrong consume method");
    if (Tok.getKind() == tok::l_brace)
      ++BraceCount;
    else if (BraceCount) {
      --BraceCount; // Don't let unbalanced }'s drive the count negative.
    }
    auto Location = Tok.getLocation();
    Tok = *TheLexer.CachedLex();
    return Location;
  }

  SourceLocation ConsumeStringToken() {
    assert(isTokenStringLiteral() &&
           "Should only consume string literals with this method");
    auto Location = Tok.getLocation();
    Tok = *TheLexer.CachedLex();
    return Location;
  }

  SourceLocation ConsumeAnyToken() {
    if (isTokenParen())
      return ConsumeParen();
    if (isTokenBracket())
      return ConsumeBracket();
    if (isTokenBrace())
      return ConsumeBrace();
    if (isTokenStringLiteral())
      return ConsumeStringToken();
    return ConsumeToken();
  }

  bool ExpectAndConsume(tok::TokenKind ExpectedTok,
                        unsigned Diag = diag::err_expected,
                        llvm::StringRef DiagMsg = {});
  bool ExpectAndConsumeSemi(unsigned DiagID = diag::err_expected);

public:
  void EnterScope(unsigned ScopeFlags);
  void ExitScope();

private:
  bool ConsumeAndStoreUntil(tok::TokenKind T1,
                            llvm::SmallVector<Token, 4> &Toks) {
    return ConsumeAndStoreUntil(T1, T1, Toks);
  }
  bool ConsumeAndStoreUntil(tok::TokenKind T1, tok::TokenKind T2,
                            llvm::SmallVector<Token, 4> &Toks);

  DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID);
  DiagnosticBuilder Diag(const Token &Token, unsigned DiagID) {
    return Diag(Token.getLocation(), DiagID);
  }
  DiagnosticBuilder Diag(unsigned DiagID) {
    return Diag(Tok.getLocation(), DiagID);
  }
};

} // namespace soll
