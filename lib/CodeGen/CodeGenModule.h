#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace soll {
class ASTContext;
class DiagnosticsEngine;

namespace CodeGen {

class CodeGenModule {
  ASTContext &Context;
  llvm::Module &TheModule;
  DiagnosticsEngine &Diags;
  llvm::LLVMContext &VMContext;

  CodeGenModule(const CodeGenModule &) = delete;
  void operator=(const CodeGenModule &) = delete;

public:
  CodeGenModule(ASTContext &C, llvm::Module &module, DiagnosticsEngine &Diags);
};

} // namespace CodeGen
} // namespace soll
