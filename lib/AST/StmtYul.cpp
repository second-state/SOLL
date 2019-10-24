// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/StmtYul.h"
#include "soll/AST/Decl.h"
#include "soll/AST/ExprYul.h"

namespace soll {

///
/// YulIdentifierList
///
std::vector<YulIdentifier *> YulIdentifierList::getIdentifiers() {
  std::vector<YulIdentifier *> Ids;
  for (auto &I : this->Ids)
    Ids.emplace_back(I.get());
  return Ids;
}

std::vector<const YulIdentifier *> YulIdentifierList::getIdentifiers() const {
  std::vector<const YulIdentifier *> Ids;
  for (auto &I : this->Ids)
    Ids.emplace_back(I.get());
  return Ids;
}

} // namespace soll
