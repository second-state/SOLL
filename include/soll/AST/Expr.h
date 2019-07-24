#pragma once

#include "soll/AST/OperationKinds.h"
#include "soll/AST/Stmt.h"

#include <optional>
#include <string>

namespace soll {

class Expr : public ExprStmt {};

using ExprPtr = std::unique_ptr<Expr>;

class TupleExpr {
  // TODO
};

class UnaryOperator : public Expr {
  ExprPtr Val;
  UnaryOperatorKind Opc;

public:
  typedef UnaryOperatorKind Opcode;

  UnaryOperator(ExprPtr &&val, Opcode opc) : Val(std::move(val)), Opc(opc) {}

  Opcode getOpcode() const { return Opc; }
  Expr *getSubExpr() const { return Val.get(); }

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setSubExpr(ExprPtr &&E) { Val = std::move(E); }

  /// isPostfix - Return true if this is a postfix operation, like x++.
  static bool isPostfix(Opcode Op) {
    return Op == UO_PostInc || Op == UO_PostDec;
  }
  /// isPrefix - Return true if this is a prefix operation, like --x.
  static bool isPrefix(Opcode Op) { return Op == UO_PreInc || Op == UO_PreDec; }
  static bool isIncrementOp(Opcode Op) {
    return Op == UO_PreInc || Op == UO_PostInc;
  }
  static bool isDecrementOp(Opcode Op) {
    return Op == UO_PreDec || Op == UO_PostDec;
  }
  static bool isIncrementDecrementOp(Opcode Op) { return Op <= UO_PreDec; }
  static bool isArithmeticOp(Opcode Op) {
    return Op >= UO_Plus && Op <= UO_LNot;
  }

  bool isPrefix() const { return isPrefix(getOpcode()); }
  bool isPostfix() const { return isPostfix(getOpcode()); }
  bool isIncrementOp() const { return isIncrementOp(getOpcode()); }
  bool isDecrementOp() const { return isDecrementOp(getOpcode()); }
  bool isIncrementDecrementOp() const {
    return isIncrementDecrementOp(getOpcode());
  }
  bool isArithmeticOp() const { return isArithmeticOp(getOpcode()); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BinaryOperator : public Expr {
  enum { LHS, RHS, END };
  ExprPtr SubExprs[END];
  BinaryOperatorKind Opc;

public:
  typedef BinaryOperatorKind Opcode;

  BinaryOperator(ExprPtr &&lhs, ExprPtr &&rhs, Opcode opc) : Opc(opc) {
    SubExprs[LHS] = std::move(lhs);
    SubExprs[RHS] = std::move(rhs);
  }

  Opcode getOpcode() const { return Opc; }

  Expr *getLHS() const { return SubExprs[LHS].get(); }
  Expr *getRHS() const { return SubExprs[RHS].get(); }

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setLHS(ExprPtr &&E) { SubExprs[LHS] = std::move(E); }
  void setRHS(ExprPtr &&E) { SubExprs[RHS] = std::move(E); }

  static bool isMultiplicativeOp(Opcode Opc) {
    return Opc >= BO_Mul && Opc <= BO_Rem;
  }
  static bool isAdditiveOp(Opcode Opc) {
    return Opc == BO_Add || Opc == BO_Sub;
  }
  static bool isShiftOp(Opcode Opc) { return Opc == BO_Shl || Opc == BO_Shr; }
  static bool isBitwiseOp(Opcode Opc) { return Opc >= BO_And && Opc <= BO_Or; }
  static bool isRelationalOp(Opcode Opc) {
    return Opc >= BO_LT && Opc <= BO_GE;
  }
  static bool isEqualityOp(Opcode Opc) { return Opc == BO_EQ || Opc == BO_NE; }
  static bool isComparisonOp(Opcode Opc) {
    return Opc >= BO_LT && Opc <= BO_NE;
  }
  static bool isCommaOp(Opcode Opc) { return Opc == BO_Comma; }
  static bool isLogicalOp(Opcode Opc) {
    return Opc == BO_LAnd || Opc == BO_LOr;
  }
  static bool isAssignmentOp(Opcode Opc) {
    return Opc >= BO_Assign && Opc <= BO_OrAssign;
  }
  static bool isCompoundAssignmentOp(Opcode Opc) {
    return Opc > BO_Assign && Opc <= BO_OrAssign;
  }
  static bool isShiftAssignOp(Opcode Opc) {
    return Opc == BO_ShlAssign || Opc == BO_ShrAssign;
  }

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
  bool isCompoundAssignmentOp() const {
    return isCompoundAssignmentOp(getOpcode());
  }
  bool isShiftAssignOp() const { return isShiftAssignOp(getOpcode()); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class CallExpr : public Expr {
  ExprPtr CalleeExpr;
  std::vector<ExprPtr> Arguments;
  // option for named call, such as set({value: 2, key: 3});
  std::optional<std::vector<std::string>> Names;

public:
  CallExpr(ExprPtr &&CalleeExpr, std::vector<ExprPtr> &&Arguments)
      : CalleeExpr(std::move(CalleeExpr)), Arguments(std::move(Arguments)) {}
  CallExpr(ExprPtr &&CalleeExpr, std::vector<ExprPtr> &&Arguments,
           std::vector<std::string> &&Names)
      : CalleeExpr(std::move(CalleeExpr)), Arguments(std::move(Arguments)),
        Names(std::move(Names)) {}
  const Expr *getCalleeExpr() const { return CalleeExpr.get(); }
  std::vector<const Expr *> getArguments() const {
    std::vector<const Expr *> arguments;
    for (auto &arg : Arguments)
      arguments.emplace_back(arg.get());
    return arguments;
  }
  const std::optional<std::vector<std::string>> &getNames() const {
    return Names;
  }
  std::optional<std::vector<std::string>> &getNames() {
    return Names;
  }

  bool isNamedCall() { return Names.has_value(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
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
  std::string name;
public:
  Identifier(std::string &&Name) : name(Name) {}
  void setName(std::string &&Name) { name = Name; }
  std::string getName() const { return name; }
public:
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ElementaryTypeNameExpr : public Expr {
  // TODO
};

class BooleanLiteral : public Expr {
  bool value;
public:
  BooleanLiteral(bool val) : value(val) {}
  void setValue(bool val) { value = val; }
  bool getValue() const { return value; }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class StringLiteral : public Expr {
  std::string value;
public:
  StringLiteral(std::string &&val) : value(val) {}
  void setValue(std::string &&val) { value = val; }
  std::string getValue() const { return value; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

// solidity NumberLiteral support 256bit operation, Boost library should be used.
class NumberLiteral : public Expr {
  int value;
public:
  NumberLiteral(int val) : value(val) {}
  void setValue(int val) { value = val; }
  int getValue() const { return value; }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
