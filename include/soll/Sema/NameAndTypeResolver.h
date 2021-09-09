// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/DeclarationContainer.h"
#include "soll/Sema/Sema.h"

#include <map>
#include <vector>

namespace soll {
class NameAndTypeResolver {
public:
  void setScope(const ASTNode *Node) { CurrentScope = Scopes[Node].get(); }
  bool resolveNamesAndTypesInternal(ASTNode *Node, bool ResolveCode);
  bool updateDeclaration(const Decl *D) {
    Scopes[nullptr]->registerDeclaration(D, false, true);
    return true;
  }

  void activateVariable(llvm::StringRef Name) {
    if (CurrentScope == nullptr) {
      return;
    }
    if (CurrentScope->isInvisible(Name))
      CurrentScope->activateVariable(Name);
  }

  std::vector<const Decl *> resolveName(llvm::StringRef Name,
                                        const ASTNode *Scope) const;
  std::vector<const Decl *>
  nameFromCurrentScope(llvm::StringRef Name,
                       bool IncludeInvisibles = false) const;
  NameAndTypeResolver(Sema &Action, GlobalContext &GC);
  void Register(SourceUnit &SU);
  bool Resolve(SourceUnit &SU);

private:
  Sema &Action;
  GlobalContext &GC;
  std::map<const ASTNode *, std::shared_ptr<DeclarationContainer>> Scopes;
  DeclarationContainer *CurrentScope = nullptr;
};
} // namespace soll