// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Lex/Token.h"
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

namespace soll {

class DiagnosticsEngine;
class FileManager;
class SourceManager;

class Lexer {
  DiagnosticsEngine &Diags;
  FileManager &FileMgr;
  SourceManager &SourceMgr;
  SourceLocation FileLoc;
  const char *BufferStart;
  const char *BufferPtr;
  const char *BufferEnd;
  mutable IdentifierTable Identifiers;

  using CachedTokensTy = llvm::SmallVector<Token, 1>;
  CachedTokensTy CachedTokens;
  CachedTokensTy::size_type CachedLexPos = 0;

public:
  Lexer(FileID FID, const llvm::MemoryBuffer *FromFile, SourceManager &SM);
  Lexer(SourceLocation FileLoc, const char *BufStart, const char *BufPtr,
        const char *BufEnd, const SourceManager &SM);

  Lexer(const Lexer &) = delete;
  Lexer &operator=(const Lexer &) = delete;

  void Initialize() { Identifiers.AddKeywords(); }

  DiagnosticsEngine &getDiagnostics() const { return Diags; }
  FileManager &getFileManager() const { return FileMgr; }
  SourceManager &getSourceManager() const { return SourceMgr; }

  SourceLocation getFileLoc() const { return FileLoc; }

  llvm::StringRef getBuffer() const {
    return llvm::StringRef(BufferStart, BufferEnd - BufferStart);
  }

  DiagnosticBuilder Diag(const char *Loc, unsigned DiagID) const;

  SourceLocation getSourceLocation(const char *Loc, unsigned TokLen = 1) const;

  SourceLocation getSourceLocation() { return getSourceLocation(BufferPtr); }

  llvm::Optional<Token> LookAhead(unsigned N) {
    if (CachedLexPos + N < CachedTokens.size())
      return CachedTokens[CachedLexPos + N];
    else
      return PeekAhead(N + 1);
  }

  llvm::Optional<Token> CachedLex();

  void EnterTokenStream(const Token *Toks, unsigned NumToks);

private:
  llvm::Optional<Token> PeekAhead(unsigned N);

  llvm::Optional<Token> Lex();

  llvm::Optional<Token> LexUnicode(uint32_t C, const char *CurPtr);

  void skipBOM();

  Token FormTokenWithChars(const char *TokEnd, tok::TokenKind Kind) {
    Token Result;
    unsigned TokLen = TokEnd - BufferPtr;
    Result.setLength(TokLen);
    Result.setLocation(getSourceLocation(BufferPtr, TokLen));
    Result.setKind(Kind);
    BufferPtr = TokEnd;
    return Result;
  }

  static constexpr bool isObviouslySimpleCharacter(char C) { return C != '\\'; }

  inline char getAndAdvanceChar(const char *&Ptr) {
    if (isObviouslySimpleCharacter(Ptr[0]))
      return *Ptr++;

    unsigned Size = 0;
    char C = getCharAndSizeSlow(Ptr, Size);
    Ptr += Size;
    return C;
  }

  const char *ConsumeChar(const char *Ptr, unsigned Size) {
    if (Size == 1)
      return Ptr + Size;

    Size = 0;
    getCharAndSizeSlow(Ptr, Size);
    return Ptr + Size;
  }

  inline char getCharAndSize(const char *Ptr, unsigned &Size) {
    if (isObviouslySimpleCharacter(Ptr[0])) {
      Size = 1;
      return *Ptr;
    }

    Size = 0;
    return getCharAndSizeSlow(Ptr, Size);
  }

  char getCharAndSizeSlow(const char *Ptr, unsigned &Size);

  static unsigned getEscapedNewLineSize(const char *P);

  Token LexIdentifier(const char *CurPtr);
  Token LexNumericConstant(const char *CurPtr);
  Token LexStringLiteral(const char *CurPtr, char Quote, tok::TokenKind Kind);
  Token LexEndOfFile(const char *CurPtr);
  void SkipWhitespace(const char *CurPtr);
  void SkipLineComment(const char *CurPtr);
  void SkipBlockComment(const char *CurPtr);

public:
  static unsigned MeasureTokenLength(SourceLocation Loc,
                                     const SourceManager &SM);
};

} // namespace soll
