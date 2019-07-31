// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Frontend/FrontendAction.h"

namespace soll {

class ASTPrintAction : public ASTFrontendAction {
protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) override;
};

} // namespace soll
