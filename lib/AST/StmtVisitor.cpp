#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Expr.h"

using namespace soll;

template <bool Const> void StmtVisitorBase<Const>::visit(UnaryOperatorType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(BinaryOperatorType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(IdentifierType &) {
  // TODO: implement
}

template <bool Const> void StmtVisitorBase<Const>::visit(LiteralType &) {
  // TODO: implement
}

/// Explicit instantiation
template class StmtVisitorBase<true>;
template class StmtVisitorBase<false>;
