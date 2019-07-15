#include "soll/Basic/TokenKinds.h"
#include <llvm/Support/ErrorHandling.h>

namespace soll {
namespace tok {

static const char *const TokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "soll/Basic/TokenKinds.def"
    nullptr};

const char *getTokenName(TokenKind Kind) {
  if (Kind < tok::NUM_TOKENS)
    return TokNames[Kind];
  llvm_unreachable("unknown TokenKind");
  return nullptr;
}

const char *getPunctuatorSpelling(TokenKind Kind) {
  switch (Kind) {
#define PUNCTUATOR(X, Y)                                                       \
  case X:                                                                      \
    return Y;
#include "soll/Basic/TokenKinds.def"
  default:
    break;
  }
  return nullptr;
}

const char *getKeywordSpelling(TokenKind Kind) {
  switch (Kind) {
#define KEYWORD(X, Y)                                                          \
  case kw_##X:                                                                 \
    return #X;
#include "soll/Basic/TokenKinds.def"
  default:
    break;
  }
  return nullptr;
}

} // namespace tok
} // namespace soll
