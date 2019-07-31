// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/ASTConsumer.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>

namespace soll {

class DiagnosticsEngine;

class CodeGenerator : public ASTConsumer {

public:
  llvm::Module *GetModule();
  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override;
};

CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
                                 llvm::StringRef ModuleName,
                                 llvm::LLVMContext &C);

} // namespace soll
