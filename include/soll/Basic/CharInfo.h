// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <cctype>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Compiler.h>

namespace soll {
/// Returns true if this is an ASCII character.
LLVM_READNONE inline bool isASCII(char c) {
  return static_cast<unsigned char>(c) <= 127;
}

/// Returns true if this is a valid first character of a C identifier,
/// which is [a-zA-Z_$].
LLVM_READONLY inline bool isIdentifierHead(unsigned char c) {
  return std::isalpha(c) || c == '$' || c == '_';
}

/// Returns true if this is a body character of a C identifier,
/// which is [a-zA-Z0-9_].
LLVM_READONLY inline bool isIdentifierBody(unsigned char c) {
  return std::isalnum(c) || c == '$' || c == '_';
}

/// Returns true if this character is horizontal ASCII whitespace:
/// ' ', '\\t', '\\f', '\\v'.
///
/// Note that this returns false for '\\0'.
LLVM_READONLY inline bool isHorizontalWhitespace(unsigned char c) {
  return c == ' ' || c == '\t' || c == '\f' || c == '\v';
}

/// Returns true if this character is vertical ASCII whitespace: '\\n', '\\r'.
///
/// Note that this returns false for '\\0'.
LLVM_READONLY inline bool isVerticalWhitespace(unsigned char c) {
  return c == '\n' || c == '\r';
}

/// Return true if this character is horizontal or vertical ASCII whitespace:
/// ' ', '\\t', '\\f', '\\v', '\\n', '\\r'.
///
/// Note that this returns false for '\\0'.
LLVM_READONLY inline bool isWhitespace(unsigned char c) {
  return std::isspace(c);
}

/// Return true if this character is an ASCII digit: [0-9]
LLVM_READONLY inline bool isDigit(unsigned char c) { return std::isdigit(c); }

/// Return true if this character is a lowercase ASCII letter: [a-z]
LLVM_READONLY inline bool isLowercase(unsigned char c) {
  return std::islower(c);
}

/// Return true if this character is an uppercase ASCII letter: [A-Z]
LLVM_READONLY inline bool isUppercase(unsigned char c) {
  return std::isupper(c);
}

/// Return true if this character is an ASCII letter: [a-zA-Z]
LLVM_READONLY inline bool isLetter(unsigned char c) { return std::isalpha(c); }

/// Return true if this character is an ASCII letter or digit: [a-zA-Z0-9]
LLVM_READONLY inline bool isAlphanumeric(unsigned char c) {
  return std::isalnum(c);
}

/// Return true if this character is an ASCII hex digit: [0-9a-fA-F]
LLVM_READONLY inline bool isHexDigit(unsigned char c) {
  return std::isxdigit(c);
}

/// Return true if this character is an ASCII punctuation character.
///
/// Note that '_' is both a punctuation character and an identifier character!
LLVM_READONLY inline bool isPunctuation(unsigned char c) {
  return std::ispunct(c);
}

/// Return true if this is the body character of a C preprocessing number,
/// which is [0-9.].
LLVM_READONLY inline bool isNumberBody(unsigned char c) {
  return std::isdigit(c) || c == '.';
}

/// Return true if this character is an ASCII printable character; that is, a
/// character that should take exactly one column to print in a fixed-width
/// terminal.
LLVM_READONLY inline bool isPrintable(unsigned char c) {
  return std::isprint(c);
}

/// Converts the given ASCII character to its lowercase equivalent.
///
/// If the character is not an uppercase character, it is returned as is.
LLVM_READONLY inline char toLowercase(char c) { return std::tolower(c); }

/// Converts the given ASCII character to its uppercase equivalent.
///
/// If the character is not a lowercase character, it is returned as is.
LLVM_READONLY inline char toUppercase(char c) { return std::toupper(c); }

/// Return true if this is a valid ASCII identifier.
///
/// Note that this is a very simple check; it does not accept UCNs as valid
/// identifier characters.
LLVM_READONLY inline bool isValidIdentifier(llvm::StringRef S) {
  if (S.empty() || !isIdentifierHead(S[0]))
    return false;

  for (llvm::StringRef::iterator I = S.begin(), E = S.end(); I != E; ++I)
    if (!isIdentifierBody(*I))
      return false;

  return true;
}

} // namespace soll
