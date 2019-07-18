#pragma once

namespace {

/// Conditional const
/// cond_const<true,  Type>: const Type
/// cond_const<false, Type>: Type
template <bool Const, class Type> struct cond_const {
  typedef const Type type;
};

template <class Type> struct cond_const<false, Type> { typedef Type type; };

} // namespace

namespace soll {

class UnaryOperator;
class BinaryOperator;
class Identifier;
class Literal;

template <bool Const> class StmtVisitorBase {
protected:
  using UnaryOperatorType = typename cond_const<Const, UnaryOperator>::type;
  using BinaryOperatorType = typename cond_const<Const, BinaryOperator>::type;
  using IdentifierType = typename cond_const<Const, Identifier>::type;
  using LiteralType = typename cond_const<Const, Literal>::type;

public:
  virtual void visit(UnaryOperatorType &);
  virtual void visit(BinaryOperatorType &);
  virtual void visit(IdentifierType &);
  virtual void visit(LiteralType &);
};

using ConstStmtVisitor = StmtVisitorBase<true>;
using StmtVisitor = StmtVisitorBase<false>;

} // namespace soll
