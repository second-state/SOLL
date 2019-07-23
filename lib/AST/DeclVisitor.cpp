#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Decl.h"

using namespace soll;

template <bool Const> void DeclVisitorBase<Const>::visit(FunctionDeclType &) {
  // TODO: implement
}

template <bool Const> void DeclVisitorBase<Const>::visit(VarDeclType &) {
  // TODO: implement
}

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
