// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/ExprAsm.h"
#include <cassert>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/iterator_range.h>
#include <memory>
#include <variant>
#include <vector>

namespace soll {
class Scope {
  unsigned Flags;
  unsigned short Depth;

  Scope *Parent;
  Scope *FunctionParent;
  Scope *BreakParent;
  Scope *ContinueParent;

  llvm::StringMap<Decl *> Decls;
  std::vector<std::variant<Identifier *, AsmIdentifier *>>
      UnresolvedIdentifiers, UnresolvedExternalIdentifiers;

public:
  enum ScopeFlags : unsigned {
    // can return
    FunctionScope = 0x01,
    // can break: while, do, switch, for
    BreakScope = 0x02,
    // can continue: while, do, for
    ContinueScope = 0x04,
  };

public:
  Scope(Scope *Parent, unsigned Flags) : Flags(Flags), Parent(Parent) {
    if (Parent) {
      Depth = Parent->Depth + 1;
      FunctionParent = Parent->FunctionParent;
    } else {
      Depth = 0;
      FunctionParent = nullptr;
    }

    if (Parent && !(Flags & FunctionScope)) {
      BreakParent = Parent->BreakParent;
      ContinueParent = Parent->ContinueParent;
    } else {
      BreakParent = ContinueParent = nullptr;
    }

    if (Flags & FunctionScope) {
      FunctionParent = this;
    }
    if (Flags & BreakScope) {
      BreakParent = this;
    }
    if (Flags & ContinueScope) {
      ContinueParent = this;
    }
  }
  const Scope *getParent() const { return Parent; }
  const Scope *getFunctionParent() const { return FunctionParent; }
  const Scope *getBreakParent() const { return BreakParent; }
  const Scope *getContinueParent() const { return ContinueParent; }

  bool addDecl(Decl *D) { return Decls.try_emplace(D->getName(), D).second; }
  void addUnresolved(std::variant<Identifier *, AsmIdentifier *> I) {
    UnresolvedIdentifiers.push_back(std::move(I));
  }
  void addUnresolvedExternal(std::variant<Identifier *, AsmIdentifier *> I) {
    UnresolvedExternalIdentifiers.push_back(std::move(I));
  }
  std::vector<std::variant<Identifier *, AsmIdentifier *>> resolveIdentifiers();
  Decl *lookupName(llvm::StringRef Name) const {
    if (auto Iter = Decls.find(Name); Iter != Decls.end()) {
      return Iter->second;
    }
    if (Parent) {
      return Parent->lookupName(Name);
    }
    return nullptr;
  }
};

} // namespace soll
