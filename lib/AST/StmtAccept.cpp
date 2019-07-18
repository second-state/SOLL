#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

using namespace soll;

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

void Literal::accept(StmtVisitor &visitor) { visitor.visit(*this); }
void Literal::accept(ConstStmtVisitor &visitor) const { visitor.visit(*this); }
