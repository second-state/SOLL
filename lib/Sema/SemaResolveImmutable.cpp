#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/AST/StmtVisitor.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <functional>
#include <llvm/ADT/StringRef.h>

namespace soll {
class DeclImmutableResolver;

class ImmutableResolver : public StmtVisitor {
  Sema &Actions;
  DeclImmutableResolver &DIR;

public:
  ImmutableResolver(Sema &Actions, DeclImmutableResolver &DIR)
      : Actions(Actions), DIR(DIR) {}
  // void visit(EmitStmtType &) override;
  // void visit(IfStmtType &) override;
  // void visit(WhileStmtType &W) override;
  // void visit(ForStmtType &F) override;
  // void visit(ContinueStmtType &) override;
  // void visit(BreakStmtType &) override;
  // void visit(ReturnStmtType &) override;
  void visit(DeclStmtType &DS) override;
  // void visit(TupleExprType &) override;
  void visit(TypesTupleExprType &TE) override { StmtVisitor::visit(TE); }
  // void visit(UnaryOperatorType &) override;
  // void visit(BinaryOperatorType &) override;
  void visit(CallExprType &CE) override {
    StmtVisitor::visit(CE);
    auto *I = dynamic_cast<AsmIdentifier *>(CE.getCalleeExpr());
    auto Arguments = CE.getArguments();
    if (I->getSpecialIdentifier() ==
        AsmIdentifier::SpecialIdentifier::setimmutable) {
      auto *ICE0 = dynamic_cast<const ImplicitCastExpr *>(Arguments[0]);
      auto *ICE1 = dynamic_cast<const ImplicitCastExpr *>(Arguments[1]);
      if (ICE0 && ICE1) {
        auto *NL = dynamic_cast<const NumberLiteral *>(ICE0->getSubExpr());
        auto *SL = dynamic_cast<const StringLiteral *>(ICE1->getSubExpr());
        if (SL && NL) {
          std::string Name = SL->getValue();
          auto &ImmutableMap = Actions.getContext().getImmutableAddressMap();
          ImmutableMap.try_emplace(llvm::StringRef(Name), NL->getValue());
        }
      }
    }
  }
  // void visit(ImplicitCastExprType &ICE) override { StmtVisitor::visit(ICE);
  // } void visit(ExplicitCastExprType &ECE) override {
  // StmtVisitor::visit(ECE); } void visit(ParenExprType &) override; void
  // visit(MemberExprType &) override; void visit(IndexAccessType &) override;
  // void visit(IdentifierType &I) override;
  // void visit(BooleanLiteralType &) override;
  // void visit(StringLiteralType &) override;
  // void visit(NumberLiteralType &) override;
  // void visit(AsmForStmtType &S) override;
  // void visit(AsmCaseStmtType &) override;
  // void visit(AsmDefaultStmtType &) override;
  void visit(AsmSwitchStmtType &AS) override { StmtVisitor::visit(AS); }
  // void visit(AsmAssignmentStmtType &) override;
  void visit(AsmFunctionDeclStmtType &FDS) override;
  // void visit(AsmLeaveStmtType &) override;
  void visit(AsmIdentifierType &AI) override { StmtVisitor::visit(AI); }
  // void visit(AsmIdentifierListType &) override;
};

class DeclImmutableResolver : public DeclVisitor {
  ImmutableResolver IR;
  Sema &Actions;
  ContractDeclType *CurrentContract;

public:
  DeclImmutableResolver(Sema &Actions)
      : IR(Actions, *this), Actions(Actions), CurrentContract(nullptr) {}
  ContractDeclType *getCurrentContract() const { return CurrentContract; }
  void visit(SourceUnitType &SU) override {
    Sema::SemaScope SourceUnitScope{&Actions, 0};
    DeclVisitor::visit(SU);
  }
  // void visit(PragmaDirectiveType &) override;
  // void visit(UsingForType &UF)
  void visit(ContractDeclType &CD) override { DeclVisitor::visit(CD); }
  void visit(FunctionDeclType &FD) override {
    DeclVisitor::visit(FD);
    {
      if (FD.getBody())
        FD.getBody()->accept(IR);
    }
  }
  void visit(EventDeclType &ED) override { DeclVisitor::visit(ED); }
  // void visit(ParamListType &) override;
  // void visit(CallableVarDeclType &) override;
  void visit(VarDeclType &VD) override { DeclVisitor::visit(VD); }
  void visit(StructDeclType &SD) override { DeclVisitor::visit(SD); }
  // void visit(ModifierInvocationType &) override;

  void visit(YulCodeType &YC) override {
    DeclVisitor::visit(YC);
    YC.getBody()->accept(IR);
  }
  // void visit(YulDataType &) override;
  // void visit(YulObjectType &) override;
  void visit(AsmFunctionDeclType &FD) override {
    DeclVisitor::visit(FD);
    { FD.getBody()->accept(IR); }
  }
  void visit(AsmVarDeclType &VD) override {
    DeclVisitor::visit(VD);
    if (VD.getValue()) {
      VD.getValue()->accept(IR);
    }
  }
};

void ImmutableResolver::visit(DeclStmtType &DS) {
  StmtVisitor::visit(DS);
  auto VarDecls = DS.getVarDecls();
  for (auto VD : VarDecls) {
    VD->accept(DIR);
  }
  if (DS.getValue()) {
    DS.getValue()->accept(*this);
  }
}

void ImmutableResolver::visit(AsmFunctionDeclStmtType &FDS) {
  StmtVisitor::visit(FDS);
  FDS.getDecl()->accept(DIR);
}

void Sema::resolveImmutable(SourceUnit &SU) {
  DeclImmutableResolver DIR(*this);
  SU.accept(DIR);
}
} // namespace soll