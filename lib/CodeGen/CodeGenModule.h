// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/TargetOptions.h"
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
  const TargetOptions &TargetOpts;
  llvm::LLVMContext &VMContext;

  CodeGenModule(const CodeGenModule &) = delete;
  void operator=(const CodeGenModule &) = delete;

public:
  CodeGenModule(ASTContext &C, llvm::Module &module,
                DiagnosticsEngine &Diags,
                const TargetOptions &TargetOpts);
};

} // namespace CodeGen
} // namespace soll
