// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Frontend/FrontendOptions.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/Value.h>
#include <unordered_map>

namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
  InputKind Language;

public:
  ASTContext(InputKind Language) : Language(Language) {}

  const InputKind &getLang() const { return Language; }
};

} // namespace soll
