// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

class ASTConsumer;
class ASTContext;
class Sema;

void ParseAST(Sema &S, ASTConsumer &C, ASTContext &Ctx, bool PrintStats = false);

} // namespace soll
