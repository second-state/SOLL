// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/StringRef.h>

namespace soll {

namespace diag {

enum {
  DIAG_SIZE_COMMON = 300,
  DIAG_SIZE_DRIVER = 200,
  DIAG_SIZE_FRONTEND = 150,
  DIAG_SIZE_LEX = 400,
  DIAG_SIZE_PARSE = 500,
  DIAG_SIZE_SEMA = 3500,
};

enum {
  DIAG_START_COMMON = 0,
  DIAG_START_DRIVER = DIAG_START_COMMON + DIAG_SIZE_COMMON,
  DIAG_START_FRONTEND = DIAG_START_DRIVER + DIAG_SIZE_DRIVER,
  DIAG_START_LEX = DIAG_START_FRONTEND + DIAG_SIZE_FRONTEND,
  DIAG_START_PARSE = DIAG_START_LEX + DIAG_SIZE_LEX,
  DIAG_START_SEMA = DIAG_START_PARSE + DIAG_SIZE_PARSE,
  DIAG_UPPER_LIMIT = DIAG_START_SEMA + DIAG_SIZE_SEMA
};

using kind = unsigned;

enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, NOWERROR, CATEGORY)    \
  ENUM,
#define COMMONSTART
#include "soll/Basic/DiagnosticCommonKinds.inc"
  NUM_BUILTIN_COMMON_DIAGNOSTICS
#undef DIAG
};

enum class Severity {
  // NOTE: 0 means "uncomputed".
  Ignored = 1, ///< Do not present this diagnostic, ignore it.
  Remark = 2,  ///< Present this diagnostic as a remark.
  Warning = 3, ///< Present this diagnostic as a warning.
  Error = 4,   ///< Present this diagnostic as an error.
  Fatal = 5    ///< Present this diagnostic as a fatal error.
};

enum class Flavor {
  WarningOrError, ///< A diagnostic that indicates a problem or potential
                  ///< problem. Can be made fatal by -Werror.
  Remark          ///< A diagnostic that indicates normal progress through
                  ///< compilation.
};

} // namespace diag

class SourceLocation;
class DiagnosticsEngine;

class DiagnosticIDs : public llvm::RefCountedBase<DiagnosticIDs> {
public:
  enum Level { Ignored, Note, Remark, Warning, Error, Fatal };

  llvm::StringRef getDescription(unsigned DiagID) const;

private:
  DiagnosticIDs::Level
  getDiagnosticLevel(unsigned DiagID, SourceLocation Loc,
                     const DiagnosticsEngine &Diag) const LLVM_READONLY;

  diag::Severity
  getDiagnosticSeverity(unsigned DiagID, SourceLocation Loc,
                        const DiagnosticsEngine &Diag) const LLVM_READONLY;

  bool ProcessDiag(DiagnosticsEngine &Diag) const;

  void EmitDiag(DiagnosticsEngine &Diag, Level DiagLevel) const;

  bool isUnrecoverable(unsigned DiagID) const;

  friend class DiagnosticsEngine;
};

} // namespace soll
