// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/Basic/Diagnostic.h"

namespace soll {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, NOWERROR, CATEGORY)    \
  ENUM,
#define SEMASTART
#include "soll/Basic/DiagnosticSemaKinds.inc"
#undef DIAG
  NUM_BUILTIN_SEMA_DIAGNOSTICS
};
} // namespace diag
} // namespace soll
