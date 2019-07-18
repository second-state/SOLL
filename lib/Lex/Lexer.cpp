#include "soll/Lex/Lexer.h"
#include "soll/Basic/CharInfo.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticLex.h"
#include "soll/Lex/Token.h"
#include <llvm/Support/ConvertUTF.h>
#include <llvm/Support/UnicodeCharRanges.h>

namespace soll {

static inline CharSourceRange makeCharRange(Lexer &L, const char *Begin,
                                            const char *End) {
  return CharSourceRange::getCharRange(L.getSourceLocation(Begin),
                                       L.getSourceLocation(End));
}

Lexer::Lexer(FileID FID, const llvm::MemoryBuffer *InputFile, SourceManager &SM)
    : Diags(SM.getDiagnostics()), FileMgr(SM.getFileManager()), SourceMgr(SM),
      FileLoc(SM.getLocForStartOfFile(FID)),
      BufferStart(InputFile->getBufferStart()),
      BufferPtr(InputFile->getBufferStart()),
      BufferEnd(InputFile->getBufferEnd()) {
  if (BufferStart == BufferPtr) {
    // Determine the size of the BOM.
    llvm::StringRef Buf(BufferStart, BufferEnd - BufferStart);
    size_t BOMLength = llvm::StringSwitch<size_t>(Buf)
                           .StartsWith("\xEF\xBB\xBF", 3) // UTF-8 BOM
                           .Default(0);

    // Skip the BOM.
    BufferPtr += BOMLength;
  }
}

SourceLocation Lexer::getSourceLocation(const char *Loc,
                                        unsigned TokLen) const {
  assert(Loc >= BufferStart && Loc <= BufferEnd &&
         "Location out of range for this buffer!");

  unsigned CharNo = Loc - BufferStart;
  return FileLoc.getLocWithOffset(CharNo);
}

DiagnosticBuilder Lexer::Diag(const char *Loc, unsigned DiagID) const {
  return Diags.Report(getSourceLocation(Loc), DiagID);
}

llvm::Optional<Token> Lexer::PeekAhead(unsigned N) {
  assert(CachedLexPos + N > CachedTokens.size() && "Confused caching.");
  for (size_t C = CachedLexPos + N - CachedTokens.size(); C > 0; --C) {
    if (auto Result = Lex()) {
      CachedTokens.push_back(*Result);
    }
  }
  if (CachedLexPos + N == CachedTokens.size()) {
    return CachedTokens.back();
  }
  return llvm::None;
}

llvm::Optional<Token> Lexer::CachedLex() {
  if (CachedLexPos < CachedTokens.size()) {
    return CachedTokens[CachedLexPos++];
  } else if (!CachedTokens.empty()) {
    CachedTokens.clear();
    CachedLexPos = 0;
  }

  return Lex();
}

llvm::Optional<Token> Lexer::Lex() {
LexNextToken:
  const char *CurPtr = BufferPtr;
  if ((*CurPtr == ' ') || (*CurPtr == '\t')) {
    ++CurPtr;
    while ((*CurPtr == ' ') || (*CurPtr == '\t'))
      ++CurPtr;
    BufferPtr = CurPtr;
  }

  unsigned SizeTmp, SizeTmp2;
  char Char = getAndAdvanceChar(CurPtr);
  tok::TokenKind Kind;

  switch (Char) {
  case 0: // null
    // Found end of file?
    if (CurPtr - 1 == BufferEnd)
      return LexEndOfFile(CurPtr - 1);

    Diag(CurPtr - 1, diag::null_in_file);
    SkipWhitespace(CurPtr);

    // We know the lexer hasn't changed, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;

  case '\r':
    if (CurPtr[0] == '\n')
      Char = getAndAdvanceChar(CurPtr);
    LLVM_FALLTHROUGH;
  case '\n':
    SkipWhitespace(CurPtr);

    // We only saw whitespace, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;
  case ' ':
  case '\t':
  case '\f':
  case '\v':
  SkipHorizontalWhitespace:
    SkipWhitespace(CurPtr);

  SkipIgnoredUnits:
    CurPtr = BufferPtr;

    // If the next token is obviously a // or /* */ comment, skip it efficiently
    // too (without going through the big switch stmt).
    if (CurPtr[0] == '/' && CurPtr[1] == '/') {
      SkipLineComment(CurPtr + 2);
      goto SkipIgnoredUnits;
    } else if (CurPtr[0] == '/' && CurPtr[1] == '*') {
      SkipBlockComment(CurPtr + 2);
      goto SkipIgnoredUnits;
    } else if (isHorizontalWhitespace(*CurPtr)) {
      goto SkipHorizontalWhitespace;
    }
    // We only saw whitespace, so just try again with this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;

  // Integer Constants and Floating Constants.
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return LexNumericConstant(CurPtr);

  // Identifiers.
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case '_':
  case '$':
    return LexIdentifier(CurPtr);

  // Character Constants.
  case '\'':
    return LexCharConstant(CurPtr);

  // String Literals.
  case '"':
    return LexStringLiteral(CurPtr);

  // Punctuators.
  case '?':
    Kind = tok::question;
    break;
  case '[':
    Kind = tok::l_square;
    break;
  case ']':
    Kind = tok::r_square;
    break;
  case '(':
    Kind = tok::l_paren;
    break;
  case ')':
    Kind = tok::r_paren;
    break;
  case '{':
    Kind = tok::l_brace;
    break;
  case '}':
    Kind = tok::r_brace;
    break;
  case '.':
    Kind = tok::period;
    break;
  case '&':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '&') {
      Kind = tok::ampamp;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else if (Char == '=') {
      Kind = tok::ampequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::amp;
    }
    break;
  case '*':
    if (getCharAndSize(CurPtr, SizeTmp) == '=') {
      Kind = tok::starequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::star;
    }
    break;
  case '+':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '+') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::plusplus;
    } else if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::plusequal;
    } else {
      Kind = tok::plus;
    }
    break;
  case '-':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '-') { // --
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::minusminus;
    } else if (Char == '>') { // ->
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::arrow;
    } else if (Char == '=') { // -=
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::minusequal;
    } else {
      Kind = tok::minus;
    }
    break;
  case '~':
    Kind = tok::tilde;
    break;
  case '!':
    if (getCharAndSize(CurPtr, SizeTmp) == '=') {
      Kind = tok::exclaimequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::exclaim;
    }
    break;
  case '/':
    // Comments
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '/') { // Line comment.
      SkipLineComment(ConsumeChar(CurPtr, SizeTmp));

      // It is common for the tokens immediately after a // comment to be
      // whitespace (indentation for the next line).  Instead of going through
      // the big switch, handle it efficiently now.
      goto SkipIgnoredUnits;
    }

    if (Char == '*') { // /**/ comment.
      SkipBlockComment(ConsumeChar(CurPtr, SizeTmp));

      // We only saw whitespace, so just try again with this lexer.
      // (We manually eliminate the tail call to avoid recursion.)
      goto LexNextToken;
    }

    if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::slashequal;
    } else {
      Kind = tok::slash;
    }
    break;
  case '%':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      Kind = tok::percentequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::percent;
    }
    break;
  case '<':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '<') {
      char After = getCharAndSize(CurPtr + SizeTmp, SizeTmp2);
      if (After == '=') {
        Kind = tok::lesslessequal;
        CurPtr = ConsumeChar(ConsumeChar(CurPtr, SizeTmp), SizeTmp2);
      } else {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::lessless;
      }
    } else if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::lessequal;
    } else {
      Kind = tok::less;
    }
    break;
  case '>':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::greaterequal;
    } else if (Char == '>') {
      char After = getCharAndSize(CurPtr + SizeTmp, SizeTmp2);
      if (After == '=') {
        CurPtr = ConsumeChar(ConsumeChar(CurPtr, SizeTmp), SizeTmp2);
        Kind = tok::greatergreaterequal;
      } else {
        CurPtr = ConsumeChar(CurPtr, SizeTmp);
        Kind = tok::greatergreater;
      }
    } else {
      Kind = tok::greater;
    }
    break;
  case '^':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
      Kind = tok::caretequal;
    } else {
      Kind = tok::caret;
    }
    break;
  case '|':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      Kind = tok::pipeequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else if (Char == '|') {
      Kind = tok::pipepipe;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::pipe;
    }
    break;
  case ':':
    Kind = tok::colon;
    break;
  case ';':
    Kind = tok::semi;
    break;
  case '=':
    Char = getCharAndSize(CurPtr, SizeTmp);
    if (Char == '=') {
      Kind = tok::equalequal;
      CurPtr = ConsumeChar(CurPtr, SizeTmp);
    } else {
      Kind = tok::equal;
    }
    break;
  case ',':
    Kind = tok::comma;
    break;

  default: {
    if (isASCII(Char)) {
      Kind = tok::unknown;
      break;
    }

    llvm::UTF32 CodePoint;

    // We can't just reset CurPtr to BufferPtr because BufferPtr may point to
    // an escaped newline.
    --CurPtr;
    llvm::ConversionResult Status = llvm::convertUTF8Sequence(
        (const llvm::UTF8 **)&CurPtr, (const llvm::UTF8 *)BufferEnd, &CodePoint,
        llvm::strictConversion);
    if (Status == llvm::conversionOK) {
      return LexUnicode(CodePoint, CurPtr);
    }

    // Non-ASCII characters tend to creep into source code unintentionally.
    // Instead of letting the parser complain about the unknown token,
    // just diagnose the invalid UTF-8, then drop the character.
    Diag(CurPtr, diag::err_invalid_utf8);

    BufferPtr = CurPtr + 1;
    // We're pretending the character didn't exist, so just try again with
    // this lexer.
    // (We manually eliminate the tail call to avoid recursion.)
    goto LexNextToken;
  }
  }

  return FormTokenWithChars(CurPtr, Kind);
}

