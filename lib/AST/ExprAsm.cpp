// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ExprAsm.h"
#include "soll/AST/DeclAsm.h"

namespace soll {

///
/// AsmIdentifier
///
AsmIdentifier::AsmIdentifier(const Token &T)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           nullptr),
      T(T), D() {}

AsmIdentifier::AsmIdentifier(const Token &T, Decl *D)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           nullptr),
      T(T), D(D) {
  updateTypeFromCurrentDecl();
}

AsmIdentifier::AsmIdentifier(const Token &T, SpecialIdentifier D, TypePtr Ty)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           std::move(Ty)),
      T(T), D(D) {}

void AsmIdentifier::updateTypeFromCurrentDecl() {
  Decl *D = getCorrespondDecl();
  if (auto VD = dynamic_cast<AsmVarDecl *>(D)) {
    setType(VD->getType());
  } else if (auto FD = dynamic_cast<AsmFunctionDecl *>(D)) {
    setType(FD->getType());
  } else {
    assert(false && "unknown decl");
    __builtin_unreachable();
  }
}

///
/// AsmIdentifierList
///
std::vector<AsmIdentifier *> AsmIdentifierList::getIdentifiers() {
  std::vector<AsmIdentifier *> Ids;
  for (auto &I : this->Ids)
    Ids.emplace_back(I.get());
  return Ids;
}

std::vector<const AsmIdentifier *> AsmIdentifierList::getIdentifiers() const {
  std::vector<const AsmIdentifier *> Ids;
  for (auto &I : this->Ids)
    Ids.emplace_back(I.get());
  return Ids;
}

} // namespace soll
