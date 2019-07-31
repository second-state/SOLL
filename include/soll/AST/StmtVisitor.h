#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {

class Block;
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
class Identifier;
class BooleanLiteral;
class StringLiteral;
class NumberLiteral;

template <bool Const> class StmtVisitorBase {
protected:
  using BlockType = typename cond_const<Const, Block>::type;
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
  using IdentifierType = typename cond_const<Const, Identifier>::type;
  using BooleanLiteralType = typename cond_const<Const, BooleanLiteral>::type;
  using StringLiteralType = typename cond_const<Const, StringLiteral>::type;
  using NumberLiteralType = typename cond_const<Const, NumberLiteral>::type;

public:
  virtual ~StmtVisitorBase() noexcept {}
  virtual void visit(BlockType &);
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
  virtual void visit(IdentifierType &);
  virtual void visit(BooleanLiteralType &);
  virtual void visit(StringLiteralType &);
  virtual void visit(NumberLiteralType &);
};

using ConstStmtVisitor = StmtVisitorBase<true>;
using StmtVisitor = StmtVisitorBase<false>;

} // namespace soll
