// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/OperationKinds.h"
#include "soll/AST/Stmt.h"
#include "soll/AST/Type.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Lex/Token.h"

#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace soll {

enum class ValueKind { VK_Unknown, VK_SValue, VK_LValue, VK_RValue };

/// Expr: Base class for all expressions.
class Expr : public ExprStmt {
  ValueKind ExprValueKind;
  TypePtr Ty;

public:
  Expr(SourceRange L) : ExprStmt(L), ExprValueKind(ValueKind::VK_Unknown) {}
  Expr(SourceRange L, ValueKind VK, TypePtr Ty)
      : ExprStmt(L), ExprValueKind(VK), Ty(Ty) {}
  ValueKind getValueKind() const { return ExprValueKind; }
  void setValueKind(ValueKind vk) { ExprValueKind = vk; }
  bool isSValue() const { return getValueKind() == ValueKind::VK_SValue; }
  bool isLValue() const { return getValueKind() == ValueKind::VK_LValue; }
  bool isRValue() const { return getValueKind() == ValueKind::VK_RValue; }
  TypePtr getType() { return Ty; }
  const TypePtr &getType() const { return Ty; }
  void setType(TypePtr Ty) { this->Ty = Ty; }
};

class Identifier : public Expr {
public:
  enum class SpecialIdentifier {
    block,
    block_coinbase,
    block_difficulty,
    block_gaslimit,
    block_number,
    block_timestamp,
    blockhash,
    gasleft,
    msg,
    msg_data,
    msg_sender,
    msg_sig,
    msg_value,
    now,
    tx,
    tx_gasprice,
    tx_origin,
    abi,
    abi_decode,
    abi_encode,
    abi_encodePacked,
    abi_encodeWithSelector,
    abi_encodeWithSignature,
    assert_,
    require,
    revert,
    addmod,
    mulmod,
    keccak256,
    sha256,
    ripemd160,
    ecrecover,
    address_balance,
    address_transfer,
    address_send,
    address_call,
    address_delegatecall,
    address_staticcall,
    array_length,
    array_push,
    array_pop,
    fixedbytes_length,
    function_selector,
    function_gas,
    function_value,
    this_,
  };

private:
  Token T;
  std::variant<std::monostate, Decl *, SpecialIdentifier> D;
  void updateTypeFromCurrentDecl();

public:
  Identifier(const Token &T);
  Identifier(const Token &T, Decl *D);
  Identifier(const Token &T, SpecialIdentifier D, TypePtr Ty);

  bool isResolved() const { return !std::holds_alternative<std::monostate>(D); }
  bool isSpecialIdentifier() const {
    return std::holds_alternative<SpecialIdentifier>(D);
  }
  const Token &getToken() const { return T; }
  llvm::StringRef getName() const { return T.getIdentifierInfo()->getName(); }
  void setCorrespondDecl(Decl *D) {
    this->D = D;
    updateTypeFromCurrentDecl();
  }
  Decl *getCorrespondDecl() { return std::get<Decl *>(D); }
  const Decl *getCorrespondDecl() const { return std::get<Decl *>(D); }
  SpecialIdentifier getSpecialIdentifier() const {
    return std::get<SpecialIdentifier>(D);
  }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

/// TupleExpr: A type expression such as "(a, b)" or
/// Note that "(a)" is not a TupleExpr, but a ParenExpr.
/// (Solc see them both as TupleExpr, which is terrible.)
class TupleExpr {
  // TODO
};

/// UnaryOperator: A unary operation such as "++a" or "!a",
/// The operand should have the proper type already to construct this node.
class UnaryOperator : public Expr {
  ExprPtr Val;
  UnaryOperatorKind Opc;

public:
  typedef UnaryOperatorKind Opcode;
  UnaryOperator(SourceRange L, ExprPtr &&val, Opcode opc)
      : Expr(L), Val(std::move(val)), Opc(opc) {}

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setSubExpr(ExprPtr &&E) { Val = std::move(E); }

  Opcode getOpcode() const { return Opc; }

  Expr *getSubExpr() { return Val.get(); }
  const Expr *getSubExpr() const { return Val.get(); }

