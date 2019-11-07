// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/ExprYul.h"
#include "soll/AST/StmtVisitor.h"
#include "soll/AST/StmtYul.h"

namespace soll {

void Block::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Block::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void EmitStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void EmitStmt::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void IfStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void IfStmt::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void ForStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ForStmt::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void ContinueStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ContinueStmt::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void BreakStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void BreakStmt::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void WhileStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void WhileStmt::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void ReturnStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ReturnStmt::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void DeclStmt::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void DeclStmt::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void UnaryOperator::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void UnaryOperator::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void BinaryOperator::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void BinaryOperator::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void CallExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void CallExpr::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void ImplicitCastExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ImplicitCastExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void ExplicitCastExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ExplicitCastExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void ParenExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ParenExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void MemberExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void MemberExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void IndexAccess::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void IndexAccess::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void Identifier::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Identifier::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void BooleanLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void BooleanLiteral::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void StringLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void StringLiteral::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void NumberLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void NumberLiteral::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulIdentifierList::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void YulIdentifierList::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulAssignment::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulAssignment::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulForStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulForStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulCaseStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulCaseStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulDefaultStmt::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void YulDefaultStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulSwitchStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulSwitchStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulIdentifier::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulIdentifier::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void YulLiteral::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void YulLiteral::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

} // namespace soll
