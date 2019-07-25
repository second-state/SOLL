#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Stmt.h"

using namespace soll;

void ASTPrinter::visit(SourceUnitType &decl) {
  os() << indent() << "SourceUnit" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ContractDeclType &decl) {
  os() << indent() << "ContractDecl" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(FunctionDeclType &decl) {
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

void ASTPrinter::visit(VarDeclType &decl) {
  os() << indent() << "VarDecl" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(BlockType &block) {
  os() << indent() << "Block" << std::endl;
  ConstStmtVisitor::visit(block);
  unindent();
}

void ASTPrinter::visit(IfStmtType &stmt) {
  os() << indent() << "IfStmt" << std::endl;
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ForStmtType &stmt) {
  os() << indent() << "ForStmt" << std::endl;
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ContinueStmtType &stmt) {
  os() << indent() << "ContinueStmt" << std::endl;
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(BreakStmtType &stmt) {
  os() << indent() << "BreakStmt" << std::endl;
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ReturnStmtType &stmt) {
  os() << indent() << "ReturnStmt" << std::endl;
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ExprStmtType &epstmt) {
  os() << indent() << "Expression Stmt" << std::endl;
  ConstStmtVisitor::visit(epstmt);
  unindent();
}

void ASTPrinter::visit(DeclStmtType &decstmt) {
  os() << indent() << "DeclStmt" << std::endl;
  ConstStmtVisitor::visit(decstmt);
  unindent();
}

void ASTPrinter::visit(UnaryOperatorType &op) {
  os() << indent() << "UnaryOperator" << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(BinaryOperatorType &op) {
  os() << indent() << "BinaryOperator" << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(CallExprType &id) {
  os() << indent() << "CallExpr" << std::endl;
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(IdentifierType &id) {
  os() << indent() << "Identifier" << std::endl;
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(BooleanLiteralType &literal) {
  os() << indent() << "BooleanLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(StringLiteralType &literal) {
  os() << indent() << "StringLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(NumberLiteralType &literal) {
  os() << indent() << "NumberLiteral" << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}
