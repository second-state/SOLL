#pragma once
#include <llvm/ADT/IntrusiveRefCntPtr.h>

namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
};

} // namespace soll
