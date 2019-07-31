// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"

namespace soll {
namespace CodeGen {

CodeGenModule::CodeGenModule(ASTContext &C, llvm::Module &M,
                             DiagnosticsEngine &Diags)
    : Context(C), TheModule(M), Diags(Diags), VMContext(M.getContext()) {}

} // namespace CodeGen
} // namespace soll
