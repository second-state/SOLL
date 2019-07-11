#pragma once
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/StringRef.h>

namespace soll {

namespace diag {

enum {
  DIAG_SIZE_COMMON = 300,
  DIAG_SIZE_DRIVER = 200,
  DIAG_SIZE_FRONTEND = 150,
  DIAG_SIZE_SERIALIZATION = 120,
  DIAG_SIZE_LEX = 400,
  DIAG_SIZE_PARSE = 500,
  DIAG_SIZE_AST = 150,
  DIAG_SIZE_COMMENT = 100,
  DIAG_SIZE_CROSSTU = 100,
  DIAG_SIZE_SEMA = 3500,
  DIAG_SIZE_ANALYSIS = 100,
  DIAG_SIZE_REFACTORING = 1000,
};

enum {
  DIAG_START_COMMON = 0,
  DIAG_START_DRIVER = DIAG_START_COMMON + DIAG_SIZE_COMMON,
  DIAG_START_FRONTEND = DIAG_START_DRIVER + DIAG_SIZE_DRIVER,
  DIAG_START_SERIALIZATION = DIAG_START_FRONTEND + DIAG_SIZE_FRONTEND,
  DIAG_START_LEX = DIAG_START_SERIALIZATION + DIAG_SIZE_SERIALIZATION,
  DIAG_START_PARSE = DIAG_START_LEX + DIAG_SIZE_LEX,
  DIAG_START_AST = DIAG_START_PARSE + DIAG_SIZE_PARSE,
  DIAG_START_COMMENT = DIAG_START_AST + DIAG_SIZE_AST,
  DIAG_START_CROSSTU = DIAG_START_COMMENT + DIAG_SIZE_CROSSTU,
  DIAG_START_SEMA = DIAG_START_CROSSTU + DIAG_SIZE_COMMENT,
  DIAG_START_ANALYSIS = DIAG_START_SEMA + DIAG_SIZE_SEMA,
  DIAG_START_REFACTORING = DIAG_START_ANALYSIS + DIAG_SIZE_ANALYSIS,
  DIAG_UPPER_LIMIT = DIAG_START_REFACTORING + DIAG_SIZE_REFACTORING
};

using kind = unsigned;

enum {
#define DIAG(ENUM, FLAGS, DEFAULT_MAPPING, DESC, GROUP, NOWERROR,              \
             SHOWINSYSHEADER, CATEGORY)                                        \
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
  enum Level {
    Ignored, Note, Remark, Warning, Error, Fatal
  };

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
