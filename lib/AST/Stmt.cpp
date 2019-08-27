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

BinaryOperator::BinaryOperator(ExprPtr &&lhs, ExprPtr &&rhs, Opcode opc,
                               TypePtr Ty)
    : Expr(ValueKind::VK_RValue, Ty), Opc(opc) {
  SubExprs[LHS] = std::move(lhs);
  SubExprs[RHS] = std::move(rhs);
  if (this->isAssignmentOp())
    setValueKind(ValueKind::VK_LValue);
}

///
/// Identifier
///
Identifier::Identifier(const std::string &Name, Decl *D)
    : Expr(ValueKind::VK_LValue), name(Name), D(D) {
  if (auto VD = dynamic_cast<const VarDecl *>(D))
    Ty = VD->GetType();
  else if (auto FD = dynamic_cast<const FunctionDecl *>(D))
    Ty = FD->getType();
}

} // namespace soll