llvm::Optional<Token> Lexer::LexUnicode(uint32_t C, const char *CurPtr) {
  if (!isASCII(*BufferPtr)) {
    // Non-ASCII characters tend to creep into source code unintentionally.
    // Instead of letting the parser complain about the unknown token,
    // just drop the character.
    // Note that we can /only/ do this when the non-ASCII character is actually
    // spelled as Unicode, not written as a UCN. The standard requires that
    // we not throw away any possible preprocessor tokens, but there's a
    // loophole in the mapping of Unicode characters to basic character set
    // characters that allows us to map these particular characters to, say,
    // whitespace.
    Diag(BufferPtr, diag::err_non_ascii)
        << FixItHint::CreateRemoval(makeCharRange(*this, BufferPtr, CurPtr));

    BufferPtr = CurPtr;
    return llvm::None;
  }

  // Otherwise, we have an explicit UCN or a character that's unlikely to show
  // up by accident.
  return FormTokenWithChars(CurPtr, tok::unknown);
}

char Lexer::getCharAndSizeSlow(const char *Ptr, unsigned &Size) {
  // If we have a slash, look for an escaped newline.
  if (Ptr[0] == '\\') {
    ++Size;
    ++Ptr;
    // Common case, backslash-char where the char is not whitespace.
    if (!isWhitespace(Ptr[0]))
      return '\\';

    // See if we have optional whitespace characters between the slash and
    // newline.
    if (unsigned EscapedNewLineSize = getEscapedNewLineSize(Ptr)) {
      // Warn if there was whitespace between the backslash and newline.
      if (Ptr[0] != '\n' && Ptr[0] != '\r')
        Diag(Ptr, diag::backslash_newline_space);

      // Found backslash<whitespace><newline>.  Parse the char after it.
      Size += EscapedNewLineSize;
      Ptr += EscapedNewLineSize;

      // Use slow version to accumulate a correct size field.
      return getCharAndSizeSlow(Ptr, Size);
    }

    // Otherwise, this is not an escaped newline, just return the slash.
    return '\\';
  }

  // If this is neither, return a single character.
  ++Size;
  return *Ptr;
}

