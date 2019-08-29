// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/Decl.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <unordered_map>
namespace soll {

class StorageAllocator {
  // Counter to store which position the next storage item should be stored in
  int StoragePosCounter;
  // Storage Item name => Position in Storage
  std::unordered_map<std::string, int> StorageItemPosTable;

public:
  StorageAllocator() : StoragePosCounter(0) {}
  // return storage position of given identifier name, allocate if not found
  unsigned findStoragePosition(const std::string &S, unsigned Len = 1) {
    if (StorageItemPosTable.count(S)) {
      return StorageItemPosTable[S];
    } else {
      int res = StoragePosCounter;
      StorageItemPosTable[S] = StoragePosCounter;
      StoragePosCounter += Len;
      return res;
    }
  }
};

class ASTContext : public llvm::RefCountedBase<ASTContext> {
  StorageAllocator SA;

public:
  unsigned findStoragePosition(const std::string &S, unsigned Len = 1) {
    return SA.findStoragePosition(S, Len);
  }
};

} // namespace soll
