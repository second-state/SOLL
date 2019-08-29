// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/OperationKinds.h"
#include "soll/AST/Stmt.h"
#include "soll/AST/Type.h"

#include <optional>
#include <string>

namespace soll {

enum class ValueKind { VK_LValue, VK_RValue };

class Expr : public ExprStmt {
  // add interface to check whether an expr is an LValue or RValue
  // TODO : override isLValue() and is RValue() for each derived class
  ValueKind ExprValueKind;

protected:
  TypePtr Ty;

public:
  // TODO: refactor this, should only use Expr(ValueKind vk, TypePtr Ty)
  // Because current Expr with Type is still working, so let Ty = nullptr for
  // temp use.
  Expr(ValueKind vk, TypePtr Ty = nullptr) : ExprValueKind(vk), Ty(Ty) {}
  ValueKind getValueKind() const { return ExprValueKind; }
  void setValueKind(ValueKind vk) { ExprValueKind = vk; }
  bool isLValue() const { return getValueKind() == ValueKind::VK_LValue; }
  bool isRValue() const { return getValueKind() == ValueKind::VK_RValue; }
  void setType(TypePtr Ty) { this->Ty = Ty; }
  TypePtr getType() { return Ty; }
  const TypePtr &getType() const { return Ty; }
};

class TupleExpr {
  // TODO
};

class UnaryOperator : public Expr {
  ExprPtr Val;
  UnaryOperatorKind Opc;

public:
  typedef UnaryOperatorKind Opcode;
  // TODO: set value kind in another pass
  UnaryOperator(ExprPtr &&val, Opcode opc)
      : Expr(ValueKind::VK_RValue), Val(std::move(val)), Opc(opc) {}

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setSubExpr(ExprPtr &&E) { Val = std::move(E); }

  Opcode getOpcode() const { return Opc; }

  Expr *getSubExpr() { return Val.get(); }
  const Expr *getSubExpr() const { return Val.get(); }

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
  // TODO: set value kind in another pass
  BinaryOperator(ExprPtr &&lhs, ExprPtr &&rhs, Opcode opc,
                 TypePtr Ty = nullptr);

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setLHS(ExprPtr &&E) { SubExprs[LHS] = std::move(E); }
  void setRHS(ExprPtr &&E) { SubExprs[RHS] = std::move(E); }

  Opcode getOpcode() const { return Opc; }

  Expr *getLHS() { return SubExprs[LHS].get(); }
  const Expr *getLHS() const { return SubExprs[LHS].get(); }

  Expr *getRHS() { return SubExprs[RHS].get(); }
  const Expr *getRHS() const { return SubExprs[RHS].get(); }

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
  CallExpr(ExprPtr &&CalleeExpr, std::vector<ExprPtr> &&Arguments,
           TypePtr Ty = nullptr)
      : Expr(ValueKind::VK_RValue, Ty), CalleeExpr(std::move(CalleeExpr)),
        Arguments(std::move(Arguments)) {}
  CallExpr(ExprPtr &&CalleeExpr, std::vector<ExprPtr> &&Arguments,
           std::vector<std::string> &&Names, TypePtr Ty = nullptr)
      : Expr(ValueKind::VK_RValue, Ty), CalleeExpr(std::move(CalleeExpr)),
        Arguments(std::move(Arguments)), Names(std::move(Names)) {}

  Expr *getCalleeExpr() { return CalleeExpr.get(); }
  const Expr *getCalleeExpr() const { return CalleeExpr.get(); }

  std::vector<Expr *> getArguments();
  std::vector<const Expr *> getArguments() const;

  std::optional<std::vector<std::string>> &getNames() { return Names; }
  const std::optional<std::vector<std::string>> &getNames() const {
    return Names;
  }

