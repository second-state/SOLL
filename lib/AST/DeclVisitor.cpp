#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Decl.h"

using namespace soll;

template <bool Const> void DeclVisitorBase<Const>::visit(SourceUnitType &SU) {
  for (auto Node : SU.getNodes())
    Node->accept(*this);
}

template <bool Const>
void DeclVisitorBase<Const>::visit(PragmaDirectiveType &) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(ContractDeclType &CD) {
  for (auto F : CD.getFuncs())
    F->accept(*this);
}

template <bool Const> void DeclVisitorBase<Const>::visit(FunctionDeclType &) {
  // leaf of Decl, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(VarDeclType &) {
  // leaf, do nothing
}

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
