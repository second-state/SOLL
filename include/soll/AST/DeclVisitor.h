#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {
class SourceUnit;
class PragmaDirective;
class ContractDecl;
class FunctionDecl;
class VarDecl;

template <bool Const> class DeclVisitorBase {
protected:
  using SourceUnitType = typename cond_const<Const, SourceUnit>::type;
  using PragmaDirectiveType = typename cond_const<Const, PragmaDirective>::type;
  using ContractDeclType = typename cond_const<Const, ContractDecl>::type;
  using FunctionDeclType = typename cond_const<Const, FunctionDecl>::type;
  using VarDeclType = typename cond_const<Const, VarDecl>::type;

public:
  virtual ~DeclVisitorBase() noexcept {}
  virtual void visit(SourceUnitType &);
  virtual void visit(PragmaDirectiveType &);
  virtual void visit(ContractDeclType &);
  virtual void visit(FunctionDeclType &);
  virtual void visit(VarDeclType &);
};

using ConstDeclVisitor = DeclVisitorBase<true>;
using DeclVisitor = DeclVisitorBase<false>;

} // namespace soll