  bool isNamedCall() const { return Names.has_value(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class CastExpr : public Expr {
  ExprPtr SubExpr;
  CastKind CastK;

protected:
  CastExpr(ExprPtr &&SE, CastKind CK, TypePtr Ty)
      : SubExpr(std::move(SE)), CastK(CK), Expr(ValueKind::VK_RValue, Ty) {}

public:
  Expr *getSubExpr() { return SubExpr.get(); }
  const Expr *getSubExpr() const { return SubExpr.get(); }
  CastKind getCastKind() const { return CastK; }
};

class ImplicitCastExpr : public CastExpr {
public:
  ImplicitCastExpr(ExprPtr &&TV, CastKind CK, TypePtr Ty)
      : CastExpr(std::move(TV), CK, Ty) {}

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ExplicitCastExpr : public CastExpr {
public:
  ExplicitCastExpr(ExprPtr &&TV, CastKind CK, TypePtr Ty)
      : CastExpr(std::move(TV), CK, Ty) {}
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class NewExpr : public Expr {
  // TODO
public:
  // TODO: set value kind in another pass
  NewExpr() : Expr(ValueKind::VK_RValue) {}
};

class MemberExpr : public Expr {
  ExprPtr Base;
  std::unique_ptr<Identifier> Name;

public:
  // TODO : set MemberExpr to RValue
  MemberExpr(ExprPtr &&Base, std::unique_ptr<Identifier> &&Name)
      : Expr(ValueKind::VK_RValue), Base(std::move(Base)),
        Name(std::move(Name)) {
    // TODO : refactor this
    // hard code to pass erc20
    Ty = std::make_shared<AddressType>();
  }

  void setBase(ExprPtr &&Base) { this->Base = std::move(Base); }
  void setName(std::unique_ptr<Identifier> &&Name) {
    this->Name = std::move(Name);
  }

  Expr *getBase() { return Base.get(); }
  const Expr *getBase() const { return Base.get(); }
  Identifier *getName() { return Name.get(); }
  const Identifier *getName() const { return Name.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class IndexAccess : public Expr {
  ExprPtr Base;
  ExprPtr Index;

public:
  IndexAccess(ExprPtr &&Base, ExprPtr &&Index, TypePtr Ty = nullptr)
      : Expr(ValueKind::VK_LValue, Ty), Base(std::move(Base)),
        Index(std::move(Index)) {}

  void setBase(ExprPtr &&Base) { this->Base = std::move(Base); }
  void setIndex(ExprPtr &&Index) { this->Index = std::move(Index); }

  Expr *getBase() { return Base.get(); }
  const Expr *getBase() const { return Base.get(); }
  Expr *getIndex() { return Index.get(); }
  const Expr *getIndex() const { return Index.get(); }

  bool isStateVariable() const;

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ParenExpr : public Expr {
  ExprPtr Val;

public:
  // TODO: set value kind in another pass
  ParenExpr(ExprPtr &&Val) : Expr(ValueKind::VK_RValue), Val(std::move(Val)) {}

  Expr *getSubExpr() { return Val.get(); }
  const Stmt *getSubExpr() const { return Val.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ConstantExpr : public Expr {
  // TODO
public:
  // TODO: set value kind in another pass
  ConstantExpr() : Expr(ValueKind::VK_RValue) {}
};

class Identifier : public Expr {
  std::string name;
  Decl *D;

public:
  Identifier(const std::string &Name, Decl *D = nullptr);

  void setName(const std::string &Name) { name = Name; }
  std::string getName() const { return name; }
  Decl *getCorrespondDecl() { return D; }
  const Decl *getCorrespondDecl() const { return D; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ElementaryTypeNameExpr : public Expr {
  // TODO
public:
  // TODO: set value kind in another pass
  ElementaryTypeNameExpr() : Expr(ValueKind::VK_LValue) {}
};

class BooleanLiteral : public Expr {
  bool value;

public:
  // TODO: set value kind in another pass
  BooleanLiteral(bool val) : Expr(ValueKind::VK_RValue), value(val) {
    Ty = std::make_shared<BooleanType>();
  }
  void setValue(bool val) { value = val; }
  bool getValue() const { return value; }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class StringLiteral : public Expr {
  std::string value;

public:
  // TODO: set value kind in another pass
  StringLiteral(std::string &&val) : Expr(ValueKind::VK_RValue), value(val) {
    Ty = std::make_shared<StringType>();
  }
  void setValue(std::string &&val) { value = val; }
  std::string getValue() const { return value; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

// solidity NumberLiteral support 256-bit operation, should
// use a 256-bit library.
class NumberLiteral : public Expr {
  int value;

public:
  // TODO: replace this, current impl. always set uint32
  // should set proper int type based on value (Solidity's rule)
  // for example:
  //   8    -> uint8
  //   7122 -> uint16
  //   -123 -> int8
  NumberLiteral(int val) : Expr(ValueKind::VK_RValue), value(val) {
    Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U32);
  }
  void setValue(int val) { value = val; }
  int getValue() const { return value; }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
