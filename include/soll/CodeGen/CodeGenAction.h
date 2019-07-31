// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Frontend/FrontendAction.h"

namespace llvm {
class LLVMContext;
class Module;
} // namespace llvm

namespace soll {

class CodeGenAction : public ASTFrontendAction {
  llvm::LLVMContext *VMContext;

protected:
  CodeGenAction(llvm::LLVMContext *VMContext);
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) override;
};

class EmitLLVMAction : public CodeGenAction {
public:
  EmitLLVMAction(llvm::LLVMContext *VMContext);
};

} // namespace soll
