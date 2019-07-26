#include "soll/AST/Decl.h"

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
    Decls.push_back(Decl.get());
  return Decls;
}

std::vector<const Decl *> ContractDecl::getSubNodes() const {
  std::vector<const Decl *> Decls;
  for (auto &Decl : this->SubNodes)
    Decls.push_back(Decl.get());
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

} // namespace soll
