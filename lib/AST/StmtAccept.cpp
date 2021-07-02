// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/ExprAsm.h"
#include "soll/AST/StmtAsm.h"
#include "soll/AST/StmtVisitor.h"

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

void TupleExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void TupleExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void TypesTupleExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void TypesTupleExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void DirectValueExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void DirectValueExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void ReturnTupleExpr::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void ReturnTupleExpr::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

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

void AsmForStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void AsmForStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmCaseStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void AsmCaseStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmDefaultStmt::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void AsmDefaultStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmSwitchStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void AsmSwitchStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmAssignmentStmt::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void AsmAssignmentStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmFunctionDeclStmt::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void AsmFunctionDeclStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmLeaveStmt::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void AsmLeaveStmt::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmIdentifierList::accept(soll::StmtVisitor &visitor) {
  visitor.visit(*this);
}
void AsmIdentifierList::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmIdentifier::accept(soll::StmtVisitor &visitor) { visitor.visit(*this); }
void AsmIdentifier::accept(soll::ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

} // namespace soll
