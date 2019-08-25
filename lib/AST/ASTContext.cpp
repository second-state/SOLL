// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTContext.h"
namespace soll {

void ASTContext::addIdentifierDecl(const std::string &S, const Decl &D) {
  ID2DeclTable[S] = &D;
}

// TODO: refactor this
// current impl. assumes no name scope
const Decl *ASTContext::findIdentifierDecl(const std::string &S) {
  if (ID2DeclTable.count(S))
    return ID2DeclTable[S];
  else
    return nullptr;
}

} // namespace soll