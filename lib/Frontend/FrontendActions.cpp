// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/ASTConsumers.h"
#include "soll/Frontend/CompilerInstance.h"

namespace soll {

std::unique_ptr<ASTConsumer>
ASTPrintAction::CreateASTConsumer(CompilerInstance &CI,
                                  llvm::StringRef InFile) {
  return CreateASTPrinter();
}

std::unique_ptr<ASTConsumer>
EmitFuncSigAction::CreateASTConsumer(CompilerInstance &CI,
                                     llvm::StringRef InFile) {
  return CreateFuncSigPrinter();
}

} // namespace soll
