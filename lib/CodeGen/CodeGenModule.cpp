// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"

namespace soll {
namespace CodeGen {

CodeGenModule::CodeGenModule(ASTContext &C, llvm::Module &M,
                             DiagnosticsEngine &Diags,
                             const TargetOptions &TargetOpts)
    : Context(C), TheModule(M), Diags(Diags), TargetOpts(TargetOpts),
      VMContext(M.getContext()) {}

} // namespace CodeGen
} // namespace soll
