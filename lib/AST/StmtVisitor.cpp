#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Expr.h"

using namespace soll;

template <bool Const> void StmtVisitorBase<Const>::visit(BlockType &block) {
  for (auto stmt : block.getStmts())
    stmt->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(IfStmtType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(ForStmtType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(ContinueStmtType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(BreakStmtType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(ReturnStmtType &) {
  // TODO: implement
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

/// Explicit instantiation
template class soll::StmtVisitorBase<true>;
template class soll::StmtVisitorBase<false>;
