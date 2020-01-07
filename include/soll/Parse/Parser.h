// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/AST.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Lex/Lexer.h"
#include "soll/Lex/Token.h"
#include "soll/Sema/Sema.h"

namespace soll {

inline std::string stringUnquote(const std::string &Quoted) {
  std::string Result;
  assert(Quoted.size() >= 2 && "string token with size < 2!");
  const char *TokBegin = Quoted.data();
  const char *TokEnd = TokBegin + Quoted.size() - 1;
  assert(TokBegin[0] == '"' || TokBegin[0] == '\'');
  assert(TokEnd[0] == TokBegin[0]);
  ++TokBegin;

  const char *TokBuf = TokBegin;
  while (TokBuf != TokEnd) {
    if (TokBuf[0] != '\\') {
      const char *InStart = TokBuf;
      do {
        ++TokBuf;
      } while (TokBuf != TokEnd && TokBuf[0] != '\\');
      Result.append(InStart, TokBuf - InStart);
      continue;
    }
    if (TokBuf[1] == 'u') {
      std::uint_fast8_t UcnLen = 4;
      std::uint32_t UcnVal = 0;
      TokBuf += 2;
      for (; TokBuf != TokEnd && UcnLen; --UcnLen) {
        unsigned CharVal = llvm::hexDigitValue(TokBuf[0]);
        UcnVal <<= 4;
        UcnVal |= CharVal;
      }
      assert(UcnLen == 0 && "Unicode escape incompleted");

      // Convert to UTF8
      std::uint_fast8_t BytesToWrite = 0;
      if (UcnVal < 0x80U) {
        BytesToWrite = 1;
      } else if (UcnVal < 0x800U) {
        BytesToWrite = 2;
      } else if (UcnVal < 0x10000U) {
        BytesToWrite = 3;
      } else {
        BytesToWrite = 4;
      }
      constexpr const unsigned ByteMask = 0xBF;
      constexpr const unsigned ByteMark = 0x80;
      static constexpr const unsigned FirstByteMark[5] = {0x00, 0x00, 0xC0,
                                                          0xE0, 0xF0};
      std::array<char, 4> Buffer;
      char *ResultBuf = &Buffer[5];
      switch (BytesToWrite) {
      case 4:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 3:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 2:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 1:
        *--ResultBuf = static_cast<char>(UcnVal | FirstByteMark[BytesToWrite]);
      }
      Result.append(ResultBuf, BytesToWrite);
      continue;
    }
    TokBuf += 2;
    char ResultChar = TokBuf[1];
    switch (ResultChar) {
    case 'x': {
      std::uint_fast8_t HexLen = 2;
      std::uint8_t HexVal = 0;
      for (; TokBuf != TokEnd && HexLen; --HexLen) {
        unsigned CharVal = llvm::hexDigitValue(TokBuf[0]);
        HexVal <<= 4;
        HexVal |= CharVal;
      }
      assert(HexLen == 0 && "Hex escape incompleted");
      ResultChar = static_cast<char>(HexVal);
      break;
    }
    case '\\':
    case '\'':
    case '\"':
      break;
    case 'b':
      ResultChar = '\b';
      break;
    case 'f':
      ResultChar = '\f';
      break;
    case 'n':
      ResultChar = '\n';
      break;
    case 'r':
      ResultChar = '\r';
      break;
    case 't':
      ResultChar = '\t';
      break;
    case 'v':
      ResultChar = '\v';
      break;
    default:
      assert(false && "unknown escape sequence!");
      break;
    }
    Result.push_back(ResultChar);
  }
  return Result;
}

inline std::string hexUnquote(const std::string &Quoted) {
  assert(Quoted.size() % 2 == 0 && "Hex escape incompleted");
  std::string Result;
  for (std::size_t I = 0; I < Quoted.size(); I += 2) {
    Result.push_back((llvm::hexDigitValue(Quoted[I]) << 4) |
                     llvm::hexDigitValue(Quoted[I + 1]));
  }
  return Result;
}

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
  unsigned short ParenCount = 0, BracketCount = 0, BraceCount = 0;

public:
  Parser(Lexer &TheLexer, Sema &Actions, DiagnosticsEngine &Diags);
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
  DataLocation parseDataLocation();

  FunctionHeaderParserResult parseFunctionHeader(bool ForceEmptyName,
                                                 bool AllowModifiers);
  std::unique_ptr<FunctionDecl>
  parseFunctionDefinitionOrFunctionTypeStateVariable();
  std::unique_ptr<VarDecl>
  parseVariableDeclaration(VarDeclParserOptions const &Options = {},
                           TypePtr &&LookAheadArrayType = nullptr);
  std::unique_ptr<EventDecl> parseEventDefinition();
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
                        llvm::StringRef DiagMsg = "");
  bool ExpectAndConsumeSemi(unsigned DiagID = diag::err_expected);

public:
  class ParseScope {
    Parser *Self;
    ParseScope(const ParseScope &) = delete;
    ParseScope &operator=(const ParseScope &) = delete;

  public:
    ParseScope(Parser *Self, unsigned ScopeFlags) : Self(Self) {
      Self->EnterScope(ScopeFlags);
    }
    void Exit() {
      if (Self) {
        Self->ExitScope();
        Self = nullptr;
      }
    }
    ~ParseScope() { Exit(); }
  };
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
