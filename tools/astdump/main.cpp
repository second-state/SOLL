#include "soll/AST/Expr.h"
#include "soll/AST/ASTPrinter.h"
#include <iostream>

using namespace soll;

int main(int argc, const char **argv) {
  BinaryOperator op(
      std::make_unique<Identifier>("c"),
      std::make_unique<BinaryOperator>(
        std::make_unique<Identifier>("a"),
        std::make_unique<Identifier>("b"),
        BinaryOperatorKind::BO_Add
      ),
      BinaryOperatorKind::BO_Assign
  );

  ASTPrinter p(std::cout);
  op.accept(p);

  return EXIT_SUCCESS;
}
