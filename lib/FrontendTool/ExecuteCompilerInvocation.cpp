// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/FrontendActions.h"
#include "soll/FrontendTool/Utils.h"

namespace soll {

std::unique_ptr<FrontendAction> CreateFrontendAction(CompilerInstance &CI) {
  llvm::StringRef Action("unknown");

  switch (CI.getFrontendOpts().ProgramAction) {
  case ASTDump:
    return std::make_unique<ASTPrintAction>();
  case EmitAssembly:
    return std::make_unique<EmitAssemblyAction>();
  case EmitBC:
    return std::make_unique<EmitBCAction>();
  case EmitLLVM:
    return std::make_unique<EmitLLVMAction>();
  case EmitLLVMOnly:
    return std::make_unique<EmitLLVMOnlyAction>();
  case EmitCodeGenOnly:
    return std::make_unique<EmitCodeGenOnlyAction>();
  case EmitObj:
    return std::make_unique<EmitObjAction>();
  case EmitFuncSig:
    return std::make_unique<EmitFuncSigAction>();
  case EmitABI:
    return std::make_unique<EmitABIAction>();
  case InitOnly:
    return std::make_unique<InitOnlyAction>();
  case ParseSyntaxOnly:
    return std::make_unique<SyntaxOnlyAction>();
  }

  assert(false && "Invalid program action!");
  __builtin_unreachable();
}

bool ExecuteCompilerInvocation(CompilerInstance *Soll) {
  std::unique_ptr<FrontendAction> Act(CreateFrontendAction(*Soll));
  if (!Act)
    return false;
  bool Success = Soll->ExecuteAction(*Act);
  return Success;
}

} // namespace soll
