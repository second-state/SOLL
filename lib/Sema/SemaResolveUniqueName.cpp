// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Sema/Sema.h"
#include <vector>

namespace soll {
class DeclUniqueNameResolver : public DeclVisitor {
  std::vector<Decl *> Stk;

public:
  DeclUniqueNameResolver() {}
  void visit(SourceUnitType &SU) override {
    SU.setUniqueName(
        "solidity"); // TODO: need different names for multi-sources
    Stk.emplace_back(&SU);
    DeclVisitor::visit(SU);
    Stk.pop_back();
  }
  void visit(ContractDeclType &CD) override {
    CD.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     CD.getName().str());
    Stk.emplace_back(&CD);
    DeclVisitor::visit(CD);
    Stk.pop_back();
    CD.resolveLLVMFuncName();
  }
  void visit(FunctionDeclType &FD) override {
    FD.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     FD.getName().str());
    Stk.emplace_back(&FD);
    DeclVisitor::visit(FD);
    Stk.pop_back();
  }
  void visit(EventDeclType &ED) override {
    ED.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     ED.getName().str());
    Stk.emplace_back(&ED);
    DeclVisitor::visit(ED);
    Stk.pop_back();
  }
  void visit(CallableVarDeclType &CD) override {
    CD.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     CD.getName().str());
    Stk.emplace_back(&CD);
    DeclVisitor::visit(CD);
    Stk.pop_back();
  }
  void visit(VarDeclType &VD) override {
    VD.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     VD.getName().str());
  }
  void visit(StructDeclType &SD) override {
    SD.setUniqueName(Stk.back()->getUniqueName().str() + "." +
                     SD.getName().str());
  }
};
void Sema::resolveUniqueName(SourceUnit &SU) {
  DeclUniqueNameResolver DUR;
  SU.accept(DUR);
}
} // namespace soll