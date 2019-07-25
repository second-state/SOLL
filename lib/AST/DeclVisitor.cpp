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
  for (auto F : CD.getSubNodes()) {
    F->accept(*this);
  }
}

template <bool Const>
void DeclVisitorBase<Const>::visit(FunctionDeclType &decl) {
  decl.getParams()->accept(*this);
  decl.getReturnParams()->accept(*this);
}

template <bool Const> void DeclVisitorBase<Const>::visit(ParamListType &decl) {
  for (auto param : decl.getParams())
    param->accept(*this);
}

template <bool Const> void DeclVisitorBase<Const>::visit(VarDeclType &) {
  // leaf, do nothing
}

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
