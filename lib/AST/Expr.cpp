// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Expr.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Stmt.h"

namespace soll {

bool IndexAccess::isStateVariable() const {
  if (auto ID = dynamic_cast<const Identifier *>(getBase())) {
    if (auto *D = dynamic_cast<const VarDecl *>(ID->getCorrespondDecl())) {
      return D->isStateVariable();
    }
  } else if (auto IA = dynamic_cast<const IndexAccess *>(getBase())) {
    return IA->isStateVariable();
  }
  return false;
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
    : Expr(ValueKind::VK_LValue, nullptr), Name(Name), D(D) {
  if (auto VD = dynamic_cast<const VarDecl *>(D)) {
    Ty = VD->GetType();
  } else if (auto FD = dynamic_cast<const FunctionDecl *>(D)) {
    Ty = FD->getType();
  } else if (dynamic_cast<const EventDecl *>(D)) {
    Ty = nullptr;
  } else {
    assert(false && "unknown decl");
    __builtin_unreachable();
  }
}

Identifier::Identifier(const std::string &Name, SpecialIdentifier D, TypePtr Ty)
    : Expr(ValueKind::VK_LValue, std::move(Ty)), Name(Name), D(D) {}

} // namespace soll
