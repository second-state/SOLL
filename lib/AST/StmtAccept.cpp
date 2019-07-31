#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

namespace soll {

void Block::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Block::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

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
void WhileStmt::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

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

} // namespace soll
