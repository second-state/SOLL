// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/SourceManager.h"

namespace soll {

static std::unique_ptr<std::vector<unsigned>>
ComputeLineNumbers(llvm::MemoryBuffer *Buffer) {
  llvm::SmallVector<unsigned, 256> LineOffsets;
  LineOffsets.push_back(0);

  auto *Buf = reinterpret_cast<const unsigned char *>(Buffer->getBufferStart());
  auto *End = reinterpret_cast<const unsigned char *>(Buffer->getBufferEnd());
  unsigned I = 0;

  while (true) {
    // Skip over the contents of the line.
    while (Buf[I] != '\n' && Buf[I] != '\r' && Buf[I] != '\0')
      ++I;

    if (Buf[I] == '\n' || Buf[I] == '\r') {
      // If this is \r\n, skip both characters.
      if (Buf[I] == '\r' && Buf[I + 1] == '\n')
        ++I;
      ++I;
      LineOffsets.push_back(I);
    } else {
      // Otherwise, this is a NUL. If end of file, exit.
      if (Buf + I == End)
        break;
      ++I;
    }
  }

  return std::make_unique<std::vector<unsigned>>(LineOffsets.begin(),
                                                 LineOffsets.end());
}

llvm::Optional<unsigned>
SrcMgr::FileInfo::getLineNumber(unsigned FilePos) const {
  if (!Buffer) {
    return llvm::None;
  }
  if (!SourceLineCache) {
    SourceLineCache = ComputeLineNumbers(Buffer.get());
  }
  auto Pos = std::lower_bound(SourceLineCache->cbegin(),
                              SourceLineCache->cend(), FilePos + 1);
  return std::distance(SourceLineCache->cbegin(), Pos);
}

llvm::Optional<unsigned>
SrcMgr::FileInfo::getColumnNumber(unsigned FilePos) const {
  if (!Buffer) {
    return llvm::None;
  }
  if (FilePos > Buffer->getBufferSize()) {
    return llvm::None;
  }
  const char *Buf = Buffer->getBufferStart();
  unsigned LineStart = FilePos;
  while (LineStart && Buf[LineStart - 1] != '\n' && Buf[LineStart - 1] != '\r')
    --LineStart;
  return FilePos - LineStart + 1;
}

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
  Diag.setSourceManager(this);
}

const SrcMgr::SLocEntry *SourceManager::loadSLocEntry(unsigned Index) const {
  assert(!SLocEntryLoaded[Index]);

  return &LoadedSLocEntryTable[Index];
}

FileID SourceManager::getFileIDSlow(unsigned SLocOffset) const {
  if (!SLocOffset)
    return FileID::get(0);

  if (SLocOffset < NextLocalOffset)
    return getFileIDLocal(SLocOffset);
  return getFileIDLoaded(SLocOffset);
}

FileID SourceManager::getFileIDLocal(unsigned SLocOffset) const {
  assert(SLocOffset < NextLocalOffset && "Bad function choice");

  const SrcMgr::SLocEntry *I;

  if (LastFileIDLookup.ID < 0 ||
      LocalSLocEntryTable[LastFileIDLookup.ID].getOffset() < SLocOffset) {
    // Neither loc prunes our search.
    I = LocalSLocEntryTable.end();
  } else {
    // Perhaps it is near the file point.
    I = LocalSLocEntryTable.begin() + LastFileIDLookup.ID;
  }

  // Find the FileID that contains this.  "I" is an iterator that points to a
  // FileID whose offset is known to be larger than SLocOffset.
  unsigned NumProbes = 0;
  while (true) {
    --I;
    if (I->getOffset() <= SLocOffset) {
      FileID Res = FileID::get(int(I - LocalSLocEntryTable.begin()));

      // Remember it.  We have good locality across FileID lookups.
      LastFileIDLookup = Res;
      return Res;
    }
    if (++NumProbes == 8)
      break;
  }

  // Convert "I" back into an index.  We know that it is an entry whose index is
  // larger than the offset we are looking for.
  unsigned GreaterIndex = I - LocalSLocEntryTable.begin();
  // LessIndex - This is the lower bound of the range that we're searching.
  // We know that the offset corresponding to the FileID is is less than
  // SLocOffset.
  unsigned LessIndex = 0;
  NumProbes = 0;
  while (true) {
    unsigned MiddleIndex = (GreaterIndex - LessIndex) / 2 + LessIndex;
    const SrcMgr::SLocEntry *SLoc = getLocalSLocEntry(MiddleIndex);
    if (!SLoc)
      return FileID::get(0);
    unsigned MidOffset = SLoc->getOffset();

    ++NumProbes;

    // If the offset of the midpoint is too large, chop the high side of the
    // range to the midpoint.
    if (MidOffset > SLocOffset) {
      GreaterIndex = MiddleIndex;
      continue;
    }

    // If the middle index contains the value, succeed and return.
    // FIXME: This could be made faster by using a function that's aware of
    // being in the local area.
    if (isOffsetInFileID(FileID::get(MiddleIndex), SLocOffset)) {
      FileID Res = FileID::get(MiddleIndex);

      // Remember it.  We have good locality across FileID lookups.
      LastFileIDLookup = Res;
      return Res;
    }

    // Otherwise, move the low-side up to the middle index.
    LessIndex = MiddleIndex;
  }
}

