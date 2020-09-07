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
    /// inline assembly
    shl,
    shr,
    sar,
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
    staticcall,
    abort,
    stop,
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
    selfbalance,
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
    address,
    returndatasize,
    returndatacopy,
    /// object
    datasize,
    dataoffset,
    datacopy,
    /// misc
    pop,
    invalid,
    chainid,
    discard,
    discardu256,
    splitu256tou64,
    combineu64tou256,
    keccak256,
    setimmutable,
    loadimmutable,
    linkersymbol
  };

private:
  Token T;
  std::variant<std::monostate, Decl *, SpecialIdentifier> D;
  void updateTypeFromCurrentDecl();

public:
  AsmIdentifier(const Token &T);
  AsmIdentifier(const Token &T, Decl *D);
  AsmIdentifier(const Token &T, SpecialIdentifier D, TypePtr Ty);

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
  AsmUnaryOperator(SourceRange L, ExprPtr &&Arg0, TypePtr ReturnTy,
                   UnaryOperatorKind opc)
      : UnaryOperator(L, std::move(Arg0), opc) {
    setType(std::move(ReturnTy));
  }
};

class AsmBinaryOperator : public BinaryOperator {
public:
  AsmBinaryOperator(SourceRange L, ExprPtr &&Arg0, ExprPtr &&Arg1,
                    TypePtr ReturnTy, BinaryOperatorKind opc)
      : BinaryOperator(L, std::move(Arg0), std::move(Arg1), opc) {
    setType(std::move(ReturnTy));
  }
};

} // namespace soll
