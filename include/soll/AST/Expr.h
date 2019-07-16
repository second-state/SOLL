#pragma once

#include "soll/AST/OperationKinds.h"
#include "soll/AST/Stmt.h"
#include "soll/Basic/SourceLocation.h"

namespace soll {

class Expr : public ExprStmt {
  // TODO
};

using ExprPtr = std::unique_ptr<Expr>;

class TupleExpr {
  // TODO
};

class UnaryOperator : public Expr {
  ExprPtr Val;
  UnaryOperatorKind Opc;
public:
  typedef UnaryOperatorKind Opcode;

  UnaryOperator(ExprPtr && val, Opcode opc): Val(val), Opc(opc) {}

  Opcode getOpcode() const { return Op; }
  Expr *getSubExpr() const { return Val.get(); }

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setSubExpr(ExprPtr &&E) { Val = E; }

  /// isPostfix - Return true if this is a postfix operation, like x++.
  static bool isPostfix(Opcode Op) { return Op == UO_PostInc || Op == UO_PostDec; }
  /// isPrefix - Return true if this is a prefix operation, like --x.
  static bool isPrefix(Opcode Op) { return Op == UO_PreInc || Op == UO_PreDec; }
  static bool isIncrementOp(Opcode Op) { return Op == UO_PreInc || Op == UO_PostInc; }
  static bool isDecrementOp(Opcode Op) { return Op == UO_PreDec || Op == UO_PostDec; }
  static bool isIncrementDecrementOp(Opcode Op) { return Op <= UO_PreDec; }
  static bool isArithmeticOp(Opcode Op) { return Op >= UO_Plus && Op <= UO_LNot; }

  bool isPrefix() const { return isPrefix(getOpcode()); }
  bool isPostfix() const { return isPostfix(getOpcode()); }
  bool isIncrementOp() const { return isIncrementOp(getOpcode()); }
  bool isDecrementOp() const { return isDecrementOp(getOpcode()); }
  bool isIncrementDecrementOp() const { return isIncrementDecrementOp(getOpcode()); }
  bool isArithmeticOp() const { return isArithmeticOp(getOpcode()); }
};

class BinaryOperator : public Expr {
  enum { LHS, RHS, END };
  ExprPtr SubExprs[END];
  BinaryOperatorKind Opc;
public:
  typedef BinaryOperatorKind Opcode;

  BinaryOperator(ExprPtr &&lhs, ExprPtr &&rhs, Opcode opc): Opc(opc) {
    SubExprs[LHS] = lhs;
    SubExprs[RHS] = rhs;
  }

  Opcode getOpcode() const { return Opc; }

  Expr *getLHS() const { return SubExprs[LHS].get(); }
  Expr *getRHS() const { return SubExprs[RHS].get(); }
  SourceLocation getBeginLoc() const { return getLHS()->getBeginLoc(); }
  SourceLocation getEndLoc() const { return getRHS()->getEndLoc(); }

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setLHS(ExprPtr &&E) { SubExprs[LHS] = E; }
  void setRHS(ExprPtr &&E) { SubExprs[RHS] = E; }

  static bool isMultiplicativeOp(Opcode Opc) { return Opc >= BO_Mul && Opc <= BO_Rem; }
  static bool isAdditiveOp(Opcode Opc) { return Opc == BO_Add || Opc==BO_Sub; }
  static bool isShiftOp(Opcode Opc) { return Opc == BO_Shl || Opc == BO_Shr; }
  static bool isBitwiseOp(Opcode Opc) { return Opc >= BO_And && Opc <= BO_Or; }
  static bool isRelationalOp(Opcode Opc) { return Opc >= BO_LT && Opc<=BO_GE; }
  static bool isEqualityOp(Opcode Opc) { return Opc == BO_EQ || Opc == BO_NE; }
  static bool isComparisonOp(Opcode Opc) { return Opc >= BO_Cmp && Opc<=BO_NE; }
  static bool isCommaOp(Opcode Opc) { return Opc == BO_Comma; }
  static bool isLogicalOp(Opcode Opc) { return Opc == BO_LAnd || Opc==BO_LOr; }
  static bool isAssignmentOp(Opcode Opc) { return Opc >= BO_Assign && Opc <= BO_OrAssign; }
  static bool isCompoundAssignmentOp(Opcode Opc) { return Opc > BO_Assign && Opc <= BO_OrAssign; }
  static bool isShiftAssignOp(Opcode Opc) { return Opc == BO_ShlAssign || Opc == BO_ShrAssign; }

  bool isMultiplicativeOp() const { return isMultiplicativeOp(getOpcode()); }
  bool isAdditiveOp() const { return isAdditiveOp(getOpcode()); }
  bool isShiftOp() const { return isShiftOp(getOpcode()); }
  bool isBitwiseOp() const { return isBitwiseOp(getOpcode()); }
  bool isRelationalOp() const { return isRelationalOp(getOpcode()); }
  bool isEqualityOp() const { return isEqualityOp(getOpcode()); }
  bool isComparisonOp() const { return isComparisonOp(getOpcode()); }
  bool isCommaOp() const { return isCommaOp(getOpcode()); }
  bool isLogicalOp() const { return isLogicalOp(getOpcode()); }
  bool isAssignmentOp() const { return isAssignmentOp(getOpcode()); }
  bool isCompoundAssignmentOp() const { return isCompoundAssignmentOp(getOpcode()); }
  bool isShiftAssignOp() const { return isShiftAssignOp(getOpcode()); }
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
  ExprPtr Val;
};

class ConstantExpr : public Expr {
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

} // namespace soll
