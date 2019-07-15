#include "soll/Basic/SourceManager.h"

namespace soll {

llvm::MemoryBuffer *SrcMgr::FileInfo::getBuffer(DiagnosticsEngine &Diag,
                                                const SourceManager &SM,
                                                SourceLocation Loc) const {
  if (Buffer) {
    return Buffer.get();
  }

  if (!File) {
    return nullptr;
  }

  auto BufferOrError = SM.getFileManager().getBufferForFile(File);

  if (!BufferOrError) {
    Diag.Report(Loc, diag::err_cannot_open_file)
        << File->getName() << BufferOrError.getError().message();
    return nullptr;
  }

  Buffer = std::move(*BufferOrError);

  if (Buffer->getBufferSize() != static_cast<size_t>(File->getSize())) {
    Diag.Report(Loc, diag::err_file_modified) << File->getName();
    return nullptr;
  }

  // If the buffer is valid, check to see if it has a UTF Byte Order Mark
  // (BOM).  We only support UTF-8 with and without a BOM right now.  See
  // http://en.wikipedia.org/wiki/Byte_order_mark for more information.
  llvm::StringRef BufStr = Buffer->getBuffer();
  const char *InvalidBOM =
      llvm::StringSwitch<const char *>(BufStr)
          .StartsWith("\xFE\xFF", "UTF-16 (BE)")
          .StartsWith("\xFF\xFE", "UTF-16 (LE)")
          .StartsWith(llvm::StringLiteral::withInnerNUL("\x00\x00\xFE\xFF"),
                      "UTF-32 (BE)")
          .StartsWith(llvm::StringLiteral::withInnerNUL("\xFF\xFE\x00\x00"),
                      "UTF-32 (LE)")
          .StartsWith("\x2B\x2F\x76", "UTF-7")
          .StartsWith("\xF7\x64\x4C", "UTF-1")
          .StartsWith("\xDD\x73\x66\x73", "UTF-EBCDIC")
          .StartsWith("\x0E\xFE\xFF", "SDSU")
          .StartsWith("\xFB\xEE\x28", "BOCU-1")
          .StartsWith("\x84\x31\x95\x33", "GB-18030")
          .Default(nullptr);

  if (InvalidBOM) {
    Diag.Report(Loc, diag::err_unsupported_bom)
        << InvalidBOM << File->getName();
    return nullptr;
  }

  return Buffer.get();
}

SourceManager::SourceManager(DiagnosticsEngine &Diag, FileManager &FileMgr)
    : Diag(Diag), FileMgr(FileMgr) {
  LocalSLocEntryTable.push_back(SrcMgr::SLocEntry::get(
      0, SrcMgr::FileInfo::get(SourceLocation(), nullptr)));
}

const SrcMgr::SLocEntry *SourceManager::loadSLocEntry(unsigned Index) const {
  assert(!SLocEntryLoaded[Index]);

  return &LoadedSLocEntryTable[Index];
}

FileID SourceManager::getPreviousFileID(FileID FID) const {
  if (FID.isInvalid())
    return FileID();

  int ID = FID.ID;
  if (ID == -1)
    return FileID();

  if (ID > 0) {
    if (ID - 1 == 0)
      return FileID();
  } else if (unsigned(-(ID - 1) - 2) >= LoadedSLocEntryTable.size()) {
    return FileID();
  }

  return FileID::get(ID - 1);
}

FileID SourceManager::getNextFileID(FileID FID) const {
  if (FID.isInvalid())
    return FileID();

  int ID = FID.ID;
  if (ID > 0) {
    if (unsigned(ID + 1) >= local_sloc_entry_size())
      return FileID();
  } else if (ID + 1 >= -1) {
    return FileID();
  }

  return FileID::get(ID + 1);
}

static llvm::Optional<llvm::sys::fs::UniqueID>
getActualFileUID(const FileEntry *File) {
  if (!File)
    return llvm::None;

  llvm::sys::fs::UniqueID ID;
  if (llvm::sys::fs::getUniqueID(File->getName(), ID))
    return llvm::None;

  return ID;
}

FileID SourceManager::createFileID(const FileEntry *SourceFile,
                                   SourceLocation IncludePos, int LoadedID,
                                   unsigned LoadedOffset) {
  if (LoadedID < 0) {
    assert(LoadedID != -1 && "Loading sentinel FileID");
    unsigned Index = unsigned(-LoadedID) - 2;
    assert(Index < LoadedSLocEntryTable.size() && "FileID out of range");
    assert(!SLocEntryLoaded[Index] && "FileID already loaded");
    LoadedSLocEntryTable[Index] = SrcMgr::SLocEntry::get(
        LoadedOffset, SrcMgr::FileInfo::get(IncludePos, SourceFile));
    SLocEntryLoaded[Index] = true;
    return FileID::get(LoadedID);
  }
  LocalSLocEntryTable.push_back(SrcMgr::SLocEntry::get(
      NextLocalOffset, SrcMgr::FileInfo::get(IncludePos, SourceFile)));
  unsigned FileSize = SourceFile->getSize();
  assert(NextLocalOffset + FileSize + 1 > NextLocalOffset &&
         NextLocalOffset + FileSize + 1 <= CurrentLoadedOffset &&
         "Ran out of source locations!");
  NextLocalOffset += FileSize + 1;

  FileID FID = FileID::get(LocalSLocEntryTable.size() - 1);
  return LastFileIDLookup = FID;
}

FileID SourceManager::translateFile(const FileEntry *SourceFile) const {
  assert(SourceFile && "Null source file!");

  FileID FirstFID;

  llvm::Optional<llvm::sys::fs::UniqueID> SourceFileUID;
  llvm::Optional<llvm::StringRef> SourceFileName;
  if (MainFileID.isValid()) {
    const SrcMgr::SLocEntry *MainSLoc = getSLocEntry(MainFileID);
    if (!MainSLoc)
      return FileID();

    const FileEntry *MainFile = MainSLoc->getFile().getFileEntry();
    if (!MainFile) {
    } else if (MainFile == SourceFile) {
      FirstFID = MainFileID;
    } else {
      SourceFileName = llvm::sys::path::filename(SourceFile->getName());
      if (*SourceFileName == llvm::sys::path::filename(MainFile->getName())) {
        SourceFileUID = getActualFileUID(SourceFile);
        if (SourceFileUID) {
          if (llvm::Optional<llvm::sys::fs::UniqueID> MainFileUID =
                  getActualFileUID(MainFile)) {
            if (*SourceFileUID == *MainFileUID) {
              FirstFID = MainFileID;
              SourceFile = MainFile;
            }
          }
        }
      }
    }
  }

  if (FirstFID.isInvalid()) {
    for (unsigned I = 0, N = local_sloc_entry_size(); I != N; ++I) {
      const SrcMgr::SLocEntry *SLoc = getLocalSLocEntry(I);
      if (!SLoc)
        return FileID();

      if (SLoc->getFile().getFileEntry() == SourceFile) {
        FirstFID = FileID::get(I);
        break;
      }
    }

    if (FirstFID.isInvalid()) {
      for (unsigned I = 0, N = loaded_sloc_entry_size(); I != N; ++I) {
        const SrcMgr::SLocEntry *SLoc = getLoadedSLocEntry(I);
        if (SLoc && SLoc->getFile().getFileEntry() == SourceFile) {
          FirstFID = FileID::get(-int(I) - 2);
          break;
        }
      }
    }
  }

  if (FirstFID.isInvalid() &&
      (SourceFileName ||
       (SourceFileName = llvm::sys::path::filename(SourceFile->getName()))) &&
      (SourceFileUID || (SourceFileUID = getActualFileUID(SourceFile)))) {
    for (unsigned I = 0, N = local_sloc_entry_size(); I != N; ++I) {
      FileID IFileID;
      IFileID.ID = I;
      const SrcMgr::SLocEntry *SLoc = getSLocEntry(IFileID);
      if (!SLoc)
        return FileID();

      const FileEntry *Entry = SLoc->getFile().getFileEntry();
      if (Entry &&
          *SourceFileName == llvm::sys::path::filename(Entry->getName())) {
        if (llvm::Optional<llvm::sys::fs::UniqueID> EntryUID =
                getActualFileUID(Entry)) {
          if (*SourceFileUID == *EntryUID) {
            FirstFID = FileID::get(I);
            SourceFile = Entry;
            break;
          }
        }
      }
    }
  }

  return FirstFID;
}

} // namespace soll
