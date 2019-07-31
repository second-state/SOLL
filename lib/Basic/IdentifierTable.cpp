// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/IdentifierTable.h"

namespace {

enum { KEYALL = 0xffffff };
enum KeywordStatus { KS_Disabled, KS_Enabled, KS_Future };

} // namespace

namespace soll {

static KeywordStatus getKeywordStatus(unsigned Flags) {
  if (Flags == KEYALL)
    return KS_Enabled;
  return KS_Disabled;
}

static void AddKeyword(llvm::StringRef Keyword, tok::TokenKind TokenCode,
                       unsigned Flags, IdentifierTable &Table) {
  KeywordStatus AddResult = getKeywordStatus(Flags);

  if (AddResult == KS_Disabled)
    return;

  IdentifierInfo &Info =
      Table.get(Keyword, AddResult == KS_Future ? tok::identifier : TokenCode);
  Info.setIsFutureCompatKeyword(AddResult == KS_Future);
}

void IdentifierTable::AddKeywords() {
#define KEYWORD(NAME, FLAGS)                                                   \
  AddKeyword(llvm::StringRef(#NAME), tok::kw_##NAME, FLAGS, *this);
#define ALIAS(NAME, TOK, FLAGS)                                                \
  AddKeyword(llvm::StringRef(NAME), tok::kw_##TOK, FLAGS, *this);
#define TESTING_KEYWORD(NAME, FLAGS)
#include "soll/Basic/TokenKinds.def"
}
} // namespace soll
