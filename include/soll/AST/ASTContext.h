// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/Type.h"
#include "soll/Frontend/FrontendOptions.h"
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/Value.h>

namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
  InputKind Language;
  const llvm::StringMap<llvm::APInt> LibrariesAddressMap;
  llvm::StringMap<llvm::APInt> ImmutableAddressMap;

public:
  const TypePtr IntegerTypeU256Ptr;
  const TypePtr IntegerTypeI256Ptr;
  const TypePtr ContractTypePtr;
  const TypePtr FixedBytesTypeB32Ptr;
  const TypePtr FixedBytesTypeB20Ptr;
  const TypePtr FixedBytesTypeB4Ptr;
  const TypePtr FixedBytesTypeB1Ptr;
  const TypePtr BooleanTypePtr;
  const TypePtr StringTypePtr;
  const TypePtr BytesTypePtr;
  const TypePtr AddressTypeNonPayablePtr;
  const TypePtr AddressTypePayablePtr;
  const TypePtr NullPtr;
  ASTContext(InputKind Language,
             const std::vector<std::string> &LibrariesAddressInfo);

  const InputKind &getLang() const { return Language; }
  const llvm::StringMap<llvm::APInt> &getLibrariesAddressMap() const {
    return LibrariesAddressMap;
  }

  llvm::StringMap<llvm::APInt> &getImmutableAddressMap() {
    return ImmutableAddressMap;
  }
};

} // namespace soll
