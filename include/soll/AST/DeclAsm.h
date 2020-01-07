// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Decl.h"

namespace soll {

class AsmVarDecl : public VarDeclBase {

public:
  AsmVarDecl(SourceRange L, llvm::StringRef name, TypePtr &&T, ExprPtr &&value)
      : VarDeclBase(L, name, Visibility::Default, std::move(T),
                    std::move(value)) {}

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

} // namespace soll
