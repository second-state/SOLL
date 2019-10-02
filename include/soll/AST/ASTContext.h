// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/Value.h>
#include <unordered_map>
namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
};

} // namespace soll
