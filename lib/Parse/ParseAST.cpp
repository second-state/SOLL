// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/ParseAST.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Lex/Lexer.h"
#include "soll/Parse/Parser.h"
#include "soll/Sema/Sema.h"
#include <memory>

namespace soll {

void ParseAST(Sema &S, ASTConsumer &C, ASTContext &Ctx,
              const std::vector<std::string> &LibrariesAddressMaps,
              bool PrintStats) {

  auto P = std::make_unique<Parser>(S.Lex, S, S.Diags, LibrariesAddressMaps);
  std::unique_ptr<SourceUnit> root;

  switch (Ctx.getLang()) {
  case InputKind::Sol:
    root = P->parse();
    break;
  case InputKind::Yul:
    root = P->parseYul();
    break;
  default:
    assert(false && "unsupported language");
    __builtin_unreachable();
  }

  C.HandleSourceUnit(Ctx, *root);
}

} // namespace soll
