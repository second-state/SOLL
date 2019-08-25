// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/Decl.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <unordered_map>
namespace soll {

class ASTContext : public llvm::RefCountedBase<ASTContext> {
  // TODO: refactor this
  // current impl. assumes no name scope
  std::unordered_map<std::string, const Decl *> ID2DeclTable;

public:
  void addIdentifierDecl(const std::string &S, const Decl &D);
  const Decl *findIdentifierDecl(const std::string &S);
};

} // namespace soll
