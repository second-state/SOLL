// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <string>
#include <vector>

namespace soll {

class ASTConsumer;
class ASTContext;
class Sema;

void ParseAST(Sema &S, ASTConsumer &C, ASTContext &Ctx,
              const std::vector<std::string> &LibrariesAddressMaps,
              bool PrintStats = false);

} // namespace soll
