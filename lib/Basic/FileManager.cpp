// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/FileManager.h"

namespace soll {

namespace {
static FileEntry *NON_EXISTENT_FILE =
    reinterpret_cast<FileEntry *>(static_cast<intptr_t>(-1));
static DirectoryEntry *NON_EXISTENT_DIR =
    reinterpret_cast<DirectoryEntry *>(static_cast<intptr_t>(-1));
} // namespace

llvm::ErrorOr<llvm::vfs::Status>
FileManager::getStatValue(llvm::StringRef Path, bool isFile,
                          std::unique_ptr<llvm::vfs::File> *F) {
  llvm::SmallString<128> FilePath(Path);
  if (!FileSystemOpts.WorkingDir.empty()) {
    FixupRelativePath(FilePath);
    Path = FilePath;
  }
  if (!isFile || !F) {
    return FS->status(Path);
  }

  auto OwnedFile = FS->openFileForRead(Path);
  if (!OwnedFile) {
    return OwnedFile.getError();
  }
  llvm::ErrorOr<llvm::vfs::Status> Status = (*OwnedFile)->status();
  if (Status && F) {
    *F = std::move(*OwnedFile);
  }
  return Status;
}

FileManager::FileManager(const FileSystemOptions &FSO,
                         llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS)
    : FS(std::move(FS)), FileSystemOpts(FSO), NextFileUID(0) {
  if (!this->FS)
    this->FS = llvm::vfs::getRealFileSystem();
}

bool FileManager::FixupRelativePath(llvm::SmallVectorImpl<char> &path) const {
  llvm::StringRef pathRef(path.data(), path.size());

  if (FileSystemOpts.WorkingDir.empty() ||
      llvm::sys::path::is_absolute(pathRef))
    return false;

  llvm::SmallString<128> NewPath(FileSystemOpts.WorkingDir);
  llvm::sys::path::append(NewPath, pathRef);
  path = NewPath;
  return true;
}

static const DirectoryEntry *getDirectoryFromFile(FileManager &FileMgr,
                                                  llvm::StringRef Filename,
                                                  bool CacheFailure) {
  if (Filename.empty())
    return nullptr;

  if (llvm::sys::path::is_separator(Filename[Filename.size() - 1]))
    return nullptr;

  llvm::StringRef DirName = llvm::sys::path::parent_path(Filename);
  // Use the current directory if file has no path component.
  if (DirName.empty())
    DirName = ".";

  return FileMgr.getDirectory(DirName, CacheFailure);
}

const DirectoryEntry *FileManager::getDirectory(llvm::StringRef DirName,
                                                bool CacheFailure) {
  if (DirName.size() > 1 && DirName != llvm::sys::path::root_path(DirName) &&
      llvm::sys::path::is_separator(DirName.back()))
    DirName = DirName.substr(0, DirName.size() - 1);
#ifdef _WIN32
  std::string DirNameStr;
  if (DirName.size() > 1 && DirName.back() == ':' &&
      DirName.equals_lower(llvm::sys::path::root_name(DirName))) {
    DirNameStr = DirName.str() + '.';
    DirName = DirNameStr;
  }
#endif

  ++NumDirLookups;
  auto &NamedDirEnt =
      *SeenDirEntries.insert(std::make_pair(DirName, nullptr)).first;

  // See if there was already an entry in the map.  Note that the map
  // contains both virtual and real directories.
  if (NamedDirEnt.second)
    return NamedDirEnt.second == NON_EXISTENT_DIR ? nullptr
                                                  : NamedDirEnt.second;

  ++NumDirCacheMisses;

  // By default, initialize it to invalid.
  NamedDirEnt.second = NON_EXISTENT_DIR;

  // Get the null-terminated directory name as stored as the key of the
  // SeenDirEntries map.
  llvm::StringRef InterndDirName = NamedDirEnt.first();

  llvm::ErrorOr<llvm::vfs::Status> Status =
      getStatValue(InterndDirName, false, nullptr);
  if (!Status) {
    if (!CacheFailure)
      SeenDirEntries.erase(DirName);
    return nullptr;
  }

  DirectoryEntry &UDE = UniqueRealDirs[Status->getUniqueID()];

  NamedDirEnt.second = &UDE;
  if (UDE.getName().empty()) {
    // We don't have this directory yet, add it.  We use the string
    // key from the SeenDirEntries map as the string.
    UDE.Name = InterndDirName;
  }

  return &UDE;
}

const FileEntry *FileManager::getFile(llvm::StringRef Filename, bool openFile,
                                      bool CacheFailure) {
  ++NumFileLookups;

  auto &NamedFileEnt =
      *SeenFileEntries.insert(std::make_pair(Filename, nullptr)).first;

  if (NamedFileEnt.second)
    return NamedFileEnt.second == NON_EXISTENT_FILE ? nullptr
                                                    : NamedFileEnt.second;

  ++NumFileCacheMisses;

  NamedFileEnt.second = NON_EXISTENT_FILE;

  llvm::StringRef InterndFileName = NamedFileEnt.first();

  const DirectoryEntry *DirInfo =
      getDirectoryFromFile(*this, Filename, CacheFailure);
  if (DirInfo == nullptr) {
    if (!CacheFailure)
      SeenFileEntries.erase(Filename);

    return nullptr;
  }

  std::unique_ptr<llvm::vfs::File> F;
  llvm::ErrorOr<llvm::vfs::Status> Status =
      getStatValue(InterndFileName, true, openFile ? &F : nullptr);
  if (!Status) {
    if (!CacheFailure)
      SeenFileEntries.erase(Filename);

    return nullptr;
  }

  assert((openFile || !F) && "undesired open file");

  FileEntry &UFE = UniqueRealFiles[Status->getUniqueID()];

  NamedFileEnt.second = &UFE;

  // If the name returned by getStatValue is different than Filename, re-intern
  // the name.
  if (Status->getName() != Filename) {
    auto &NamedFileEnt =
        *SeenFileEntries.insert(std::make_pair(Status->getName(), nullptr))
             .first;
    if (!NamedFileEnt.second)
      NamedFileEnt.second = &UFE;
    else
      assert(NamedFileEnt.second == &UFE &&
             "filename from getStatValue() refers to wrong file");
    InterndFileName = NamedFileEnt.first().data();
  }

  if (UFE.isValid()) { // Already have an entry with this inode, return it.

    // FIXME: this hack ensures that if we look up a file by a virtual path in
    // the VFS that the getDir() will have the virtual path, even if we found
    // the file by a 'real' path first. This is required in order to find a
    // module's structure when its headers/module map are mapped in the VFS.
    // We should remove this as soon as we can properly support a file having
    // multiple names.
    if (DirInfo != UFE.Dir && Status->IsVFSMapped)
      UFE.Dir = DirInfo;

    // Always update the name to use the last name by which a file was accessed.
    // FIXME: Neither this nor always using the first name is correct; we want
    // to switch towards a design where we return a FileName object that
    // encapsulates both the name by which the file was accessed and the
    // corresponding FileEntry.
    UFE.Name = InterndFileName;

    return &UFE;
  }

  // Otherwise, we don't have this file yet, add it.
  UFE.Name = InterndFileName;
  UFE.Size = Status->getSize();
  UFE.ModTime = llvm::sys::toTimeT(Status->getLastModificationTime());
  UFE.Dir = DirInfo;
  UFE.UID = NextFileUID++;
  UFE.UniqueID = Status->getUniqueID();
  UFE.IsNamedPipe = Status->getType() == llvm::sys::fs::file_type::fifo_file;
  UFE.File = std::move(F);
  UFE.IsValid = true;

  if (UFE.File) {
    if (auto PathName = UFE.File->getName())
      fillRealPathName(&UFE, *PathName);
  }
  return &UFE;
}

bool FileManager::makeAbsolutePath(llvm::SmallVectorImpl<char> &Path) const {
  bool Changed = FixupRelativePath(Path);

  if (!llvm::sys::path::is_absolute(
          llvm::StringRef(Path.data(), Path.size()))) {
    FS->makeAbsolute(Path);
    Changed = true;
  }

  return Changed;
}

void FileManager::fillRealPathName(FileEntry *UFE, llvm::StringRef FileName) {
  llvm::SmallString<128> AbsPath(FileName);
  // This is not the same as `VFS::getRealPath()`, which resolves symlinks
  // but can be very expensive on real file systems.
  // FIXME: the semantic of RealPathName is unclear, and the name might be
  // misleading. We need to clean up the interface here.
  makeAbsolutePath(AbsPath);
  llvm::sys::path::remove_dots(AbsPath, /*remove_dot_dot=*/true);
  UFE->RealPathName = AbsPath.str();
}

llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
FileManager::getBufferForFile(const FileEntry *Entry, bool isVolatile,
                              bool ShouldCloseOpenFile) {
  uint64_t FileSize = Entry->getSize();
  if (isVolatile)
    FileSize = -1;

  llvm::StringRef Filename = Entry->getName();
  if (Entry->File) {
    auto Result =
        Entry->File->getBuffer(Filename, FileSize,
                               /*RequiresNullTerminator=*/true, isVolatile);
    // FIXME: we need a set of APIs that can make guarantees about whether a
    // FileEntry is open or not.
    if (ShouldCloseOpenFile)
      Entry->closeFile();
    return Result;
  }

  // Otherwise, open the file.

  if (FileSystemOpts.WorkingDir.empty())
    return FS->getBufferForFile(Filename, FileSize,
                                /*RequiresNullTerminator=*/true, isVolatile);

  llvm::SmallString<128> FilePath(Entry->getName());
  FixupRelativePath(FilePath);
  return FS->getBufferForFile(FilePath, FileSize,
                              /*RequiresNullTerminator=*/true, isVolatile);
}

llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
FileManager::getBufferForFile(llvm::StringRef Filename, bool isVolatile) {
  if (FileSystemOpts.WorkingDir.empty())
    return FS->getBufferForFile(Filename, -1, true, isVolatile);

  llvm::SmallString<128> FilePath(Filename);
  FixupRelativePath(FilePath);
  return FS->getBufferForFile(FilePath.c_str(), -1, true, isVolatile);
}

} // namespace soll
