// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ExprAsm.h"
#include "soll/AST/DeclAsm.h"

namespace soll {

///
/// AsmIdentifier
///
AsmIdentifier::AsmIdentifier(const std::string &Name, SpecialIdentifier D,
                             TypePtr Ty)
    : Expr(ValueKind::VK_LValue, std::move(Ty)), Name(Name), D(D) {}

AsmIdentifier::AsmIdentifier(const std::string &Name, Decl *D)
    : Expr(ValueKind::VK_LValue, nullptr), Name(Name), D(D) {
  if (auto VD = dynamic_cast<AsmVarDecl *>(D)) {
    Ty = VD->GetType();
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
