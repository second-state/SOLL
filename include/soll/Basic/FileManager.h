#pragma once
#include "soll/Basic/FileSystemOptions.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <map>

namespace soll {

class DirectoryEntry {
  friend class FileManager;

  llvm::StringRef Name;

public:
  llvm::StringRef getName() const { return Name; }
};

class FileEntry {
  friend class FileManager;

  llvm::StringRef Name;
  std::string RealPathName;
  off_t Size;
  time_t ModTime;
  const DirectoryEntry *Dir;
  unsigned UID;
  llvm::sys::fs::UniqueID UniqueID;
  bool IsNamedPipe;
  bool IsValid;

  mutable std::unique_ptr<llvm::vfs::File> File;

public:
  FileEntry() : UniqueID(0, 0), IsNamedPipe(false), IsValid(false) {}

  FileEntry(const FileEntry &) = delete;
  FileEntry &operator=(const FileEntry &) = delete;

  llvm::StringRef getName() const { return Name; }
  llvm::StringRef tryGetRealPathName() const { return RealPathName; }
  bool isValid() const { return IsValid; }
  off_t getSize() const { return Size; }
  unsigned getUID() const { return UID; }
  const llvm::sys::fs::UniqueID &getUniqueID() const { return UniqueID; }
  time_t getModificationTime() const { return ModTime; }

  const DirectoryEntry *getDir() const { return Dir; }

  bool operator<(const FileEntry &RHS) const { return UniqueID < RHS.UniqueID; }

  bool isNamedPipe() const { return IsNamedPipe; }

  void closeFile() const { File.reset(); }

  bool isOpenForTests() const { return File != nullptr; }
};

class FileManager : public llvm::RefCountedBase<FileManager> {
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS;
  FileSystemOptions FileSystemOpts;

  std::map<llvm::sys::fs::UniqueID, DirectoryEntry> UniqueRealDirs;
  std::map<llvm::sys::fs::UniqueID, FileEntry> UniqueRealFiles;

  llvm::StringMap<DirectoryEntry*, llvm::BumpPtrAllocator> SeenDirEntries;
  llvm::StringMap<FileEntry*, llvm::BumpPtrAllocator> SeenFileEntries;

  unsigned NextFileUID;
  unsigned NumDirLookups = 0, NumFileLookups = 0;
  unsigned NumDirCacheMisses = 0, NumFileCacheMisses = 0;

  llvm::ErrorOr<llvm::vfs::Status>
  getStatValue(llvm::StringRef Path, bool isFile,
               std::unique_ptr<llvm::vfs::File> *F);
  bool makeAbsolutePath(llvm::SmallVectorImpl<char> &Path) const;
  void fillRealPathName(FileEntry *UFE, llvm::StringRef FileName);

public:
  FileManager(const FileSystemOptions &FileSystemOpts,
              llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS = nullptr);

  const DirectoryEntry *getDirectory(llvm::StringRef DirName,
                                     bool CacheFailure = true);

  const FileEntry *getFile(llvm::StringRef Filename, bool OpenFile = false,
                           bool CacheFailure = true);

  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> getVirtualFileSystem() const {
    return FS;
  }

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
  getBufferForFile(const FileEntry *Entry, bool isVolatile = false,
                   bool ShouldCloseOpenFile = true);
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>>
  getBufferForFile(llvm::StringRef Filename, bool isVolatile = false);

  bool FixupRelativePath(llvm::SmallVectorImpl<char> &path) const;
};

} // namespace soll
