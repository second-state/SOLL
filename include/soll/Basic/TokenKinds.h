// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <llvm/Support/Compiler.h>

namespace soll {
namespace tok {

enum TokenKind : unsigned short {
#define TOK(X) X,
#include "soll/Basic/TokenKinds.def"
  NUM_TOKENS
};

const char *getTokenName(TokenKind Kind) LLVM_READNONE;
const char *getPunctuatorSpelling(TokenKind Kind) LLVM_READNONE;
const char *getKeywordSpelling(TokenKind Kind) LLVM_READNONE;

inline bool isAnyIdentifier(TokenKind K) {
  return (K == tok::identifier) || (K == tok::raw_identifier);
}

inline bool isStringLiteral(TokenKind K) {
  return K == tok::string_literal || K == tok::hex_string_literal;
}

inline bool isLiteral(TokenKind K) {
  return K == tok::numeric_constant || K == tok::char_constant ||
         isStringLiteral(K);
}

} // namespace tok
} // namespace soll
