// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Decl.h"

namespace soll {

class AsmVarDecl : public VarDeclBase {

public:
  AsmVarDecl(TypePtr &&T, llvm::StringRef name, ExprPtr &&value)
      : VarDeclBase(std::move(T), name, std::move(value), Visibility::Default) {
  }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

} // namespace soll
