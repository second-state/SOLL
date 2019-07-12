#pragma once

#include "Stmt.h"
#include "OperationKinds.h"

namespace soll {

class Expr : public ExprStmt {
    // TODO
};

class TupleExpr {
    // TODO
};

class UnaryOperator : public Expr {
  StmtPtr Val;
public:
  typedef UnaryOperator Opcode;
static bool isPostfix(Opcode Op) {
  return Op == UO_PostInc || Op == UO_PostDec;
}
};

class BinaryOperator : public Expr {
  enum { LHS, RHS, END };
  StmtPtr SubExprs[END];
public:
  typedef BinaryOperatorKind Opcode;
};

class CallExpr : public Expr {
  // TODO
};

class ImplicitCastExpr : public Expr {
  // TODO
};

class ExplicitCastExpr : public Expr {
  // TODO
};

class NewExpr : public Expr {
  // TODO
};

class MemberExpr : public Expr {
  // TODO
};

class IndexAccess : public Expr {
  // TODO
};

class ParenExpr : public Expr {
  Stmt *Val;
};

class ConstantExpr : public Expr{
  // TODO
};

class Identifier : public Expr {
  // TODO
};

class ElementaryTypeNameExpr : public Expr {
  // TODO
};

class Literal : public Expr {
  // TODO
};

}
