// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"
#include <llvm/IR/Intrinsics.h>

namespace soll {
namespace CodeGen {

CodeGenModule::CodeGenModule(ASTContext &C, llvm::Module &M,
                             DiagnosticsEngine &Diags,
                             const TargetOptions &TargetOpts)
    : Context(C), TheModule(M), Diags(Diags), TargetOpts(TargetOpts),
      VMContext(M.getContext()) {}

llvm::Function *CodeGenModule::getIntrinsic(unsigned IID,
                                            llvm::ArrayRef<llvm::Type *> Typs) {
  return llvm::Intrinsic::getDeclaration(&TheModule, (llvm::Intrinsic::ID)IID,
                                         Typs);
}

} // namespace CodeGen
} // namespace soll
