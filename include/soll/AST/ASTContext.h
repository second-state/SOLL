// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/Decl.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <unordered_map>
namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {};

} // namespace soll
