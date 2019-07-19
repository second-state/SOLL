#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Expr.h"

using namespace soll;

void ASTPrinter::visit(const UnaryOperator &op) {
  os() << indent() << "UnaryOperator" << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(const BinaryOperator &op) {
  os() << indent() << "BinaryOperator" << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(const Identifier &id) {
  os() << indent() << "Identifier" << std::endl;
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(const Literal &literal) {
  os() << indent() << "Literal" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}
