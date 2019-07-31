// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/Basic/Diagnostic.h"

namespace soll {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, NOWERROR,              \
             SHOWINSYSHEADER, CATEGORY)                                        \
  ENUM,
#define LEXSTART
#include "soll/Basic/DiagnosticLexKinds.inc"
#undef DIAG
  NUM_BUILTIN_LEX_DIAGNOSTICS
};
} // namespace diag
} // namespace soll
