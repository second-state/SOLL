#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Decl.h"

namespace soll {

template <bool Const> void StmtVisitorBase<Const>::visit(BlockType &block) {
  for (auto stmt : block.getStmts())
    stmt->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(DeclStmtType &declstmt) {
}

template <bool Const> void StmtVisitorBase<Const>::visit(IfStmtType &stmt) {
  stmt.getCond()->accept(*this);
  stmt.getThen()->accept(*this);
  if (stmt.getElse())
    stmt.getElse()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(ForStmtType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(ContinueStmtType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(BreakStmtType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(ReturnStmtType &stmt) {
  stmt.getRetValue()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(UnaryOperatorType &op) {
  op.getSubExpr()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(BinaryOperatorType &op) {
  op.getLHS()->accept(*this);
  op.getRHS()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(CallExprType &expr) {
  expr.getCalleeExpr()->accept(*this);
  for (auto args : expr.getArguments())
    args->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(IdentifierType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(BooleanLiteralType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(StringLiteralType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(NumberLiteralType &) {
  // leaf, do nothing
}

} // namespace soll

/// Explicit instantiation
template class soll::StmtVisitorBase<true>;
template class soll::StmtVisitorBase<false>;
