// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace soll {

class DeclarationContainer {
public:
  DeclarationContainer() = default;
  explicit DeclarationContainer(const ASTNode *EnclosingNode,
                                DeclarationContainer *EnclosingContainer)
      : EnclosingNode(EnclosingNode), EnclosingContainer(EnclosingContainer) {
    if (EnclosingContainer)
      EnclosingContainer->InnerContainers.emplace_back(this);
  }

  ASTNode const *enclosingNode() const { return EnclosingNode; }

  void activateVariable(llvm::StringRef Name) {
    assert(InvisibleDeclarations.count(Name) &&
           InvisibleDeclarations.at(Name).size() == 1);
    assert(Declarations.count(Name) == 0 || Declarations.at(Name).empty());

    Declarations[Name].emplace_back(InvisibleDeclarations.at(Name).front());
    InvisibleDeclarations.erase(Name);
  }

  bool isInvisible(llvm::StringRef Name) const {
    return InvisibleDeclarations.count(Name);
  }

  bool registerDeclaration(const Decl *D, bool Invisible, bool Update) {
    return registerDeclaration(D, llvm::StringRef(), nullptr, Invisible,
                               Update);
  }

  bool registerDeclaration(const Decl *D, llvm::StringRef Name,
                           const SourceRange *Loc, bool Invisible,
                           bool Update) {
    if (Name.empty()) {
      Name = D->getName();
    }

    if (Name.empty())
      return true;

    if (Update) {
      assert(!dynamic_cast<const FunctionDecl *>(D) &&
             "Attempt to update function definition.");
      Declarations.erase(Name);
      InvisibleDeclarations.erase(Name);
      return true;
    } else {
      if (conflictingDeclaration(D, Name))
        return false;
      if (EnclosingContainer && D->isVisibleAsUnqualifiedName()) {
        // TODO: isVisibleAsUnqualifiedName for struct not merged yet.
        //       It use old path to handle
      }
    }

    std::vector<const Decl *> &Decls =
        Invisible ? InvisibleDeclarations[Name] : Declarations[Name];
    if (find(Decls.begin(), Decls.end(), D) == Decls.end())
      Decls.emplace_back(D);
    return true;
  }

  std::vector<const Decl *>
  resolveName(llvm::StringRef Name, bool Recursive = false,
              bool AlsoInvisible = false,
              bool OnlyVisibleAsUnqualifiedNames = false) const {
    std::vector<const Decl *> Res;

    if (Declarations.count(Name)) {
      for (auto E : Declarations.at(Name)) {
        if (OnlyVisibleAsUnqualifiedNames) {
          if (!E->isVisibleAsUnqualifiedName())
            continue;
        }
        Res.emplace_back(E);
      }
    }

    if (AlsoInvisible && InvisibleDeclarations.count(Name)) {
      for (auto E : InvisibleDeclarations.at(Name)) {
        if (OnlyVisibleAsUnqualifiedNames) {
          if (!E->isVisibleAsUnqualifiedName())
            continue;
        }
        Res.emplace_back(E);
      }
    }

    if (Res.empty() && Recursive && EnclosingContainer)
      Res = EnclosingContainer->resolveName(Name, true, AlsoInvisible,
                                            OnlyVisibleAsUnqualifiedNames);

    return Res;
  }

  const Decl *conflictingDeclaration(const Decl *D,
                                     llvm::StringRef Name) const {
    if (Name.empty())
      Name = D->getName();
    assert(!Name.empty());

    std::vector<Decl const *> Decls;
    if (Declarations.count(Name))
      Decls += Declarations.at(Name);

    if (InvisibleDeclarations.count(Name))
      Decls += InvisibleDeclarations.at(Name);

    if (dynamic_cast<const FunctionDecl *>(D) ||
        dynamic_cast<const EventDecl *>(D) ||
        dynamic_cast<const MagicVariableDecl *>(D)) {
      for (const Decl *RegDecl : Decls) {
        if (dynamic_cast<const FunctionDecl *>(D) &&
            !dynamic_cast<const FunctionDecl *>(RegDecl))
          return RegDecl;
        if (dynamic_cast<const EventDecl *>(D) &&
            !dynamic_cast<const EventDecl *>(RegDecl))
          return RegDecl;
        if (dynamic_cast<const MagicVariableDecl *>(D) &&
            !dynamic_cast<const MagicVariableDecl *>(RegDecl))
          return RegDecl;
      }
    } else if (Decls.size() == 1 && Decls.front() == D) {
      return nullptr;
    } else if (!Decls.empty()) {
      return Decls.front();
    }

    return nullptr;
  }

private:
  const ASTNode *EnclosingNode = nullptr;
  const DeclarationContainer *EnclosingContainer = nullptr;
  std::vector<const DeclarationContainer *> InnerContainers;
  std::map<llvm::StringRef, std::vector<const Decl *>> Declarations;
  std::map<llvm::StringRef, std::vector<const Decl *>> InvisibleDeclarations;
};

} // namespace soll