// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/TargetOptions.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <string>
#include <utility>
#include <vector>

namespace soll {

class DiagnosticsEngine;

class CodeGenerator : public ASTConsumer {
protected:
  std::string Entry;
  std::vector<std::pair<std::string, std::string>> NestedEntries;

public:
  llvm::Module *getModule();
  const std::string &getEntry() const { return Entry; }
  const std::vector<std::pair<std::string, std::string>> &getNestedEntries() const {
    return NestedEntries;
  }
  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override;
};

CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
                                 llvm::StringRef ModuleName,
                                 llvm::LLVMContext &C,
                                 const TargetOptions &TargetOpts);

} // namespace soll
