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

FileID FullSourceLoc::getFileID() const {
  assert(isValid());
  return SrcMgr->getFileID(*this);
}

PresumedLoc FullSourceLoc::getPresumedLoc() const {
  if (!isValid())
    return PresumedLoc();

  return SrcMgr->getPresumedLoc(*this);
}

unsigned FullSourceLoc::getFileOffset() const {
  assert(isValid());
  return SrcMgr->getFileOffset(*this);
}

llvm::Optional<unsigned> FullSourceLoc::getLineNumber() const {
  assert(isValid());
  return SrcMgr->getLineNumber(getFileID(), getFileOffset());
}

llvm::Optional<unsigned> FullSourceLoc::getColumnNumber() const {
  assert(isValid());
  return SrcMgr->getColumnNumber(getFileID(), getFileOffset());
}

const FileEntry *FullSourceLoc::getFileEntry() const {
  assert(isValid());
  return SrcMgr->getFileEntryForID(getFileID());
}

llvm::StringRef FullSourceLoc::getBufferData() const {
  assert(isValid());
  return SrcMgr->getBuffer(SrcMgr->getFileID(*this))->getBuffer();
}

std::pair<FileID, unsigned> FullSourceLoc::getDecomposedLoc() const {
  return SrcMgr->getDecomposedLoc(*this);
}

} // namespace soll
