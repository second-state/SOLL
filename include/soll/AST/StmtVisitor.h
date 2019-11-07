// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {

class Block;
class EmitStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class ContinueStmt;
class BreakStmt;
class ReturnStmt;
class DeclStmt;
class UnaryOperator;
class BinaryOperator;
class CallExpr;
class ImplicitCastExpr;
class ExplicitCastExpr;
class ParenExpr;
class MemberExpr;
class IndexAccess;
class Identifier;
class BooleanLiteral;
class StringLiteral;
class NumberLiteral;

class YulIdentifierList;
class YulAssignment;
class YulForStmt;
class YulCaseStmt;
class YulDefaultStmt;
class YulSwitchStmt;
class YulIdentifier;
class YulLiteral;

template <bool Const> class StmtVisitorBase {
protected:
  using BlockType = typename cond_const<Const, Block>::type;
  using EmitStmtType = typename cond_const<Const, EmitStmt>::type;
  using IfStmtType = typename cond_const<Const, IfStmt>::type;
  using WhileStmtType = typename cond_const<Const, WhileStmt>::type;
  using ForStmtType = typename cond_const<Const, ForStmt>::type;
  using ContinueStmtType = typename cond_const<Const, ContinueStmt>::type;
  using BreakStmtType = typename cond_const<Const, BreakStmt>::type;
  using ReturnStmtType = typename cond_const<Const, ReturnStmt>::type;
  using DeclStmtType = typename cond_const<Const, DeclStmt>::type;
  using UnaryOperatorType = typename cond_const<Const, UnaryOperator>::type;
  using BinaryOperatorType = typename cond_const<Const, BinaryOperator>::type;
  using CallExprType = typename cond_const<Const, CallExpr>::type;
  using ImplicitCastExprType =
      typename cond_const<Const, ImplicitCastExpr>::type;
  using ExplicitCastExprType =
      typename cond_const<Const, ExplicitCastExpr>::type;
  using ParenExprType = typename cond_const<Const, ParenExpr>::type;
  using MemberExprType = typename cond_const<Const, MemberExpr>::type;
  using IndexAccessType = typename cond_const<Const, IndexAccess>::type;
  using IdentifierType = typename cond_const<Const, Identifier>::type;
  using BooleanLiteralType = typename cond_const<Const, BooleanLiteral>::type;
  using StringLiteralType = typename cond_const<Const, StringLiteral>::type;
  using NumberLiteralType = typename cond_const<Const, NumberLiteral>::type;

  using YulIdentifierListType =
      typename cond_const<Const, YulIdentifierList>::type;
  using YulAssignmentType = typename cond_const<Const, YulAssignment>::type;
  using YulForStmtType = typename cond_const<Const, YulForStmt>::type;
  using YulCaseStmtType = typename cond_const<Const, YulCaseStmt>::type;
  using YulDefaultStmtType = typename cond_const<Const, YulDefaultStmt>::type;
  using YulSwitchStmtType = typename cond_const<Const, YulSwitchStmt>::type;
  using YulIdentifierType = typename cond_const<Const, YulIdentifier>::type;
  using YulLiteralType = typename cond_const<Const, YulLiteral>::type;

public:
  virtual ~StmtVisitorBase() noexcept {}
  virtual void visit(BlockType &);
  virtual void visit(EmitStmtType &);
  virtual void visit(IfStmtType &);
  virtual void visit(WhileStmtType &);
  virtual void visit(ForStmtType &);
  virtual void visit(ContinueStmtType &);
  virtual void visit(BreakStmtType &);
  virtual void visit(ReturnStmtType &);
  virtual void visit(DeclStmtType &);
  virtual void visit(UnaryOperatorType &);
  virtual void visit(BinaryOperatorType &);
  virtual void visit(CallExprType &);
  virtual void visit(ImplicitCastExprType &);
  virtual void visit(ExplicitCastExprType &);
  virtual void visit(ParenExprType &);
  virtual void visit(MemberExprType &);
  virtual void visit(IndexAccessType &);
  virtual void visit(IdentifierType &);
  virtual void visit(BooleanLiteralType &);
  virtual void visit(StringLiteralType &);
  virtual void visit(NumberLiteralType &);

  virtual void visit(YulIdentifierListType &);
  virtual void visit(YulAssignmentType &);
  virtual void visit(YulForStmtType &);
  virtual void visit(YulCaseStmtType &);
  virtual void visit(YulDefaultStmtType &);
  virtual void visit(YulSwitchStmtType &);
  virtual void visit(YulIdentifierType &);
  virtual void visit(YulLiteralType &);
};

using ConstStmtVisitor = StmtVisitorBase<true>;
using StmtVisitor = StmtVisitorBase<false>;

} // namespace soll
