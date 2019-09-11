// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Expr.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Stmt.h"

namespace soll {

bool IndexAccess::isStateVariable() const {
  if (auto ID = dynamic_cast<const Identifier *>(getBase())) {
    if (auto *D = dynamic_cast<const VarDecl*>(ID->getCorrespondDecl())) {
      return D->isStateVariable();
    }
  } else if (auto IA = dynamic_cast<const IndexAccess *>(getBase())) {
    return IA->isStateVariable();
  }
  return false;
}

} // namespace soll
