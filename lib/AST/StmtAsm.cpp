// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/StmtAsm.h"

namespace soll {

///
/// AsmSwitchCase
///
std::list<AsmSwitchCase *> AsmSwitchStmt::getSwitchCaseList() {
  std::list<AsmSwitchCase *> Cases;
  for (AsmSwitchCase *Case = getFirstCase(); Case; Case = Case->getNextCase()) {
    Cases.emplace_front(Case);
  }
  return Cases;
}

std::list<const AsmSwitchCase *> AsmSwitchStmt::getSwitchCaseList() const {
  std::list<const AsmSwitchCase *> Cases;
  for (const AsmSwitchCase *Case = getFirstCase(); Case;
       Case = Case->getNextCase()) {
    Cases.emplace_front(Case);
  }
  return Cases;
}

void AsmSwitchStmt::addSwitchCase(std::unique_ptr<AsmSwitchCase> &&SC) {
  assert(!SC->getNextCase() && "case/default already added to a switch");
  if (FirstCase.get()) {
    SC->setNextCase(std::move(FirstCase));
    FirstCase = std::move(SC);
  } else {
    FirstCase = std::move(SC);
  }
}

///
/// AsmFunctionDeclStmt
///
AsmFunctionDeclStmt::AsmFunctionDeclStmt(
    llvm::StringRef name, std::unique_ptr<ParamList> &&params,
    std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body)
    : Name(name), Params(std::move(params)),
      ReturnParams(std::move(returnParams)), Body(std::move(body)),
      Implemented(body != nullptr) {
  std::vector<TypePtr> PTys;
  std::vector<TypePtr> RTys;
  for (auto VD : this->getParams()->getParams())
    PTys.push_back(VD->GetType());
  for (auto VD : this->getReturnParams()->getParams())
    RTys.push_back(VD->GetType());
  FuncTy = std::make_shared<FunctionType>(std::move(PTys), std::move(RTys));
}

} // namespace soll
