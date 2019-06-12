#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/Support/VirtualFileSystem.h>
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Frontend/DiagnosticRenderer.h"

namespace soll {

class CompilerInvocation;

class CompilerInstance {
    CompilerInstance(const CompilerInstance&) = delete;
    CompilerInstance& operator=(const CompilerInstance&) = delete;
public:
    explicit CompilerInstance();
    CompilerInvocation& GetInvocation();
    bool Execute();
    bool Execute(const std::string& filename);
private:
    llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> m_VirtualFileSystem;
    std::unique_ptr<CompilerInvocation> m_Invocation;
};

} // namespace soll
