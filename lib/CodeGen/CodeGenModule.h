#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace soll {

class CodeGenModule {
  CodeGenModule(const CodeGenModule &) = delete;
  void operator=(const CodeGenModule &) = delete;

public:
  CodeGenModule(llvm::Module &module);

private:
  llvm::Module &module;
};

} // namespace soll
