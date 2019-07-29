#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"

namespace {

std::string ToString(soll::UnaryOperatorKind op) {
  switch (op) {
  case soll::UO_Plus:
    return "+";
  case soll::UO_Minus:
    return "-";
  case soll::UO_Not:
    return "~";
  case soll::UO_LNot:
    return "!";
  default:
    return "";
  }
}

std::string ToString(soll::BinaryOperatorKind op) {
  switch (op) {
  case soll::BO_Exp:
    return "**";
  case soll::BO_Mul:
    return "*";
  case soll::BO_Div:
    return "/";
  case soll::BO_Rem:
    return "%";
  case soll::BO_Add:
    return "+";
  case soll::BO_Sub:
    return "-";
  case soll::BO_Shl:
    return "<<";
  case soll::BO_Shr:
    return ">>";
  case soll::BO_And:
    return "&";
  case soll::BO_Xor:
    return "^";
  case soll::BO_Or:
    return "|";
  case soll::BO_LT:
    return "<";
  case soll::BO_GT:
    return ">";
  case soll::BO_LE:
    return "<=";
  case soll::BO_GE:
    return ">=";
  case soll::BO_EQ:
    return "==";
  case soll::BO_NE:
    return "!=";
  case soll::BO_LAnd:
    return "&&";
  case soll::BO_LOr:
    return "||";
  case soll::BO_Assign:
    return "=";
  case soll::BO_MulAssign:
    return "*=";
  case soll::BO_DivAssign:
    return "/=";
  case soll::BO_RemAssign:
    return "%=";
  case soll::BO_AddAssign:
    return "+=";
  case soll::BO_SubAssign:
    return "-=";
  case soll::BO_ShlAssign:
    return "<<=";
  case soll::BO_ShrAssign:
    return ">>=";
  case soll::BO_AndAssign:
    return "&=";
  case soll::BO_XorAssign:
    return "^=";
  case soll::BO_OrAssign:
    return "|=";
  case soll::BO_Comma:
    return ",";
  default:
    return "";
  }
}

} // namespace

namespace soll {

void ASTPrinter::visit(SourceUnitType &decl) {
  os() << indent() << "SourceUnit" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ContractDeclType &decl) {
  os() << indent() << "ContractDecl \"" << decl.getName() << "\"" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(FunctionDeclType &decl) {
  os() << indent() << "FunctionDecl \"" << decl.getName() << "\"" << std::endl;
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
  os() << indent() << "VarDecl \"" << decl.getName() << "\"" << std::endl;
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ModifierInvocationType &modifier) {
  os() << indent() << "ModifierInvocation \"" << modifier.getName() << "\""
       << std::endl;
  ConstDeclVisitor::visit(modifier);
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

void ASTPrinter::visit(DeclStmtType &decstmt) {
  os() << indent() << "DeclStmt" << std::endl;
  ConstStmtVisitor::visit(decstmt);
  for (auto vdecl : decstmt.getVarDecls())
    vdecl->accept(*this);
  if(decstmt.getValue() != nullptr)
    decstmt.getValue()->accept(*this);
  unindent();
}

void ASTPrinter::visit(UnaryOperatorType &op) {
  os() << indent() << "UnaryOperator \"" << ToString(op.getOpcode()) << "\""
       << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(BinaryOperatorType &op) {
  os() << indent() << "BinaryOperator \"" << ToString(op.getOpcode()) << "\""
       << std::endl;
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(CallExprType &id) {
  os() << indent() << "CallExpr" << std::endl;
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(IdentifierType &id) {
  os() << indent() << "Identifier \"" << id.getName() << "\"" << std::endl;
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(BooleanLiteralType &literal) {
  os() << indent() << "BooleanLiteral \"" << literal.getValue() << "\""
       << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(StringLiteralType &literal) {
  os() << indent() << "StringLiteral \"" << literal.getValue() << "\""
       << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(NumberLiteralType &literal) {
  os() << indent() << "NumberLiteral \"" << literal.getValue() << "\""
       << std::endl;
  ConstStmtVisitor::visit(literal);
  unindent();
}

} // namespace soll