unsigned Lexer::getEscapedNewLineSize(const char *Ptr) {
  unsigned Size = 0;
  while (isWhitespace(Ptr[Size])) {
    ++Size;

    if (Ptr[Size - 1] != '\n' && Ptr[Size - 1] != '\r')
      continue;

    // If this is a \r\n or \n\r, skip the other half.
    if ((Ptr[Size] == '\r' || Ptr[Size] == '\n') && Ptr[Size - 1] != Ptr[Size])
      ++Size;

    return Size;
  }

  // Not an escaped newline, must be a \t or something else.
  return 0;
}

Token Lexer::LexIdentifier(const char *CurPtr) {
  // Match [_A-Za-z0-9]*, we have already matched [_A-Za-z$]
  unsigned Size;
  unsigned char C = *CurPtr++;
  while (isIdentifierBody(C))
    C = *CurPtr++;

  --CurPtr; // Back up over the skipped character.

  const char *IdStart = BufferPtr;
  Token Result = FormTokenWithChars(CurPtr, tok::raw_identifier);
  Result.setRawIdentifierData(IdStart);

  {
    IdentifierInfo *II = &Identifiers.get(llvm::StringRef(IdStart, Result.getLength()));
    Result.setIdentifierInfo(II);
    Result.setKind(II->getTokenID());
  }

  return Result;
}

