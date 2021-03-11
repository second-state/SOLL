#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"

namespace soll {
class DeclIdentifierResolver;
class IdentifierResolver : public StmtVisitor {
  Sema &Actions;
  DeclIdentifierResolver &DIR;

public:
  IdentifierResolver(Sema &Actions, DeclIdentifierResolver &DIR)
      : Actions(Actions), DIR(DIR) {}
  void visit(BlockType &B) override {
    std::unique_ptr<Sema::SemaScope> BlockScope;
    if (!B.hasScope()) {
      BlockScope = std::make_unique<Sema::SemaScope>(&Actions, 0);
    }
    StmtVisitor::visit(B);
  }
  // void visit(EmitStmtType &) override;
  // void visit(IfStmtType &) override;
  void visit(WhileStmtType &W) override {
    W.getCond()->accept(*this);
    {
      Sema::SemaScope WhileScope{&Actions,
                                 Scope::BreakScope | Scope::ContinueScope};
      W.getBody()->accept(*this);
    }
  }
  void visit(ForStmtType &F) override {
    if (F.getInit())
      F.getInit()->accept(*this);
    if (F.getCond())
      F.getCond()->accept(*this);
    if (F.getLoop())
      F.getLoop()->accept(*this);
    {
      Sema::SemaScope ForScope{&Actions,
                               Scope::BreakScope | Scope::ContinueScope};
      F.getBody()->accept(*this);
    }
  }
  // void visit(ContinueStmtType &) override;
  // void visit(BreakStmtType &) override;
  // void visit(ReturnStmtType &) override;
  void visit(DeclStmtType &DS) override;
  // void visit(TupleExprType &) override;
  // void visit(UnaryOperatorType &) override;
  // void visit(BinaryOperatorType &) override;
  // void visit(CallExprType &) override;
  // void visit(ImplicitCastExprType &) override;
  // void visit(ExplicitCastExprType &) override;
  // void visit(ParenExprType &) override;
  // void visit(MemberExprType &) override;
  // void visit(IndexAccessType &) override;
  void visit(IdentifierType &I) override {
    if (I.isResolved())
      return;
    Decl *D = Actions.lookupName(I.getName());
    if (D == nullptr) {
      Actions.CurrentScope()->addUnresolvedExternal(&I);
      return;
    }
    if (auto SD = dynamic_cast<StructDecl *>(D)) {
      auto Ty = SD->getConstructorType();
      I.setType(Ty);
      I.setSpecialIdentifier(Identifier::SpecialIdentifier::struct_constructor);
      return;
    }
    I.setCorrespondDecl(D);
  }
  // void visit(BooleanLiteralType &) override;
  // void visit(StringLiteralType &) override;
  // void visit(NumberLiteralType &) override;

