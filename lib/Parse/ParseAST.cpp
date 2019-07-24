#include "soll/Parse/ParseAST.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTPrinter.h"
#include "soll/Lex/Lexer.h"
#include "soll/Parse/Parser.h"
#include "soll/Sema/Sema.h"
#include <iostream>
#include <memory>
namespace soll {

void ParseAST(Sema &S, bool PrintStats) {
  auto P = std::make_unique<Parser>(S.Lex);
  auto &&root = P->parse();

  ASTPrinter p(std::cout);
  root->accept(p);
}

} // namespace soll
