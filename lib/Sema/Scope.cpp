// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Scope.h"

namespace soll {

std::vector<std::variant<Identifier *, AsmIdentifier *>>
Scope::resolveIdentifiers() {
  std::vector<std::variant<Identifier *, AsmIdentifier *>> Unresolvable;
  for (auto &I : UnresolvedIdentifiers) {
    const bool Founded = std::visit(
        [this](const auto &I) {
          assert(!I->isResolved());
          if (auto Iter = Decls.find(I->getName()); Iter != Decls.end()) {
            I->setCorrespondDecl(Iter->second);
            return true;
          }
          return false;
        },
        I);
    if (Founded) {
      continue;
    }
    if (Parent) {
      Parent->addUnresolved(I);
      continue;
    }
    // Unresolvable identifier founded!
    Unresolvable.push_back(I);
  }
  for (auto &I : UnresolvedExternalIdentifiers) {
    if (Parent) {
      if (Flags & FunctionScope) {
        Parent->addUnresolved(I);
      } else {
        Parent->addUnresolvedExternal(I);
      }
      continue;
    }
    Unresolvable.push_back(I);
  }
  // Done
  return Unresolvable;
}

} // namespace soll
