// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Expr.h"
#include <utility>

namespace soll {

class AsmIdentifier : public Expr {
public:
  /// List of identifiers to predefined low-level functions.
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
  AsmIdentifier(const std::string &Name, SpecialIdentifier D, TypePtr Ty);
  AsmIdentifier(const std::string &Name, Decl *D);

  void setName(const std::string &Name) { this->Name = Name; }
  std::string getName() const { return Name; }
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

class AsmIdentifierList {
  std::vector<std::unique_ptr<AsmIdentifier>> Ids;

public:
  AsmIdentifierList(std::vector<std::unique_ptr<AsmIdentifier>> &&Ids)
      : Ids(std::move(Ids)) {}

  std::vector<AsmIdentifier *> getIdentifiers();
  std::vector<const AsmIdentifier *> getIdentifiers() const;

  void accept(StmtVisitor &);
  void accept(ConstStmtVisitor &) const;
};

class AsmUnaryOperator : public UnaryOperator {
public:
  AsmUnaryOperator(ExprPtr &&Arg0, TypePtr Ty, UnaryOperatorKind opc)
      : UnaryOperator(std::move(Arg0), opc, Ty) {}
};

class AsmBinaryOperator : public BinaryOperator {
public:
  AsmBinaryOperator(ExprPtr &&Arg0, ExprPtr &&Arg1, TypePtr Ty,
                    BinaryOperatorKind opc)
      : BinaryOperator(std::move(Arg0), std::move(Arg1), opc, Ty) {}
};

/// iszerou256(u256) -> u256
///
/// Although it's an unary operator, the implementation here inherits from
/// binary operator and compared the argument with the number literal 0.
class AsmIsZeroOperator : public BinaryOperator {
public:
  AsmIsZeroOperator(ExprPtr &&Arg, TypePtr Ty)
      : BinaryOperator(std::move(Arg), std::make_unique<NumberLiteral>(0),
                       BinaryOperatorKind::BO_EQ, Ty) {}
};

} // namespace soll
