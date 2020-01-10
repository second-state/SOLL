// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/StmtAsm.h"

namespace soll {

///
/// AsmSwitchCase
///
std::vector<AsmSwitchCase *> AsmSwitchStmt::getCases() {
  std::vector<AsmSwitchCase *> Cases;
  for (auto &C : this->Cases)
    Cases.emplace_back(C.get());
  return Cases;
}

std::vector<const AsmSwitchCase *> AsmSwitchStmt::getCases() const {
  std::vector<const AsmSwitchCase *> Cases;
  for (const auto &C : this->Cases)
    Cases.emplace_back(C.get());
  return Cases;
}

} // namespace soll
