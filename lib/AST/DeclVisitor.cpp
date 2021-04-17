// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Decl.h"
#include "soll/AST/DeclAsm.h"
#include "soll/AST/DeclYul.h"

namespace soll {

template <bool Const> void DeclVisitorBase<Const>::visit(SourceUnitType &SU) {
  for (auto Node : SU.getNodes())
    Node->accept(*this);
}

template <bool Const>
void DeclVisitorBase<Const>::visit(PragmaDirectiveType &) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(UsingForType &) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(ContractDeclType &CD) {
  if (CD.getConstructor() != nullptr) {
    CD.getConstructor()->accept(*this);
  }
  if (CD.getFallback() != nullptr) {
    CD.getFallback()->accept(*this);
  }
  for (auto F : CD.getUsingForNodes()) {
    F->accept(*this);
  }
  for (auto F : CD.getInheritNodes()) {
    F->accept(*this);
  }
  for (auto F : CD.getSubNodes()) {
    F->accept(*this);
  }
}

template <bool Const>
void DeclVisitorBase<Const>::visit(FunctionDeclType &decl) {
  if (decl.getParams() != nullptr)
    decl.getParams()->accept(*this);
  if (decl.getReturnParams() != nullptr)
    decl.getReturnParams()->accept(*this);
}

template <bool Const> void DeclVisitorBase<Const>::visit(EventDeclType &decl) {
  if (decl.getParams() != nullptr)
    decl.getParams()->accept(*this);
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

template <bool Const> void DeclVisitorBase<Const>::visit(StructDeclType &) {
  // leaf, do nothing
}

template <bool Const>
void DeclVisitorBase<Const>::visit(ModifierInvocationType &) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(YulCodeType &C) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(YulDataType &D) {
  // leaf, do nothing
}

template <bool Const> void DeclVisitorBase<Const>::visit(YulObjectType &O) {
  O.getCode()->accept(*this);
  for (auto InnerO : O.getObjectList())
    InnerO->accept(*this);
  for (auto InnerD : O.getDataList())
    InnerD->accept(*this);
}

template <bool Const>
void DeclVisitorBase<Const>::visit(AsmFunctionDeclType &D) {
  if (D.getParams() != nullptr)
    D.getParams()->accept(*this);
  if (D.getReturnParams() != nullptr)
    D.getReturnParams()->accept(*this);
}

template <bool Const> void DeclVisitorBase<Const>::visit(AsmVarDeclType &) {
  // leaf, do nothing
}

} // namespace soll

/// Explicit instantiation
template class soll::DeclVisitorBase<true>;
template class soll::DeclVisitorBase<false>;
