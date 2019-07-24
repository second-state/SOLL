#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"

using namespace soll;

void ASTPrinter::visit(const SourceUnitType &decl) {
  os() << indent() << "SourceUnit" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(const ContractDeclType &decl) {
  os() << indent() << "ContractDecl" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(const FunctionDecl &decl) {
  os() << indent() << "FunctionDecl" << std::endl;
  ConstDeclVisitor::visit(decl);
  decl.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(ParamListType &param) {
  os() << indent() << "ParamList" << std::endl;
  ConstDeclVisitor::visit(param);
  unindent();
}

void ASTPrinter::visit(const VarDecl &decl) {
  os() << indent() << "VarDecl" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(const Block &block) {
  os() << indent() << "Block" << std::endl;
  ConstStmtVisitor::visit(block);
  unindent();
}

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

void ASTPrinter::visit(const BooleanLiteral &literal) {
  os() << indent() << "BooleanLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(const StringLiteral &literal) {
  os() << indent() << "StringLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(const NumberLiteral &literal) {
  os() << indent() << "NumberLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}
