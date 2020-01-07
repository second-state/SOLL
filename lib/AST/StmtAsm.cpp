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

///
/// AsmFunctionDeclStmt
///
AsmFunctionDeclStmt::AsmFunctionDeclStmt(
    SourceRange L, llvm::StringRef name, std::unique_ptr<ParamList> &&params,
    std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body)
    : Stmt(L), Name(name), Params(std::move(params)),
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
