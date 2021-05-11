// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Frontend/FrontendOptions.h"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/Value.h>

namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
  InputKind Language;
  const llvm::StringMap<llvm::APInt> LibrariesAddressMap;

public:
  ASTContext(InputKind Language,
             const std::vector<std::string> &LibrariesAddressInfo);

  const InputKind &getLang() const { return Language; }
  const llvm::StringMap<llvm::APInt> &getLibrariesAddressMap() const {
    return LibrariesAddressMap;
  }
};

} // namespace soll