Token Lexer::LexNumericConstant(const char *CurPtr) {
  unsigned Size;
  char C = getCharAndSize(CurPtr, Size);
  char PrevCh = 0;
  while (isNumberBody(C)) {
    CurPtr = ConsumeChar(CurPtr, Size);
    PrevCh = C;
    C = getCharAndSize(CurPtr, Size);
  }

  // Update the location of token as well as BufferPtr.
  const char *TokStart = BufferPtr;
  Token Result = FormTokenWithChars(CurPtr, tok::numeric_constant);
  Result.setLiteralData(TokStart);
  return Result;
}

Token Lexer::LexCharConstant(const char *CurPtr) {
  // Does this character contain the \0 character?
  const char *NulCharacter = nullptr;

  char C = getAndAdvanceChar(CurPtr);
  if (C == '\'') {
    Diag(BufferPtr, diag::ext_empty_character);
    return FormTokenWithChars(CurPtr, tok::unknown);
  }

  while (C != '\'') {
    // Skip escaped characters.
    if (C == '\\')
      C = getAndAdvanceChar(CurPtr);

    if (C == '\n' || C == '\r' ||              // Newline.
        (C == 0 && CurPtr - 1 == BufferEnd)) { // End of file.
      Diag(BufferPtr, diag::ext_unterminated_char_or_string) << 0;
      return FormTokenWithChars(CurPtr - 1, tok::unknown);
    }

    if (C == 0) {
      NulCharacter = CurPtr - 1;
    }
    C = getAndAdvanceChar(CurPtr);
  }

  // If a nul character existed in the character, warn about it.
  if (NulCharacter)
    Diag(NulCharacter, diag::null_in_char_or_string) << 0;

  // Update the location of token as well as BufferPtr.
  const char *TokStart = BufferPtr;
  Token Result = FormTokenWithChars(CurPtr, tok::char_constant);
  Result.setLiteralData(TokStart);
  return Result;
}

Token Lexer::LexStringLiteral(const char *CurPtr) {
  const char *AfterQuote = CurPtr;
  // Does this string contain the \0 character?
  const char *NulCharacter = nullptr;

  char C = getAndAdvanceChar(CurPtr);
  while (C != '"') {
    // Skip escaped characters.  Escaped newlines will already be processed by
    // getAndAdvanceChar.
    if (C == '\\')
      C = getAndAdvanceChar(CurPtr);

    if (C == '\n' || C == '\r' ||              // Newline.
        (C == 0 && CurPtr - 1 == BufferEnd)) { // End of file.
      Diag(BufferPtr, diag::ext_unterminated_char_or_string) << 1;
      return FormTokenWithChars(CurPtr - 1, tok::unknown);
    }

    if (C == 0) {
      NulCharacter = CurPtr - 1;
    }
    C = getAndAdvanceChar(CurPtr);
  }

  // If a nul character existed in the string, warn about it.
  if (NulCharacter)
    Diag(NulCharacter, diag::null_in_char_or_string) << 1;

  // Update the location of the token as well as the BufferPtr instance var.
  const char *TokStart = BufferPtr;
  Token Result = FormTokenWithChars(CurPtr, tok::string_literal);
  Result.setLiteralData(TokStart);
  return Result;
}

Token Lexer::LexEndOfFile(const char *CurPtr) {
  BufferPtr = CurPtr;
  return FormTokenWithChars(BufferEnd, tok::eof);
}

void Lexer::SkipWhitespace(const char *CurPtr) {
  // Whitespace - Skip it, then return the token after the whitespace.
  bool SawNewline = isVerticalWhitespace(CurPtr[-1]);

  unsigned char Char = *CurPtr;

  // Skip consecutive spaces efficiently.
  while (true) {
    // Skip horizontal whitespace very aggressively.
    while (isHorizontalWhitespace(Char))
      Char = *++CurPtr;

    // Otherwise if we have something other than whitespace, we're done.
    if (!isVerticalWhitespace(Char))
      break;

    // OK, but handle newline.
    SawNewline = true;
    Char = *++CurPtr;
  }

  // If this isn't immediately after a newline, there is leading space.
  char PrevChar = CurPtr[-1];
  bool HasLeadingSpace = !isVerticalWhitespace(PrevChar);

  BufferPtr = CurPtr;
}

