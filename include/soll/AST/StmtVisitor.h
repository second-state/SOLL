#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {

class UnaryOperator;
class BinaryOperator;
class Identifier;
class BooleanLiteral;
class StringLiteral;
class NumberLiteral;

template <bool Const> class StmtVisitorBase {
protected:
  using UnaryOperatorType = typename cond_const<Const, UnaryOperator>::type;
  using BinaryOperatorType = typename cond_const<Const, BinaryOperator>::type;
  using IdentifierType = typename cond_const<Const, Identifier>::type;
  using BooleanLiteralType = typename cond_const<Const, BooleanLiteral>::type;
  using StringLiteralType = typename cond_const<Const, StringLiteral>::type;
  using NumberLiteralType = typename cond_const<Const, NumberLiteral>::type;
public:
  virtual void visit(UnaryOperatorType &);
  virtual void visit(BinaryOperatorType &);
  virtual void visit(IdentifierType &);
  virtual void visit(BooleanLiteralType &);
  virtual void visit(StringLiteralType &);
  virtual void visit(NumberLiteralType &);
};

using ConstStmtVisitor = StmtVisitorBase<true>;
using StmtVisitor = StmtVisitorBase<false>;

} // namespace soll
