// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/ParseAST.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Lex/Lexer.h"
#include "soll/Parse/Parser.h"
#include "soll/Sema/Sema.h"
#include <memory>

namespace soll {

std::vector<std::unique_ptr<SourceUnit>>
ParseAST(Sema &S, const InputKind &Lang, bool PrintStats) {
  auto P = std::make_unique<Parser>(S.Lex, S, S.Diags);
  switch (Lang) {
  case InputKind::Sol: {
    return P->parse();
  }
  case InputKind::Yul: {
    std::vector<std::unique_ptr<SourceUnit>> Ret;
    Ret.emplace_back(P->parseYul());
    return Ret;
  }
  default:
    assert(false && "unsupported language");
    __builtin_unreachable();
  }
}

} // namespace soll