bool Lexer::isCodeCompletionPoint(const char *CurPtr) const { return false; }

void Lexer::SkipLineComment(const char *CurPtr) {
  // Scan over the body of the comment.  The common case, when scanning, is that
  // the comment contains normal ascii characters with nothing interesting in
  // them.  As such, optimize for this case with the inner loop.
  //
  // This loop terminates with CurPtr pointing at the newline (or end of buffer)
  // character that ends the line comment.
  char C;
  while (true) {
    C = *CurPtr;
    // Skip over characters in the fast loop.
    while (C != 0 &&               // Potentially EOF.
           C != '\n' && C != '\r') // Newline or DOS-style newline.
      C = *++CurPtr;

    const char *NextLine = CurPtr;
    if (C != 0) {
      // We found a newline, see if it's escaped.
      const char *EscapePtr = CurPtr - 1;
      bool HasSpace = false;
      while (isHorizontalWhitespace(*EscapePtr)) { // Skip whitespace.
        --EscapePtr;
        HasSpace = true;
      }

      if (*EscapePtr == '\\')
        // Escaped newline.
        CurPtr = EscapePtr;
      else
        break; // This is a newline, we're done.

      // If there was space between the backslash and newline, warn about it.
      if (HasSpace)
        Diag(EscapePtr, diag::backslash_newline_space);
    }

    // Otherwise, this is a hard case.  Fall back on getAndAdvanceChar to
    // properly decode the character.  Read it in raw mode to avoid emitting
    // diagnostics about things like trigraphs.  If we see an escaped newline,
    // we'll handle it below.
    const char *OldPtr = CurPtr;
    C = getAndAdvanceChar(CurPtr);

    // If we only read only one character, then no special handling is needed.
    // We're done and can skip forward to the newline.
    if (C != 0 && CurPtr == OldPtr + 1) {
      CurPtr = NextLine;
      break;
    }

    // If we read multiple characters, and one of those characters was a \r or
    // \n, then we had an escaped newline within the comment.  Emit diagnostic
    // unless the next line is also a // comment.
    if (CurPtr != OldPtr + 1 && C != '/' &&
        (CurPtr == BufferEnd + 1 || CurPtr[0] != '/')) {
      for (; OldPtr != CurPtr; ++OldPtr)
        if (OldPtr[0] == '\n' || OldPtr[0] == '\r') {
          // Okay, we found a // comment that ends in a newline, if the next
          // line is also a // comment, but has spaces, don't emit a diagnostic.
          if (isWhitespace(C)) {
            const char *ForwardPtr = CurPtr;
            while (isWhitespace(*ForwardPtr)) // Skip whitespace.
              ++ForwardPtr;
            if (ForwardPtr[0] == '/' && ForwardPtr[1] == '/')
              break;
          }

          Diag(OldPtr - 1, diag::ext_multi_line_line_comment);
          break;
        }
    }

    if (C == '\r' || C == '\n' || CurPtr == BufferEnd + 1) {
      --CurPtr;
      break;
    }
  }

  ++CurPtr;
  BufferPtr = CurPtr;
}

/// isBlockCommentEndOfEscapedNewLine - Return true if the specified newline
/// character (either \\n or \\r) is part of an escaped newline sequence.  Issue
/// a diagnostic if so.  We know that the newline is inside of a block comment.
static bool isEndOfBlockCommentWithEscapedNewLine(const char *CurPtr,
                                                  Lexer *L) {
  assert(CurPtr[0] == '\n' || CurPtr[0] == '\r');

  // Back up off the newline.
  --CurPtr;

  // If this is a two-character newline sequence, skip the other character.
  if (CurPtr[0] == '\n' || CurPtr[0] == '\r') {
    // \n\n or \r\r -> not escaped newline.
    if (CurPtr[0] == CurPtr[1])
      return false;
    // \n\r or \r\n -> skip the newline.
    --CurPtr;
  }

  // If we have horizontal whitespace, skip over it.  We allow whitespace
  // between the slash and newline.
  bool HasSpace = false;
  while (isHorizontalWhitespace(*CurPtr) || *CurPtr == 0) {
    --CurPtr;
    HasSpace = true;
  }

  // If we have a slash, we know this is an escaped newline.
  if (CurPtr[0] != '\\' || CurPtr[-1] != '*')
    return false;

  // Warn about having an escaped newline between the */ characters.
  L->Diag(CurPtr, diag::escaped_newline_block_comment_end);

  // If there was space between the backslash and newline, warn about it.
  if (HasSpace)
    L->Diag(CurPtr, diag::backslash_newline_space);

  return true;
}