  /// isPostfix - Return true if this is a postfix operation, like x++.
  static inline constexpr bool isPostfix(Opcode Op) {
    return Op == UO_PostInc || Op == UO_PostDec;
  }
  /// isPrefix - Return true if this is a prefix operation, like --x.
  static inline constexpr bool isPrefix(Opcode Op) {
    return Op == UO_PreInc || Op == UO_PreDec;
  }
  static inline constexpr bool isIncrementOp(Opcode Op) {
    return Op == UO_PreInc || Op == UO_PostInc;
  }
  static inline constexpr bool isDecrementOp(Opcode Op) {
    return Op == UO_PreDec || Op == UO_PostDec;
  }
  static inline constexpr bool isIncrementDecrementOp(Opcode Op) {
    return Op <= UO_PreDec;
  }
  static inline constexpr bool isArithmeticOp(Opcode Op) {
    return Op >= UO_Plus && Op <= UO_IsZero;
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

/// BinaryOperator: A binary operation such as "a + b" or "a = b",
/// The operands should have the proper types already to construct this node.
class BinaryOperator : public Expr {
  enum { LHS, RHS, END };
  ExprPtr SubExprs[END];
  BinaryOperatorKind Opc;

public:
  typedef BinaryOperatorKind Opcode;
  BinaryOperator(SourceRange L, ExprPtr &&lhs, ExprPtr &&rhs, Opcode opc);

  void setOpcode(Opcode Opc) { this->Opc = Opc; }
  void setLHS(ExprPtr &&E) { SubExprs[LHS] = std::move(E); }
  void setRHS(ExprPtr &&E) { SubExprs[RHS] = std::move(E); }

  Opcode getOpcode() const { return Opc; }

  Expr *getLHS() { return SubExprs[LHS].get(); }
  const Expr *getLHS() const { return SubExprs[LHS].get(); }

  Expr *getRHS() { return SubExprs[RHS].get(); }
  const Expr *getRHS() const { return SubExprs[RHS].get(); }

  static inline constexpr Opcode compoundAssignmentToNormal(Opcode Opc) {
    switch (Opc) {
    case BO_MulAssign:
      return BO_Mul;
    case BO_DivAssign:
      return BO_Div;
    case BO_RemAssign:
      return BO_Rem;
    case BO_AddAssign:
      return BO_Add;
    case BO_SubAssign:
      return BO_Sub;
    case BO_ShlAssign:
      return BO_Shl;
    case BO_ShrAssign:
      return BO_Shr;
    case BO_AndAssign:
      return BO_And;
    case BO_XorAssign:
      return BO_Xor;
    case BO_OrAssign:
      return BO_Or;
    default:
      return Opc;
    }
  }
  static inline constexpr bool isMultiplicativeOp(Opcode Opc) {
    return Opc >= BO_Mul && Opc <= BO_Rem;
  }
  static inline constexpr bool isAdditiveOp(Opcode Opc) {
    return Opc == BO_Add || Opc == BO_Sub;
  }
  static inline constexpr bool isShiftOp(Opcode Opc) {
    return Opc == BO_Shl || Opc == BO_Shr;
  }
  static inline constexpr bool isBitwiseOp(Opcode Opc) {
    return Opc >= BO_And && Opc <= BO_Or;
  }
  static inline constexpr bool isRelationalOp(Opcode Opc) {
    return Opc >= BO_LT && Opc <= BO_GE;
  }
  static inline constexpr bool isEqualityOp(Opcode Opc) {
    return Opc == BO_EQ || Opc == BO_NE;
  }
  static inline constexpr bool isComparisonOp(Opcode Opc) {
    return Opc >= BO_LT && Opc <= BO_NE;
  }
  static inline constexpr bool isCommaOp(Opcode Opc) { return Opc == BO_Comma; }
  static inline constexpr bool isLogicalOp(Opcode Opc) {
    return Opc == BO_LAnd || Opc == BO_LOr;
  }
  static inline constexpr bool isAssignmentOp(Opcode Opc) {
    return Opc >= BO_Assign && Opc <= BO_OrAssign;
  }
  static inline constexpr bool isCompoundAssignmentOp(Opcode Opc) {
    return Opc > BO_Assign && Opc <= BO_OrAssign;
  }
  static inline constexpr bool isShiftAssignOp(Opcode Opc) {
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

/// CallExpr: A function call such as "f(a, b)".
class CallExpr : public Expr {
  ExprPtr CalleeExpr;
  std::vector<ExprPtr> Arguments;
  // option for named call, such as set({value: 2, key: 3});
  std::optional<std::vector<std::string>> Names;

public:
  CallExpr(SourceRange L, ExprPtr &&CalleeExpr,
           std::vector<ExprPtr> &&Arguments)
      : Expr(L), CalleeExpr(std::move(CalleeExpr)),
        Arguments(std::move(Arguments)) {}
  CallExpr(SourceRange L, ExprPtr &&CalleeExpr,
           std::vector<ExprPtr> &&Arguments, std::vector<std::string> &&Names)
      : Expr(L), CalleeExpr(std::move(CalleeExpr)),
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

/// CastExpr: Base class for ImplicitCastExpr and ExplicitCastExpr.
/// The casted type is store in Ty of class Expr.
class CastExpr : public Expr {
  ExprPtr SubExpr;
  CastKind CastK;

protected:
  CastExpr(SourceRange L, ExprPtr &&SE, CastKind CK, TypePtr Ty)
      : Expr(L, ValueKind::VK_RValue, Ty), SubExpr(std::move(SE)), CastK(CK) {}

public:
  Expr *getSubExpr() { return SubExpr.get(); }
  const Expr *getSubExpr() const { return SubExpr.get(); }
  CastKind getCastKind() const { return CastK; }
  void setCastKind(CastKind CK) { CastK = CK; }
};

class ImplicitCastExpr : public CastExpr {
public:
  ImplicitCastExpr(ExprPtr &&TV)
      : CastExpr(TV->getLocation(), std::move(TV), CastKind::None, nullptr) {}
  ImplicitCastExpr(ExprPtr &&TV, CastKind CK, TypePtr Ty)
      : CastExpr(TV->getLocation(), std::move(TV), CK, Ty) {}

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ExplicitCastExpr : public CastExpr {
public:
  ExplicitCastExpr(SourceRange L, ExprPtr &&TV, CastKind CK, TypePtr Ty)
      : CastExpr(L, std::move(TV), CK, Ty) {}
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class NewExpr : public Expr {
  // TODO
public:
  NewExpr(SourceRange L, TypePtr Ty) : Expr(L, ValueKind::VK_RValue, Ty) {}
};

class MemberExpr : public Expr {
  ExprPtr Base;
  std::unique_ptr<Identifier> Name;

public:
  MemberExpr(SourceRange L, ExprPtr &&Base, std::unique_ptr<Identifier> &&Name)
      : Expr(L, ValueKind::VK_RValue, Name->getType()), Base(std::move(Base)),
        Name(std::move(Name)) {}

  void setBase(ExprPtr &&Base) { this->Base = std::move(Base); }
  void setName(std::unique_ptr<Identifier> &&Name) {
    this->setType(Name->getType());
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
  IndexAccess(SourceRange L, ExprPtr &&Base, ExprPtr &&Index)
      : Expr(L), Base(std::move(Base)), Index(std::move(Index)) {}

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
  ParenExpr(SourceRange L, ExprPtr &&Val)
      : Expr(L, Val->getValueKind(), Val->getType()), Val(std::move(Val)) {}

  Expr *getSubExpr() { return Val.get(); }
  const Expr *getSubExpr() const { return Val.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ConstantExpr : public Expr {
  // TODO
public:
  ConstantExpr(SourceRange L) : Expr(L, ValueKind::VK_RValue, nullptr) {}
};

class ElementaryTypeNameExpr : public Expr {
  // TODO
public:
  ElementaryTypeNameExpr(SourceRange L)
      : Expr(L, ValueKind::VK_LValue, nullptr) {}
};

class BooleanLiteral : public Expr {
  bool value;

public:
  BooleanLiteral(const Token &T, bool val)
      : Expr(T.getRange(), ValueKind::VK_RValue,
             std::make_shared<BooleanType>()),
        value(val) {}
  void setValue(bool val) { value = val; }
  bool getValue() const { return value; }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class StringLiteral : public Expr {
  std::string value;

public:
  StringLiteral(const Token &T, std::string &&val)
      : Expr(T.getRange(), ValueKind::VK_RValue,
             std::make_shared<StringType>()),
        value(std::move(val)) {}
  void setValue(std::string &&val) { value = std::move(val); }
  std::string getValue() const { return value; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class NumberLiteral : public Expr {
  llvm::APInt Value;
  static TypePtr getFittingType(bool Signed, unsigned int BitWidth) {
    if (Signed) {
      return std::make_shared<IntegerType>(IntegerType::getIntN(BitWidth));
    } else {
      return std::make_shared<IntegerType>(IntegerType::getUIntN(BitWidth));
    }
  }

public:
  // TODO: replace this, current impl. always set uint256
  // should set proper int type based on value (Solidity's rule)
  // for example:
  //   8    -> uint8
  //   7122 -> uint16
  //   -123 -> int8
  NumberLiteral(const Token &T, bool Signed, llvm::APInt V)
      : Expr(T.getRange(), ValueKind::VK_RValue,
             getFittingType(Signed, V.getBitWidth())),
        Value(V) {}
  const llvm::APInt &getValue() const { return Value; }
  bool IsSigned() const {
    auto *IntTy = dynamic_cast<IntegerType *>(getType().get());
    assert(IntTy != nullptr && "NumberLiteral with non-IntegerType");
    return IntTy->isSigned();
  }
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
