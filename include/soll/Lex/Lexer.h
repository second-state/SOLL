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
  const char *BufferStart;
  const char *BufferEnd;
  SourceLocation FileLoc;
  const char *BufferPtr;
  mutable IdentifierTable Identifiers;

public:
  Lexer(FileID FID, const llvm::MemoryBuffer *FromFile, SourceManager &SM);

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

private:
  llvm::Optional<Token> Lex();

  llvm::Optional<Token> LexUnicode(uint32_t C, const char *CurPtr);

  Token FormTokenWithChars(const char *TokEnd, tok::TokenKind Kind) {
    Token Result;
    unsigned TokLen = TokEnd - BufferPtr;
    Result.setLength(TokLen);
    Result.setLocation(getSourceLocation(BufferPtr, TokLen));
    Result.setKind(Kind);
    BufferPtr = TokEnd;
    return Result;
  }

  static constexpr bool isObviouslySimpleCharacter(char C) {
    return C != '\\';
  }

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
  Token LexStringLiteral(const char *CurPtr);
  Token LexCharConstant(const char *CurPtr);
  Token LexEndOfFile(const char *CurPtr);
  void SkipWhitespace(const char *CurPtr);
  void SkipLineComment(const char *CurPtr);
  void SkipBlockComment(const char *CurPtr);

  bool isCodeCompletionPoint(const char *CurPtr) const;
};

} // namespace soll
