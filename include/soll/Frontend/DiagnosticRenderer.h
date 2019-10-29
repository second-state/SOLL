// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/SourceLocation.h"
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>

namespace soll {

class DiagnosticRenderer {
public:
  virtual ~DiagnosticRenderer() = default;

protected:
  virtual void
  emitDiagnosticMessage(FullSourceLoc Loc, PresumedLoc PLoc,
                        DiagnosticsEngine::Level Level, llvm::StringRef Message,
                        llvm::ArrayRef<CharSourceRange> Ranges) = 0;

  virtual void emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
                                 DiagnosticsEngine::Level Level,
                                 llvm::ArrayRef<CharSourceRange> Ranges) = 0;

public:
  void emitDiagnostic(FullSourceLoc Loc, DiagnosticsEngine::Level Level,
                      llvm::StringRef Message,
                      llvm::ArrayRef<CharSourceRange> Ranges,
                      llvm::ArrayRef<FixItHint> FixItHints);
};

} // namespace soll
