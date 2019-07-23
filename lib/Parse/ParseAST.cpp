#include "soll/Parse/ParseAST.h"
#include "soll/Parse/Parser.h"
#include "soll/Sema/Sema.h"
#include "soll/Lex/Lexer.h"
#include "soll/AST/AST.h"
#include <memory>

namespace soll {

void ParseAST(Sema &S, bool PrintStats) {
  auto P = std::make_unique<Parser>(S.Lex);
  P->parse();
}

} // namespace soll