#ifdef __SSE2__
#include <emmintrin.h>
#elif __ALTIVEC__
#include <altivec.h>
#undef bool
#endif

void Lexer::SkipBlockComment(const char *CurPtr) {
  unsigned CharSize;
  unsigned char C = getCharAndSize(CurPtr, CharSize);
  CurPtr += CharSize;
  if (C == 0 && CurPtr == BufferEnd + 1) {
    Diag(BufferPtr, diag::err_unterminated_block_comment);
    --CurPtr;

    BufferPtr = CurPtr;
    return;
  }

  // Check to see if the first character after the '/*' is another /.  If so,
  // then this slash does not end the block comment, it is part of it.
  if (C == '/')
    C = *CurPtr++;

  while (true) {
    // Skip over all non-interesting characters until we find end of buffer or a
    // (probably ending) '/' character.
    if (CurPtr + 24 < BufferEnd) {
      // While not aligned to a 16-byte boundary.
      while (C != '/' && ((intptr_t)CurPtr & 0x0F) != 0)
        C = *CurPtr++;

      if (C == '/')
        goto FoundSlash;

#ifdef __SSE2__
      __m128i Slashes = _mm_set1_epi8('/');
      while (CurPtr + 16 <= BufferEnd) {
        int cmp = _mm_movemask_epi8(
            _mm_cmpeq_epi8(*(const __m128i *)CurPtr, Slashes));
        if (cmp != 0) {
          // Adjust the pointer to point directly after the first slash. It's
          // not necessary to set C here, it will be overwritten at the end of
          // the outer loop.
          CurPtr += llvm::countTrailingZeros<unsigned>(cmp) + 1;
          goto FoundSlash;
        }
        CurPtr += 16;
      }
#elif __ALTIVEC__
      __vector unsigned char Slashes = {'/', '/', '/', '/', '/', '/', '/', '/',
                                        '/', '/', '/', '/', '/', '/', '/', '/'};
      while (CurPtr + 16 <= BufferEnd &&
             !vec_any_eq(*(const vector unsigned char *)CurPtr, Slashes))
        CurPtr += 16;
#else
      // Scan for '/' quickly.  Many block comments are very large.
      while (CurPtr[0] != '/' && CurPtr[1] != '/' && CurPtr[2] != '/' &&
             CurPtr[3] != '/' && CurPtr + 4 < BufferEnd) {
        CurPtr += 4;
      }
#endif

      // It has to be one of the bytes scanned, increment to it and read one.
      C = *CurPtr++;
    }

    // Loop to scan the remainder.
    while (C != '/' && C != '\0')
      C = *CurPtr++;

    if (C == '/') {
    FoundSlash:
      if (CurPtr[-2] == '*') // We found the final */.  We're done!
        break;

      if ((CurPtr[-2] == '\n' || CurPtr[-2] == '\r')) {
        if (isEndOfBlockCommentWithEscapedNewLine(CurPtr - 2, this)) {
          // We found the final */, though it had an escaped newline between the
          // * and /.  We're done!
          break;
        }
      }
      if (CurPtr[0] == '*' && CurPtr[1] != '/') {
        // If this is a /* inside of the comment, emit a warning.  Don't do this
        // if this is a /*/, which will end the comment.  This misses cases with
        // embedded escaped newlines, but oh well.
        Diag(CurPtr - 1, diag::warn_nested_block_comment);
      }
    } else if (C == 0 && CurPtr == BufferEnd + 1) {
      Diag(BufferPtr, diag::err_unterminated_block_comment);
      // Note: the user probably forgot a */.  We could continue immediately
      // after the /*, but this would involve lexing a lot of what really is the
      // comment, which surely would confuse the parser.
      --CurPtr;

      BufferPtr = CurPtr;
      return;
    }

    C = *CurPtr++;
  }

  // It is common for the tokens immediately after a /**/ comment to be
  // whitespace.  Instead of going through the big switch, handle it
  // efficiently now.  This is safe even in KeepWhitespaceMode because we would
  // have already returned above with the comment as a token.
  if (isHorizontalWhitespace(*CurPtr)) {
    SkipWhitespace(CurPtr + 1);
    return;
  }

  // Otherwise, just return so that the next character will be lexed as a token.
  BufferPtr = CurPtr;
}

} // namespace soll
