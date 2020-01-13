// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "../utils/SHA-3/Keccak.h"
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

std::vector<VarDecl *> ContractDecl::getVars() {
  std::vector<VarDecl *> Nodes;
  for (auto &Node : SubNodes) {
    if (auto VD = dynamic_cast<VarDecl *>(Node.get()))
      Nodes.emplace_back(VD);
  }
  return Nodes;
}

std::vector<const VarDecl *> ContractDecl::getVars() const {
  std::vector<const VarDecl *> Nodes;
  for (auto &Node : SubNodes) {
    if (auto VD = dynamic_cast<const VarDecl *>(Node.get()))
      Nodes.emplace_back(VD);
  }
  return Nodes;
}

std::vector<FunctionDecl *> ContractDecl::getFuncs() {
  std::vector<FunctionDecl *> Nodes;
  for (auto &Node : SubNodes) {
    if (auto FD = dynamic_cast<FunctionDecl *>(Node.get())) {
      Nodes.emplace_back(FD);
    }
  }
  return Nodes;
}

std::vector<const FunctionDecl *> ContractDecl::getFuncs() const {
  std::vector<const FunctionDecl *> Nodes;
  for (auto &Node : this->SubNodes) {
    if (auto FD = dynamic_cast<const FunctionDecl *>(Node.get())) {
      Nodes.emplace_back(FD);
    }
  }
  return Nodes;
}

std::vector<EventDecl *> ContractDecl::getEvents() {
  std::vector<EventDecl *> Nodes;
  for (auto &Node : SubNodes) {
    if (auto ED = dynamic_cast<EventDecl *>(Node.get())) {
      Nodes.emplace_back(ED);
    }
  }
  return Nodes;
}

std::vector<const EventDecl *> ContractDecl::getEvents() const {
  std::vector<const EventDecl *> Nodes;
  for (auto &Node : this->SubNodes) {
    if (auto ED = dynamic_cast<const EventDecl *>(Node.get())) {
      Nodes.emplace_back(ED);
    }
  }
  return Nodes;
}

FunctionDecl *ContractDecl::getConstructor() { return Constructor.get(); }

const FunctionDecl *ContractDecl::getConstructor() const {
  return Constructor.get();
}

FunctionDecl *ContractDecl::getFallback() { return Fallback.get(); }

const FunctionDecl *ContractDecl::getFallback() const { return Fallback.get(); }

///
/// CallableVarDecl
///
std::vector<unsigned char> CallableVarDecl::getSignatureHash() const {
  Keccak h(256);
  h.addData(getName().bytes_begin(), 0, getName().size());
  h.addData('(');
  bool first = true;
  for (const VarDeclBase *var : getParams()->getParams()) {
    if (!first)
      h.addData(',');
    first = false;
    assert(var->GetType() && "unsupported type!");
    const std::string &name = var->GetType()->getName();
    h.addData(reinterpret_cast<const uint8_t *>(name.data()), 0, name.size());
  }
  h.addData(')');
  return h.digest();
}

std::uint32_t CallableVarDecl::getSignatureHashUInt32() const {
  const std::vector<unsigned char> &op = getSignatureHash();
  return op[0] | (op[1] << 8u) | (op[2] << 16u) | (op[3] << 24u);
}

///
/// ParamList
///
std::vector<const VarDeclBase *> ParamList::getParams() const {
  std::vector<const VarDeclBase *> Params;
  for (auto &P : this->Params)
    Params.emplace_back(P.get());
  return Params;
}

std::vector<VarDeclBase *> ParamList::getParams() {
  std::vector<VarDeclBase *> Params;
  for (auto &P : this->Params)
    Params.emplace_back(P.get());
  return Params;
}

unsigned ParamList::getABIStaticSize() const {
  unsigned Result = 0;
  for (const auto &VD : Params) {
    Result += VD->GetType()->getABIStaticSize();
  }
  return Result;
}

FunctionDecl::FunctionDecl(
    SourceRange L, llvm::StringRef Name, Visibility V, StateMutability SM,
    bool IsConstructor, bool IsFallback, std::unique_ptr<ParamList> &&Params,
    std::vector<std::unique_ptr<ModifierInvocation>> &&Modifiers,
    std::unique_ptr<ParamList> &&ReturnParams, std::unique_ptr<Block> &&Body)
    : CallableVarDecl(L, Name, V, std::move(Params), std::move(ReturnParams)),
      SM(SM), IsConstructor(IsConstructor), IsFallback(IsFallback),
      FunctionModifiers(std::move(Modifiers)), Body(std::move(Body)) {
  std::vector<TypePtr> PTys;
  std::vector<TypePtr> RTys;
  for (auto VD : this->getParams()->getParams())
    PTys.push_back(VD->GetType());
  for (auto VD : this->getReturnParams()->getParams())
    RTys.push_back(VD->GetType());
  FuncTy = std::make_shared<FunctionType>(std::move(PTys), std::move(RTys));
}

EventDecl::EventDecl(SourceRange L, llvm::StringRef Name,
                     std::unique_ptr<ParamList> &&Params, bool IsAnonymous)
    : CallableVarDecl(L, Name, Decl::Visibility::Default, std::move(Params)),
      IsAnonymous(IsAnonymous) {
  std::vector<TypePtr> PTys;
  std::vector<TypePtr> RTys;
  for (auto VD : this->getParams()->getParams())
    PTys.push_back(VD->GetType());
  FuncTy = std::make_shared<FunctionType>(std::move(PTys), std::move(RTys));
}

} // namespace soll
