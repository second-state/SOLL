#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Decl.h"

namespace soll {

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

template <bool Const>
void DeclVisitorBase<Const>::visit(CallableVarDeclType &) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(VarDeclType &) {
  // leaf, do nothing
}

template <bool Const>
void DeclVisitorBase<Const>::visit(ModifierInvocationType &) {
  // leaf, do nothing
}

} // namespace soll

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
