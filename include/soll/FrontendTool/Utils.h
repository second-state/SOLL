// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <memory>

namespace soll {

class CompilerInstance;
class FrontendAction;

std::unique_ptr<FrontendAction> CreateFrontendAction(CompilerInstance &CI);
bool ExecuteCompilerInvocation(CompilerInstance *Soll);

} // namespace soll
