// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Expr.h"
#include <utility>

namespace soll {

class YulIdentifier : public Expr {
public:
  /// List of identifiers to predefined Yul functions.
  enum class SpecialIdentifier {
    /// logical
    not_,
    and_,
    or_,
    xor_,
    /// arithmetic
    addu256,
    subu256,
    mulu256,
    divu256,
    divs256,
    modu256,
    mods256,
    signextendu256,
    expu256,
    addmodu256,
    mulmodu256,
    ltu256,
    gtu256,
    lts256,
    gts256,
    equ256,
    iszerou256,
    notu256,
    andu256,
    oru256,
    xoru256,
    shlu256,
    shru256,
    sars256,
    byte,
    /// memory and storage
    mload,
    mstore,
    mstore8,
    sload,
    sstore,
    msize,
    /// execution control
    create,
    create2,
    call,
    callcode,
    delegatecall,
    abort,
    return_,
    revert,
    selfdestruct,
    log0,
    log1,
    log2,
    log3,
    log4,
    /// state
    blockcoinbase,
    blockdifficulty,
    blockgaslimit,
    blockhash,
    blocknumber,
    blocktimestamp,
    txorigin,
    txgasprice,
    gasleft,
    balance,
    this_,
    caller,
    callvalue,
    calldataload,
    calldatasize,
    calldatacopy,
    codesize,
    codecopy,
    extcodesize,
    extcodecopy,
    extcodehash,
    /// object
    datasize,
    dataoffset,
    datacopy,
    /// misc
    discard,
    discardu256,
    splitu256tou64,
    combineu64tou256,
    keccak256,
  };

private:
  std::string Name;
  std::variant<Decl *, SpecialIdentifier> D;

public:
  YulIdentifier(const std::string &Name, SpecialIdentifier D, TypePtr Ty);
  YulIdentifier(const std::string &Name, Decl *D);

  bool isSpecialIdentifier() const {
    return std::holds_alternative<SpecialIdentifier>(D);
  }
  Decl *getCorrespondDecl() { return std::get<Decl *>(D); }
  const Decl *getCorrespondDecl() const { return std::get<Decl *>(D); }
  SpecialIdentifier getSpecialIdentifier() const {
    return std::get<SpecialIdentifier>(D);
  }

  void accept(soll::StmtVisitor &) override;
  void accept(soll::ConstStmtVisitor &) const override;
};

class YulLiteral : public Expr {
  ExprPtr Literal; ///< NumberLiteral | StringLiteral | BooleanLiteral

public:
  YulLiteral(std::unique_ptr<NumberLiteral> &&Literal)
      : Expr(ValueKind::VK_RValue, Literal->getType()),
        Literal(std::move(Literal)) {}
  YulLiteral(std::unique_ptr<StringLiteral> &&Literal)
      : Expr(ValueKind::VK_RValue, Literal->getType()),
        Literal(std::move(Literal)) {}
  YulLiteral(std::unique_ptr<BooleanLiteral> &&Literal)
      : Expr(ValueKind::VK_RValue, Literal->getType()),
        Literal(std::move(Literal)) {}

  Expr *getLiteral() { return Literal.get(); }
  const Expr *getLiteral() const { return Literal.get(); }

  void accept(soll::StmtVisitor &) override;
  void accept(soll::ConstStmtVisitor &) const override;
};

class YulIdentifierList {
  std::vector<std::unique_ptr<YulIdentifier>> Ids;

public:
  YulIdentifierList(std::vector<std::unique_ptr<YulIdentifier>> &&Ids)
      : Ids(std::move(Ids)) {}

  std::vector<YulIdentifier *> getIdentifiers();
  std::vector<const YulIdentifier *> getIdentifiers() const;

  void accept(StmtVisitor &);
  void accept(ConstStmtVisitor &) const;
};

class YulAssignment : public Expr {
  std::unique_ptr<YulIdentifierList> LHS;
  ExprPtr RHS;

public:
  YulAssignment(std::unique_ptr<YulIdentifierList> &&LHS, ExprPtr &&RHS,
                TypePtr Ty)
      : Expr(ValueKind::VK_RValue, Ty), LHS(std::move(LHS)),
        RHS(std::move(RHS)) {}

  YulIdentifierList *getLHS() { return LHS.get(); }
  const YulIdentifierList *getLHS() const { return LHS.get(); }
  Expr *getRHS() { return RHS.get(); }
  const Expr *getRHS() const { return RHS.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class YulUnaryOperator : public UnaryOperator {
public:
  YulUnaryOperator(ExprPtr &&Arg0, TypePtr Ty, UnaryOperatorKind opc)
      : UnaryOperator(std::move(Arg0), opc, Ty) {}
};

class YulBinaryOperator : public BinaryOperator {
public:
  YulBinaryOperator(ExprPtr &&Arg0, ExprPtr &&Arg1, TypePtr Ty,
                    BinaryOperatorKind opc)
      : BinaryOperator(std::move(Arg0), std::move(Arg1), opc, Ty) {}
};

} // namespace soll
