// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/SourceLocation.h"
#include "soll/Basic/SourceManager.h"

namespace soll {

void SourceLocation::print(llvm::raw_ostream &OS, const SourceManager &SM) {
  if (!isValid()) {
    OS << "<invalid loc>";
    return;
  }

  const PresumedLoc PLoc = SM.getPresumedLoc(*this);

  if (PLoc.isInvalid()) {
    OS << "<invalid>";
    return;
  }
  // The macro expansion and spelling pos is identical for file locs.
  OS << PLoc.getFilename() << ':' << PLoc.getLine() << ':' << PLoc.getColumn();
}

} // namespace soll
