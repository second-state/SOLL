// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {

class SourceUnit;
class PragmaDirective;
class UsingFor;
class ContractDecl;
class FunctionDecl;
class EventDecl;
class ParamList;
class CallableVarDecl;
class VarDecl;
class StructDecl;
class ModifierInvocation;

class YulCode;
class YulData;
class YulObject;
class AsmFunctionDecl;
class AsmVarDecl;

template <bool Const> class DeclVisitorBase {
protected:
  using SourceUnitType = typename cond_const<Const, SourceUnit>::type;
  using PragmaDirectiveType = typename cond_const<Const, PragmaDirective>::type;
  using UsingForType = typename cond_const<Const, UsingFor>::type;
  using ContractDeclType = typename cond_const<Const, ContractDecl>::type;
  using FunctionDeclType = typename cond_const<Const, FunctionDecl>::type;
  using EventDeclType = typename cond_const<Const, EventDecl>::type;
  using ParamListType = typename cond_const<Const, ParamList>::type;
  using CallableVarDeclType = typename cond_const<Const, CallableVarDecl>::type;
  using VarDeclType = typename cond_const<Const, VarDecl>::type;
  using StructDeclType = typename cond_const<Const, StructDecl>::type;
  using ModifierInvocationType =
      typename cond_const<Const, ModifierInvocation>::type;

  using YulCodeType = typename cond_const<Const, YulCode>::type;
  using YulDataType = typename cond_const<Const, YulData>::type;
  using YulObjectType = typename cond_const<Const, YulObject>::type;
  using AsmFunctionDeclType = typename cond_const<Const, AsmFunctionDecl>::type;
  using AsmVarDeclType = typename cond_const<Const, AsmVarDecl>::type;

public:
  virtual ~DeclVisitorBase() noexcept {}
  virtual void visit(SourceUnitType &);
  virtual void visit(PragmaDirectiveType &);
  virtual void visit(UsingForType &);
  virtual void visit(ContractDeclType &);
  virtual void visit(FunctionDeclType &);
  virtual void visit(EventDeclType &);
  virtual void visit(ParamListType &);
  virtual void visit(CallableVarDeclType &);
  virtual void visit(VarDeclType &);
  virtual void visit(StructDeclType &);
  virtual void visit(ModifierInvocationType &);

  virtual void visit(YulCodeType &);
  virtual void visit(YulDataType &);
  virtual void visit(YulObjectType &);
  virtual void visit(AsmFunctionDeclType &);
  virtual void visit(AsmVarDeclType &);
};

using ConstDeclVisitor = DeclVisitorBase<true>;
using DeclVisitor = DeclVisitorBase<false>;

} // namespace soll
