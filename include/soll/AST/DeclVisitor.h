#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {
class SourceUnit;
class PragmaDirective;
class ContractDecl;
class FunctionDecl;
class ParamList;
class CallableVarDecl;
class VarDecl;
class ModifierInvocation;

template <bool Const> class DeclVisitorBase {
protected:
  using SourceUnitType = typename cond_const<Const, SourceUnit>::type;
  using PragmaDirectiveType = typename cond_const<Const, PragmaDirective>::type;
  using ContractDeclType = typename cond_const<Const, ContractDecl>::type;
  using FunctionDeclType = typename cond_const<Const, FunctionDecl>::type;
  using ParamListType = typename cond_const<Const, ParamList>::type;
  using CallableVarDeclType = typename cond_const<Const, CallableVarDecl>::type;
  using VarDeclType = typename cond_const<Const, VarDecl>::type;
  using ModifierInvocationType =
      typename cond_const<Const, ModifierInvocation>::type;

public:
  virtual ~DeclVisitorBase() noexcept {}
  virtual void visit(SourceUnitType &);
  virtual void visit(PragmaDirectiveType &);
  virtual void visit(ContractDeclType &);
  virtual void visit(FunctionDeclType &);
  virtual void visit(ParamListType &);
  virtual void visit(CallableVarDeclType &);
  virtual void visit(VarDeclType &);
  virtual void visit(ModifierInvocationType &);
};

using ConstDeclVisitor = DeclVisitorBase<true>;
using DeclVisitor = DeclVisitorBase<false>;

} // namespace soll
