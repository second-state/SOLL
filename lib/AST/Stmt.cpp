// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Stmt.h"
#include "soll/AST/Decl.h"

namespace soll {

///
/// DeclStmt
///
std::vector<const VarDecl *> DeclStmt::getVarDecls() const {
  std::vector<const VarDecl *> VarDecls;
  for (auto &D : this->VarDecls)
    VarDecls.emplace_back(D.get());
  return VarDecls;
}

std::vector<VarDecl *> DeclStmt::getVarDecls() {
  std::vector<VarDecl *> VarDecls;
  for (auto &D : this->VarDecls)
    VarDecls.emplace_back(D.get());
  return VarDecls;
}

///
/// Block
///
void Block::setStmts(std::vector<StmtPtr> &&Stmts) {
  this->Stmts.clear();
  for (auto &&S : Stmts)
    this->Stmts.emplace_back(std::move(S));
}

std::vector<const Stmt *> Block::getStmts() const {
  std::vector<const Stmt *> Stmts;
  for (auto &S : this->Stmts)
    Stmts.emplace_back(S.get());
  return Stmts;
}

std::vector<Stmt *> Block::getStmts() {
  std::vector<Stmt *> Stmts;
  for (auto &S : this->Stmts)
    Stmts.emplace_back(S.get());
  return Stmts;
}

} // namespace soll
