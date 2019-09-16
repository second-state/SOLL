// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Frontend/ASTConsumers.h"
#include <llvm/Support/raw_ostream.h>

namespace soll {

class FuncSigPrinter : public ASTConsumer, public ConstDeclVisitor {

  llvm::raw_ostream &Out;

public:
  FuncSigPrinter(llvm::raw_ostream &Out = llvm::outs()) : Out(Out) {}

  void HandleSourceUnit(ASTContext &Ctx, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(FunctionDeclType &) override;

  llvm::raw_ostream &os() { return Out; }
};

std::unique_ptr<ASTConsumer> CreateFuncSigPrinter(llvm::raw_ostream &Out) {
  return std::make_unique<FuncSigPrinter>(Out);
}

void FuncSigPrinter::visit(FunctionDeclType &decl) {
  // TODO
}

} // namespace soll
