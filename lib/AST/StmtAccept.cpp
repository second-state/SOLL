#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

using namespace soll;

void Block::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Block::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void UnaryOperator::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void UnaryOperator::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void BinaryOperator::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void BinaryOperator::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}
void Identifier::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Identifier::accept(ConstStmtVisitor &visitor) const {
  visitor.visit(*this);
}

void BooleanLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void BooleanLiteral::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void StringLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void StringLiteral::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }

void NumberLiteral::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void NumberLiteral::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }
