// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/ParseAST.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Lex/Lexer.h"
#include "soll/Parse/Parser.h"
#include "soll/Sema/Sema.h"
#include <memory>

namespace soll {

void ParseAST(Sema &S, ASTConsumer &C, ASTContext &Ctx, bool PrintStats) {
  auto P = std::make_unique<Parser>(S.Lex);
  auto root = P->parse();

  C.HandleSourceUnit(Ctx, *root);
}

} // namespace soll
