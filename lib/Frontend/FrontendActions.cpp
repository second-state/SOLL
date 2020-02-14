// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/ASTConsumers.h"
#include "soll/Frontend/CompilerInstance.h"

namespace soll {

std::unique_ptr<ASTConsumer>
InitOnlyAction::CreateASTConsumer(CompilerInstance &CI,
                                  llvm::StringRef InFile) {
  return std::make_unique<ASTConsumer>();
}

void InitOnlyAction::ExecuteAction() {}

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

std::unique_ptr<ASTConsumer>
EmitABIAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) {
  return CreateABIPrinter();
}

SyntaxOnlyAction::~SyntaxOnlyAction() {}

std::unique_ptr<ASTConsumer>
SyntaxOnlyAction::CreateASTConsumer(CompilerInstance &CI,
                                    llvm::StringRef InFile) {
  return llvm::make_unique<ASTConsumer>();
}

} // namespace soll
