// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iomanip>
#include <llvm/Support/raw_ostream.h>

namespace {

std::string ToString(const std::vector<unsigned char> &v,
                     unsigned int size = 4) {
  size = std::min(size, (unsigned int)v.size());

  std::ostringstream oss;
  for (unsigned int i = 0; i < size; ++i)
    oss << std::hex << std::setw(2) << std::setfill('0')
        << (unsigned int)(v[i]);
  return oss.str();
}

} // namespace

namespace soll {

class FuncSigPrinter : public ASTConsumer, public ConstDeclVisitor {

  llvm::raw_ostream &Out;

public:
  FuncSigPrinter(llvm::raw_ostream &Out = llvm::outs()) : Out(Out) {}

  void HandleSourceUnit(ASTContext &Ctx, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(ContractDeclType &) override;
  void visit(FunctionDeclType &) override;
  void visit(ParamListType &) override;

  llvm::raw_ostream &os() { return Out; }
};

std::unique_ptr<ASTConsumer> CreateFuncSigPrinter(llvm::raw_ostream &Out) {
  return std::make_unique<FuncSigPrinter>(Out);
}

void FuncSigPrinter::visit(ContractDeclType &C) {
  if (C.getConstructor() != nullptr) {
    C.getConstructor()->accept(*this);
  }
  if (C.getFallback() != nullptr) {
    C.getFallback()->accept(*this);
  }
  for (auto SN : C.getSubNodes()) {
    if (auto F = dynamic_cast<const FunctionDecl *>(SN)) {
      F->accept(*this);
    }
  }
}

void FuncSigPrinter::visit(FunctionDeclType &F) {
  os() << ToString(F.getSignatureHash()) << ": " << F.getName() << "(";
  F.getParams()->accept(*this);
  os() << ")\n";
}

void FuncSigPrinter::visit(ParamListType &PL) {
  bool first = true;
  for (auto param : PL.getParams()) {
    if (first)
      first = false;
    else
      os() << ",";
    os() << param->GetType()->getName();
  }
}

} // namespace soll
