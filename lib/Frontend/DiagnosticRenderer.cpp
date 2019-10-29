// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/DiagnosticRenderer.h"
#include <llvm/ADT/SmallVector.h>

namespace soll {

void DiagnosticRenderer::emitDiagnostic(FullSourceLoc Loc,
                                        DiagnosticsEngine::Level Level,
                                        llvm::StringRef Message,
                                        llvm::ArrayRef<CharSourceRange> Ranges,
                                        llvm::ArrayRef<FixItHint> FixItHints) {
  assert(Loc.hasManager() || Loc.isInvalid());

  if (!Loc.isValid()) {
    emitDiagnosticMessage(Loc, PresumedLoc(), Level, Message, Ranges);
  } else {
    llvm::SmallVector<CharSourceRange, 20> MutableRanges(Ranges.begin(),
                                                         Ranges.end());

    FullSourceLoc UnexpandedLoc = Loc;

    // Loc = Loc.getFileLoc();

    PresumedLoc PLoc = Loc.getPresumedLoc();

    // emitIncludeStack(Loc, PLoc, Level);
    emitDiagnosticMessage(Loc, PLoc, Level, Message, Ranges);
  }
}

} // namespace soll
