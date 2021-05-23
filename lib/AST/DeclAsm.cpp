// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/DeclAsm.h"

namespace soll {

AsmFunctionDecl::AsmFunctionDecl(SourceRange L, llvm::StringRef Name,
                                 std::unique_ptr<ParamList> &&Params,
                                 std::unique_ptr<ParamList> &&ReturnParams,
                                 std::unique_ptr<Block> &&Body)
    : CallableVarDecl(L, Name, Visibility::Internal, std::move(Params),
                      std::move(ReturnParams)),
      Body(std::move(Body)) {
  std::vector<std::reference_wrapper<const TypePtr>> PTys;
  std::vector<std::reference_wrapper<const TypePtr>> RTys;
  for (auto VD : this->getParams()->getParams())
    PTys.emplace_back(std::cref(VD->getType()));
  for (auto VD : this->getReturnParams()->getParams())
    RTys.emplace_back(std::cref(VD->getType()));
  FuncTy = std::make_shared<FunctionType>(std::move(PTys), std::move(RTys));
}

} // namespace soll
