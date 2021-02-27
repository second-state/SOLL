// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/CodeGenOptions.h"
#include "soll/Basic/TargetOptions.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace soll {

class DiagnosticsEngine;

class CodeGenerator : public ASTConsumer {
protected:
  std::vector<std::pair<std::string, const Decl *>> Entry;
  std::vector<std::tuple<std::string, std::string, const Decl *>> NestedEntries;

public:
  llvm::Module *getModule();
  const std::vector<std::pair<std::string, const Decl *>> &getEntry() const {
    return Entry;
  }
  const std::vector<std::tuple<std::string, std::string, const Decl *>> &
  getNestedEntries() const {
    return NestedEntries;
  }
  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override;
};

CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
                                 llvm::StringRef ModuleName,
                                 llvm::LLVMContext &C,
                                 const CodeGenOptions &CodeGenOpts,
                                 const TargetOptions &TargetOpts);

} // namespace soll