  void visit(AsmForStmtType &S) override {
    Sema::SemaScope ForScope{&Actions, 0};
    S.getInit()->accept(*this);
    S.getCond()->accept(*this);
    S.getLoop()->accept(*this);
    {
      Sema::SemaScope ForScope{&Actions,
                               Scope::BreakScope | Scope::ContinueScope};
      S.getBody()->accept(*this);
    }
  }
  // void visit(AsmCaseStmtType &) override;
  // void visit(AsmDefaultStmtType &) override;
  void visit(AsmSwitchStmtType &AS) override {
    Sema::SemaScope SwitchScope{&Actions, 0};
  }
  // void visit(AsmAssignmentStmtType &) override;
  void visit(AsmFunctionDeclStmtType &FDS) override {
    StmtVisitor::visit(FDS);
    if (auto *D = FDS.getDecl()) {
      if (auto *B = D->getBody()) {
        B->accept(*this);
      }
    }
  }
  // void visit(AsmLeaveStmtType &) override;
  void visit(AsmIdentifierType &AI) override {
    StmtVisitor::visit(AI);
    if (AI.isResolved())
      return;
    Decl *D = Actions.lookupName(AI.getName());
    if (D == nullptr) {
      if (AI.isCall()) {
        Actions.CurrentScope()->addUnresolved(&AI);
      } else {
        Actions.CurrentScope()->addUnresolvedExternal(&AI);
      }
      return;
    }
    AI.setCorrespondDecl(D);
  }
  // void visit(AsmIdentifierListType &) override;
};

class DeclIdentifierResolver : public DeclVisitor {
  IdentifierResolver IR;
  Sema &Actions;
  ContractDeclType *CurrentContract;

public:
  DeclIdentifierResolver(Sema &Actions)
      : IR(Actions, *this), Actions(Actions), CurrentContract(nullptr) {}
  ContractDeclType *getCurrentContract() const { return CurrentContract; }
  TypePtr handleUnresolveType(UnresolveType *UT) {
    Decl *D = Actions.lookupName(UT->getIdentifierName());
    if (auto *SD = dynamic_cast<StructDecl *>(D))
      return SD->getType();
    else if (auto *CD = dynamic_cast<ContractDecl *>(D)) {
      return CD->getType();
    }
    __builtin_unreachable();
  }
  void visit(SourceUnitType &SU) override {
    Sema::SemaScope SourceUnitScope{&Actions, 0};
    DeclVisitor::visit(SU);
  }
  // void visit(PragmaDirectiveType &) override;
  void visit(ContractDeclType &CD) override {
    auto CT = std::make_shared<ContractType>(&CD);
    CD.setType(CT);
    Actions.addDecl(&CD);
    Actions.addContractDecl(&CD);
    {
      Sema::SemaScope ContractScope{&Actions, 0};
      CurrentContract = &CD;
      DeclVisitor::visit(CD);
      CurrentContract = nullptr;
    }
  }
  void visit(FunctionDeclType &FD) override {
    Actions.addDecl(&FD);
    {
      Sema::SemaScope ArgumentScope{&Actions, 0};
      DeclVisitor::visit(FD);
      {
        Sema::SemaScope FunctionScope{&Actions, Scope::FunctionScope};
        FD.getBody()->accept(IR);
      }
    }
  }
  void visit(EventDeclType &ED) override {
    Actions.addDecl(&ED);
    DeclVisitor::visit(ED);
  }
  // void visit(ParamListType &) override;
  // void visit(CallableVarDeclType &) override;
  void visit(VarDeclType &VD) override {
    Actions.addDecl(&VD);
    // TODO: handle ArrayType of UnresolveType
    if (auto *UT = dynamic_cast<UnresolveType *>(VD.getType().get())) {
      VD.setType(handleUnresolveType(UT));
    }
    DeclVisitor::visit(VD);
    if (VD.getValue()) {
      VD.getValue()->accept(IR);
    }
  }
  void visit(StructDeclType &SD) override {
    if (Actions.lookupName(SD.getName())) {
      Actions.Diag(SD.getToken().getLocation(), diag::err_duplicate_definition)
          << SD.getName().str();
      return;
    }
    Actions.addDecl(&SD);
    DeclVisitor::visit(SD);
    if (auto STy = dynamic_cast<StructType *>(SD.getType().get())) {
      for (auto &Ty : STy->getElementTypes()) {
        if (auto *UT = dynamic_cast<UnresolveType *>(Ty.get())) {
          Ty = handleUnresolveType(UT);
        }
      }
    }
  }
  // void visit(ModifierInvocationType &) override;

  // void visit(YulCodeType &) override;
  // void visit(YulDataType &) override;
  // void visit(YulObjectType &) override;
  void visit(AsmFunctionDeclType &FD) override {
    Actions.addDecl(&FD);
    {
      Sema::SemaScope ArgumentScope{&Actions, 0};
      DeclVisitor::visit(FD);
      {
        Sema::SemaScope FunctionScope{&Actions, Scope::FunctionScope};
        FD.getBody()->accept(IR);
      }
    }
  }
  void visit(AsmVarDeclType &VD) override {
    Actions.addDecl(&VD);
    DeclVisitor::visit(VD);
    if (VD.getValue()) {
      VD.getValue()->accept(IR);
    }
  }
};

void IdentifierResolver::visit(DeclStmtType &DS) {
  StmtVisitor::visit(DS);
  auto VarDecls = DS.getVarDecls();
  for (auto VD : VarDecls) {
    VD->accept(DIR);
  }
  if (DS.getValue()) {
    DS.getValue()->accept(*this);
  }
}

void Sema::resolveIdentifierDecl(SourceUnit &SU) {
  DeclIdentifierResolver DIR(*this);
  SU.accept(DIR);
}
} // namespace soll