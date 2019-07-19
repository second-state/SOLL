#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Expr.h"

using namespace soll;

template <bool Const>
void StmtVisitorBase<Const>::visit(UnaryOperatorType &op) {
  op.getSubExpr()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(BinaryOperatorType &op) {
  op.getLHS()->accept(*this);
  op.getRHS()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(IdentifierType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(LiteralType &) {
  // leaf, do nothing
}

/// Explicit instantiation
template class StmtVisitorBase<true>;
template class StmtVisitorBase<false>;
