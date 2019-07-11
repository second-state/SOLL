#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/FileManager.h"
#include "soll/Basic/SourceLocation.h"
#include <llvm/ADT/BitVector.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>

namespace soll {

namespace SrcMgr {

class FileInfo {
  SourceLocation IncludeLoc;
  const FileEntry *File;
  mutable std::unique_ptr<llvm::MemoryBuffer> Buffer;

public:
  static FileInfo get(SourceLocation IL, const FileEntry *F) {
    FileInfo X;
    X.IncludeLoc = IL;
    X.File = F;
    return X;
  }

  SourceLocation getIncludeLoc() const { return IncludeLoc; }

  const FileEntry *getFileEntry() const { return File; }

  llvm::MemoryBuffer *getBuffer(DiagnosticsEngine &Diag,
                                const SourceManager &SM,
                                SourceLocation Loc = SourceLocation()) const;
};

class SLocEntry {
  unsigned Offset;
  FileInfo File;

public:
  SLocEntry() : Offset(), File() {}

  unsigned getOffset() const { return Offset; }

  const FileInfo &getFile() const { return File; }

  static SLocEntry get(unsigned Offset, FileInfo &&FI) {
    SLocEntry E;
    E.Offset = Offset;
    E.File = std::move(FI);
    return E;
  }
};

} // namespace SrcMgr

class SourceManager : public llvm::RefCountedBase<SourceManager> {
  DiagnosticsEngine &Diag;

  FileManager &FileMgr;

  llvm::DenseMap<const FileEntry *, std::unique_ptr<llvm::MemoryBuffer>>
      FileBuffers;

  llvm::SmallVector<SrcMgr::SLocEntry, 0> LocalSLocEntryTable;

  mutable llvm::SmallVector<SrcMgr::SLocEntry, 0> LoadedSLocEntryTable;

  static const unsigned MaxLoadedOffset = 1U << 31U;

  unsigned NextLocalOffset = 0;
  unsigned CurrentLoadedOffset = MaxLoadedOffset;

  llvm::BitVector SLocEntryLoaded;

  FileID MainFileID;

  mutable FileID LastFileIDLookup;

  SourceManager(const SourceManager &) = delete;
  SourceManager &operator=(const SourceManager &) = delete;

public:
  SourceManager(DiagnosticsEngine &Diag, FileManager &FileMgr);

  DiagnosticsEngine &getDiagnostics() const { return Diag; }

  FileManager &getFileManager() const { return FileMgr; }

  FileID getMainFileID() const { return MainFileID; }
  void setMainFileID(FileID FID) { MainFileID = FID; }

  FileID createFileID(const FileEntry *SourceFile, SourceLocation IncludePos,
                      int LoadedID = 0, unsigned LoadedOffset = 0);

  FileID getOrCreateFileID(const FileEntry *SourceFile) {
    FileID ID = translateFile(SourceFile);
    return ID.isValid() ? ID : createFileID(SourceFile, SourceLocation());
  }

  llvm::MemoryBuffer *getBuffer(FileID FID,
                                SourceLocation Loc = SourceLocation()) const {
    const SrcMgr::SLocEntry *Entry = getSLocEntry(FID);
    if (!Entry) {
      return nullptr;
    }

    return Entry->getFile().getBuffer(Diag, *this, Loc);
  }

  FileID getFileID(SourceLocation SpellingLoc) const { return FileID(); }

  llvm::StringRef getFilename(SourceLocation SpellingLoc) const {
    return llvm::StringRef();
  }

  SourceLocation getLocForStartOfFile(FileID FID) const {
    const auto Entry = getSLocEntry(FID);
    if (!Entry)
      return SourceLocation();

    const unsigned FileOffset = Entry->getOffset();
    return SourceLocation::get(FileOffset);
  }

  SourceLocation getLocForEndOfFile(FileID FID) const {
    const auto Entry = getSLocEntry(FID);
    if (!Entry)
      return SourceLocation();

    const unsigned FileOffset = Entry->getOffset();
    return SourceLocation::get(FileOffset + getFileIDSize(FID));
  }

  SourceLocation getIncludeLoc(FileID FID) const {
    const auto Entry = getSLocEntry(FID);
    if (!Entry)
      return SourceLocation();

    return Entry->getFile().getIncludeLoc();
  }

  unsigned getFileIDSize(FileID FID) const;

  llvm::Optional<unsigned> getLineNumber(FileID FID, unsigned FilePos) const;
  llvm::Optional<unsigned> getSpellingLineNumber(SourceLocation Loc) const;
  llvm::Optional<unsigned> getExpansionLineNumber(SourceLocation Loc) const;
  llvm::Optional<unsigned> getPresumedLineNumber(SourceLocation Loc) const;

  llvm::Optional<unsigned> getColumnNumber(FileID FID, unsigned FilePos) const;
  llvm::Optional<unsigned> getSpellingColumnNumber(SourceLocation Loc) const;
  llvm::Optional<unsigned> getExpansionColumnNumber(SourceLocation Loc) const;
  llvm::Optional<unsigned> getPresumedColumnNumber(SourceLocation Loc) const;

  llvm::Optional<llvm::StringRef> getBufferName(SourceLocation Loc) const;

  FileID translateFile(const FileEntry *SourceFile) const;

  SourceLocation translateLineCol(FileID FID, unsigned Line,
                                  unsigned Col) const;

  PresumedLoc getPresumedLoc(SourceLocation Loc,
                             bool UseLineDirectives = true) const;

  unsigned local_sloc_entry_size() const { return LocalSLocEntryTable.size(); }

  unsigned loaded_sloc_entry_size() const {
    return LoadedSLocEntryTable.size();
  }

private:
  const SrcMgr::SLocEntry *getSLocEntry(FileID FID) const {
    if (FID.ID == 0 || FID.ID == -1) {
      return nullptr;
    }
    return getSLocEntryByID(FID.ID);
  }

  const SrcMgr::SLocEntry *getLocalSLocEntry(unsigned Index) const {
    assert(Index < LocalSLocEntryTable.size() && "Invalid index");
    return &LocalSLocEntryTable[Index];
  }

  const SrcMgr::SLocEntry *getSLocEntryByID(int ID) const {
    assert(ID != -1 && "Using FileID sentinel value");
    if (ID < 0)
      return getLoadedSLocEntryByID(ID);
    return getLocalSLocEntry(static_cast<unsigned>(ID));
  }

  const SrcMgr::SLocEntry *getLoadedSLocEntryByID(int ID) const {
    return getLoadedSLocEntry(static_cast<unsigned>(-ID - 2));
  }

  const SrcMgr::SLocEntry *getLoadedSLocEntry(unsigned Index) const {
    assert(Index < LoadedSLocEntryTable.size() && "Invalid index");
    if (SLocEntryLoaded[Index])
      return &LoadedSLocEntryTable[Index];
    return loadSLocEntry(Index);
  }

  const SrcMgr::SLocEntry *loadSLocEntry(unsigned Index) const;

  FileID getPreviousFileID(FileID FID) const;

  FileID getNextFileID(FileID FID) const;
};

} // namespace soll
