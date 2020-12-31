// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/AST.h"
#include <vector>

namespace soll {

class ASTConsumer;
class ASTContext;
class Sema;

std::vector<std::unique_ptr<SourceUnit>>
ParseAST(Sema &S, const InputKind &Lang, bool PrintStats = false);

} // namespace soll