FileID SourceManager::getFileIDLoaded(unsigned SLocOffset) const {
  // Sanity checking, otherwise a bug may lead to hanging in release build.
  if (SLocOffset < CurrentLoadedOffset) {
    assert(0 && "Invalid SLocOffset or bad function choice");
    return FileID();
  }

  // Essentially the same as the local case, but the loaded array is sorted
  // in the other direction.

  // First do a linear scan from the last lookup position, if possible.
  unsigned I;
  int LastID = LastFileIDLookup.ID;
  if (LastID >= 0 || getLoadedSLocEntryByID(LastID)->getOffset() < SLocOffset)
    I = 0;
  else
    I = (-LastID - 2) + 1;

  unsigned NumProbes;
  for (NumProbes = 0; NumProbes < 8; ++NumProbes, ++I) {
    // Make sure the entry is loaded!
    const SrcMgr::SLocEntry *E = getLoadedSLocEntry(I);
    if (E->getOffset() <= SLocOffset) {
      FileID Res = FileID::get(-int(I) - 2);

      LastFileIDLookup = Res;
      return Res;
    }
  }

  // Linear scan failed. Do the binary search. Note the reverse sorting of the
  // table: GreaterIndex is the one where the offset is greater, which is
  // actually a lower index!
  unsigned GreaterIndex = I;
  unsigned LessIndex = LoadedSLocEntryTable.size();
  NumProbes = 0;
  while (true) {
    ++NumProbes;
    unsigned MiddleIndex = (LessIndex - GreaterIndex) / 2 + GreaterIndex;
    const SrcMgr::SLocEntry *E = getLoadedSLocEntry(MiddleIndex);
    if (E->getOffset() == 0)
      return FileID(); // invalid entry.

    ++NumProbes;

    if (E->getOffset() > SLocOffset) {
      // Sanity checking, otherwise a bug may lead to hanging in release build.
      if (GreaterIndex == MiddleIndex) {
        assert(0 && "binary search missed the entry");
        return FileID();
      }
      GreaterIndex = MiddleIndex;
      continue;
    }

    if (isOffsetInFileID(FileID::get(-int(MiddleIndex) - 2), SLocOffset)) {
      FileID Res = FileID::get(-int(MiddleIndex) - 2);
      LastFileIDLookup = Res;
      return Res;
    }

    // Sanity checking, otherwise a bug may lead to hanging in release build.
    if (LessIndex == MiddleIndex) {
      assert(0 && "binary search missed the entry");
      return FileID();
    }
    LessIndex = MiddleIndex;
  }
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
  assert(SourceFile && "Null source file!");

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
  LastFileIDLookup = FID;
  return FID;
}

unsigned SourceManager::getFileIDSize(FileID FID) const {
  const SrcMgr::SLocEntry *Entry = getSLocEntry(FID);
  if (!Entry)
    return 0;

  int ID = FID.ID;
  unsigned NextOffset;
  if ((ID > 0 && unsigned(ID + 1) == local_sloc_entry_size()))
    NextOffset = getNextLocalOffset();
  else if (ID + 1 == -1)
    NextOffset = MaxLoadedOffset;
  else
    NextOffset = getSLocEntry(FileID::get(ID + 1))->getOffset();

  return NextOffset - Entry->getOffset() - 1;
}

llvm::Optional<unsigned> SourceManager::getLineNumber(FileID FID,
                                                      unsigned FilePos) const {
  if (FID.isInvalid()) {
    return llvm::None;
  }

  const auto &Entry = getSLocEntry(FID);
  if (!Entry) {
    return llvm::None;
  }

  return Entry->getFile().getLineNumber(FilePos);
}

llvm::Optional<unsigned>
SourceManager::getColumnNumber(FileID FID, unsigned FilePos) const {

  if (FID.isInvalid()) {
    return llvm::None;
  }

  const auto &Entry = getSLocEntry(FID);
  if (!Entry) {
    return llvm::None;
  }

  return Entry->getFile().getColumnNumber(FilePos);
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

PresumedLoc SourceManager::getPresumedLoc(SourceLocation Loc) const {
  if (Loc.isInvalid()) {
    return PresumedLoc();
  }

  std::pair<FileID, unsigned> LocInfo = getDecomposedExpansionLoc(Loc);

  const SrcMgr::SLocEntry *Entry = getSLocEntry(LocInfo.first);
  if (!Entry)
    return PresumedLoc();

  const SrcMgr::FileInfo &FI = Entry->getFile();

  llvm::StringRef Filename;
  if (FI.getFileEntry())
    Filename = FI.getFileEntry()->getName();
  else
    Filename = FI.getBuffer(Diag, *this)->getBufferIdentifier();

  auto LineNo = getLineNumber(LocInfo.first, LocInfo.second);
  if (!LineNo)
    return PresumedLoc();
  auto ColNo = getColumnNumber(LocInfo.first, LocInfo.second);
  if (!ColNo)
    return PresumedLoc();

  SourceLocation IncludeLoc = FI.getIncludeLoc();

  return PresumedLoc(Filename.data(), *LineNo, *ColNo, IncludeLoc);
}

} // namespace soll
