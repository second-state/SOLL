#include "soll/AST/Stmt.h"
#include "soll/AST/Decl.h"

namespace soll {

DeclStmt::DeclStmt(std::vector<DeclPtr> &&Decls, ExprPtr &&Value) :
   Decls(std::move(Decls)), Value(std::move(Value)) {}

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

}
