#include "soll/AST/Stmt.h"
#include "soll/AST/Decl.h"

namespace soll {

///
/// DeclStmt
///
std::vector<const Decl*> DeclStmt::getDecls() const{
  std::vector<const Decl*> Decls;
  for (auto &D : this->Decls)
    Decls.emplace_back(D.get());
  return Decls;
}

std::vector<Decl*> DeclStmt::getDecls() {
  std::vector<Decl*> Decls;
  for (auto &D : this->Decls)
    Decls.emplace_back(D.get());
  return Decls;
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

///
/// CallExpr
///
std::vector<Expr *> CallExpr::getArguments() {
  std::vector<Expr *> Args;
  for (auto &Arg : Arguments)
    Args.emplace_back(Arg.get());
  return Args;
}

std::vector<const Expr *> CallExpr::getArguments() const {
  std::vector<const Expr *> Args;
  for (auto &Arg : Arguments)
    Args.emplace_back(Arg.get());
  return Args;
}
}
