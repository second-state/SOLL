// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Decl.h"

namespace soll {

class AsmFunctionDecl : public CallableVarDecl {
  std::unique_ptr<Block> Body;
  TypePtr FuncTy;

public:
  AsmFunctionDecl(SourceRange L, llvm::StringRef Name,
                  std::unique_ptr<ParamList> &&Params,
                  std::unique_ptr<ParamList> &&ReturnParams,
                  std::unique_ptr<Block> &&Body);

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }
  void setBody(std::unique_ptr<Block> &&B) { Body = std::move(B); }
  TypePtr getType() const { return FuncTy; }

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;
};

class AsmVarDecl : public VarDeclBase {
public:
  AsmVarDecl(SourceRange L, llvm::StringRef name, TypePtr &&T, ExprPtr &&value)
      : VarDeclBase(L, name, Visibility::Default, std::move(T),
                    std::move(value)) {}

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;
};

} // namespace soll
