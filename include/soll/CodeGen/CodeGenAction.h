// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/CodeGen/BackendUtil.h"
#include "soll/Frontend/FrontendAction.h"

namespace llvm {
class LLVMContext;
class Module;
} // namespace llvm

namespace soll {

class CodeGenAction : public ASTFrontendAction {
  BackendAction Action;
  std::unique_ptr<llvm::LLVMContext> OwnedVMContext;
  llvm::LLVMContext *VMContext;

protected:
  CodeGenAction(BackendAction Action, llvm::LLVMContext *VMContext = nullptr);
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) override;
};

class EmitAssemblyAction : public CodeGenAction {
public:
  EmitAssemblyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitBCAction : public CodeGenAction {
public:
  EmitBCAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitLLVMAction : public CodeGenAction {
public:
  EmitLLVMAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitLLVMOnlyAction : public CodeGenAction {
public:
  EmitLLVMOnlyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitCodeGenOnlyAction : public CodeGenAction {
public:
  EmitCodeGenOnlyAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitObjAction : public CodeGenAction {
public:
  EmitObjAction(llvm::LLVMContext *_VMContext = nullptr);
};

class EmitWasmAction : public CodeGenAction {
public:
  EmitWasmAction(llvm::LLVMContext *_VMContext = nullptr);
};

} // namespace soll
