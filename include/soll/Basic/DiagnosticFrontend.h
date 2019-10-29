// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/Basic/Diagnostic.h"

namespace soll {
namespace diag {
enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, NOWERROR, CATEGORY)    \
  ENUM,
#define FRONTENDSTART
#include "soll/Basic/DiagnosticFrontendKinds.inc"
#undef DIAG
  NUM_BUILTIN_FRONTEND_DIAGNOSTICS
};
} // namespace diag
} // namespace soll
