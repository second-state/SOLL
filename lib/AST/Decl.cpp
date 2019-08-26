// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "soll/AST/Type.h"

namespace soll {

///
/// Source Unit
///
void SourceUnit::setNodes(std::vector<DeclPtr> &&Nodes) {
  for (auto &Node : Nodes)
    this->Nodes.emplace_back(std::move(Node));
}

std::vector<Decl *> SourceUnit::getNodes() {
  std::vector<Decl *> Nodes;
  for (auto &Node : this->Nodes)
    Nodes.emplace_back(Node.get());
  return Nodes;
}

std::vector<const Decl *> SourceUnit::getNodes() const {
  std::vector<const Decl *> Nodes;
  for (auto &Node : this->Nodes)
    Nodes.emplace_back(Node.get());
  return Nodes;
}

///
/// ContractDecl
///
std::vector<Decl *> ContractDecl::getSubNodes() {
  std::vector<Decl *> Decls;
  for (auto &Decl : this->SubNodes)
    Decls.emplace_back(Decl.get());
  return Decls;
}

std::vector<const Decl *> ContractDecl::getSubNodes() const {
  std::vector<const Decl *> Decls;
  for (auto &Decl : this->SubNodes)
    Decls.emplace_back(Decl.get());
  return Decls;
}

///
/// ParamList
///
std::vector<const VarDecl *> ParamList::getParams() const {
  std::vector<const VarDecl *> Params;
  for (auto &P : this->Params)
    Params.emplace_back(P.get());
  return Params;
}

std::vector<VarDecl *> ParamList::getParams() {
  std::vector<VarDecl *> Params;
  for (auto &P : this->Params)
    Params.emplace_back(P.get());
  return Params;
}

FunctionDecl::FunctionDecl(
    llvm::StringRef name, Visibility visibility, StateMutability sm,
    bool isConstructor, std::unique_ptr<ParamList> &&Params,
    std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
    std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body)
    : CallableVarDecl(name, visibility, std::move(Params),
                      std::move(returnParams)),
      SM(sm), IsConstructor(isConstructor),
      FunctionModifiers(std::move(modifiers)), Body(std::move(body)),
      Implemented(body != nullptr) {
  std::vector<TypePtr> PTys;
  std::vector<TypePtr> RTys;
  for (auto VD : this->getParams()->getParams())
    PTys.push_back(VD->GetType());
  for (auto VD : this->getReturnParams()->getParams())
    RTys.push_back(VD->GetType());
  FuncTy = std::make_shared<FunctionType>(std::move(PTys), std::move(RTys));
}

} // namespace soll
