#pragma once

#include "soll/ADT/STLExtras.h"

namespace soll {

class FunctionDecl;
class VarDecl;

template <bool Const> class DeclVisitorBase {
protected:
  using FunctionDeclType = typename cond_const<Const, FunctionDecl>::type;
  using VarDeclType = typename cond_const<Const, VarDecl>::type;

public:
  virtual void visit(FunctionDeclType &);
  virtual void visit(VarDeclType &);
};

using ConstDeclVisitor = DeclVisitorBase<true>;
using DeclVisitor = DeclVisitorBase<false>;

} // namespace soll
