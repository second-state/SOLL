#include <cassert>
#include <memory>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/TextDiagnostic.h"

namespace soll {

CompilerInstance::CompilerInstance()
    : m_VirtualFileSystem(llvm::vfs::getRealFileSystem())
    , m_Invocation(std::make_unique<CompilerInvocation>()) {
}

bool CompilerInstance::Execute() {
    return m_Invocation->Execute(*this);
}

CompilerInvocation& CompilerInstance::GetInvocation() {
    assert(m_Invocation.get() != nullptr);
    return *m_Invocation;
}

bool CompilerInstance::Execute(const std::string& filename) {
    static llvm::LLVMContext Context;
    static llvm::IRBuilder<> Build(Context);

    std::unique_ptr<llvm::Module> Module = std::make_unique<llvm::Module>(filename, Context);

    auto Input = m_VirtualFileSystem->getBufferForFile(filename);
    if (std::error_code ec = Input.getError()) {
        return false;
    }

    Module->print(llvm::errs(), nullptr);

    return true;
}

} // namespace soll
