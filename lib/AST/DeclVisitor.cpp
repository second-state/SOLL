#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Decl.h"

using namespace soll;

template <bool Const> void DeclVisitorBase<Const>::visit(FunctionDeclType &) {
  // leaf of Decl, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(VarDeclType &) {
  // leaf, do nothing
}

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
