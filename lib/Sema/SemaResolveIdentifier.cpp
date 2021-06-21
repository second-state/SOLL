#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <functional>

namespace soll {
class DeclIdentifierResolver;

TypePtr handleUnresolveType(Sema &Actions, UnresolveType *UT) {
  Decl *D = Actions.lookupName(UT->getIdentifierName());
  if (auto *SD = dynamic_cast<StructDecl *>(D))
    return SD->getType();
  else if (auto *CD = dynamic_cast<ContractDecl *>(D)) {
    return CD->getType();
  }
  __builtin_unreachable();
}

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
  void visit(TypesTupleExprType &TE) override {
    StmtVisitor::visit(TE);
    std::function<void(TupleType *)> handleUnresolveTupleType;
    handleUnresolveTupleType = [&](TupleType *TupleTy) {
      for (auto &Ty : TupleTy->getElementTypes()) {
        if (auto *UT = dynamic_cast<UnresolveType *>(Ty.get())) {
          Ty = handleUnresolveType(Actions, UT);
        } else if (auto TP = dynamic_cast<TupleType *>(Ty.get())) {
          handleUnresolveTupleType(TP);
        }
      }
    };
    handleUnresolveTupleType(dynamic_cast<TupleType *>(TE.getType().get()));
  }
  // void visit(UnaryOperatorType &) override;
  // void visit(BinaryOperatorType &) override;
  // void visit(CallExprType &) override;
  // void visit(ImplicitCastExprType &) override;
  // void visit(ExplicitCastExprType &) override;
  // void visit(ParenExprType &) override;
  void visit(MemberExprType &M) override;
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
    StmtVisitor::visit(AS);
  }
  // void visit(AsmAssignmentStmtType &) override;
  void visit(AsmFunctionDeclStmtType &FDS) override;
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
  void visit(SourceUnitType &SU) override {
    Sema::SemaScope SourceUnitScope{&Actions, 0};
    DeclVisitor::visit(SU);
  }
  // void visit(PragmaDirectiveType &) override;
  void visit(UsingForType &UF) {
    auto LibraryIdentifierPath = UF.getLibraryName();
    for (auto LibraryIdentifier : LibraryIdentifierPath.getPath()) {
      auto Lib = Actions.lookupContractDeclName(LibraryIdentifier);
      assert(Lib->getKind() == ContractDecl::ContractKind::Library &&
             "Not a Library!");
      UF.addLibrary(Lib);
    }
    if (auto *UT = dynamic_cast<UnresolveType *>(UF.getType().get())) {
      UF.setType(handleUnresolveType(Actions, UT));
    }
  }
  void visit(ContractDeclType &CD) override {
    Actions.addDecl(&CD);
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

        // abstract function do not have body
        if (FD.getBody())
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
      VD.setType(handleUnresolveType(Actions, UT));
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
          Ty = handleUnresolveType(Actions, UT);
        }
      }
    }
  }
  // void visit(ModifierInvocationType &) override;

  void visit(YulCodeType &YC) override {
    DeclVisitor::visit(YC);
    YC.getBody()->accept(IR);
  }
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

void IdentifierResolver::visit(AsmFunctionDeclStmtType &FDS) {
  StmtVisitor::visit(FDS);
  FDS.getDecl()->accept(DIR);
}

void IdentifierResolver::visit(MemberExprType &M) {
  // NameCheckByPass prevent raise a alert when M.name() is unsolved
  // It is usually used when M.base is special case.
  bool NameCheckByPass = false;
  bool BaseCheckByPass = false;

  auto Base = M.getBase();
  Base->accept(*this);

  {
    Sema::SemaScope MemberAccessScope{&Actions, 0, false};

    if (Base->getType() == nullptr) {
      if (auto I = dynamic_cast<Identifier *>(Base)) {
        auto BaseName = I->getName().str();
        const std::vector<std::string> GlobalPassList{"block", "msg", "tx"};

        if (std::find(GlobalPassList.begin(), GlobalPassList.end(), BaseName) !=
            GlobalPassList.end()) {
          BaseCheckByPass = true;
        } else {
          // TODO: we should handle `address` type as base.
          BaseCheckByPass = true;
        }
      }
      if (!BaseCheckByPass) {
        assert(false && "Base can not be unsolved!");
      }
      return;
    }

    switch (Base->getType()->getCategory()) {
    case Type::Category::Contract: {
      auto I = dynamic_cast<Identifier *>(Base);
      assert(I && "Contract name must be a Identifier");

      if (I->isSpecialIdentifier()) {
        switch (I->getSpecialIdentifier()) {
        case Identifier::SpecialIdentifier::this_:
          for (auto F : DIR.getCurrentContract()->getFuncs())
            Actions.addDecl(F);
          break;
        case Identifier::SpecialIdentifier::super_: {
          auto Bases = DIR.getCurrentContract()->getResolvedBaseContracts();
          if (Bases.size() > 1) {
            Actions.Diag(Base->getLocation().getBegin(),
                         diag::err_super_in_noninheritance_contract);
          }
          // Super class is at the index 1 of Bases
          for (auto F : Bases[1]->getFuncs())
            Actions.addDecl(F);
        } break;
        default:
          assert(I && "Unimplement SpecialIdentifier Base");
        }
      } else if (auto C =
                     dynamic_cast<ContractDecl *>(I->getCorrespondDecl())) {
        // Inheritance call with Contract name
        for (auto F : C->getFuncs())
          Actions.addDecl(F);
      } else if (auto CT = dynamic_cast<ContractType *>(I->getType().get())) {
        // Contract external call
        if (CT->getDecl()) {
          for (auto F : CT->getDecl()->getFuncs())
            Actions.addDecl(F);
        } else if (I->getName() == "this") {
          for (auto F : DIR.getCurrentContract()->getFuncs())
            Actions.addDecl(F);
        }
      }
    } break;
    case Type::Category::Struct: {
      if (auto ST = dynamic_cast<StructType *>(Base->getType().get())) {
        auto Types = ST->getElementTypes();
        auto ElementName = M.getName()->getName().str();

        if (ST->hasElement(ElementName)) {
          // Note: struct do not have sub-decl yet. check name only.
          M.setType(Types[ST->getElementIndex(ElementName)]);
          NameCheckByPass = true;
          return;
        } else {
          Actions.Diag(M.getName()->getLocation().getBegin(),
                       diag::err_no_member)
              << ElementName << M.getBase()->getType()->getName();
        }
      }
    } break;
    case Type::Category::Address: {
      // TODO: Handle Address case
      // TODO: Address.send(x), should bypass send and check x.
      NameCheckByPass = true;
      // XXX: Fix alert for a.send()
      return;
    } break;
    default:
      // TODO: Handle Library call
      NameCheckByPass = true;
      break;
    }
    if (!NameCheckByPass)
      M.getName()->accept(*this);
  }

  if (!NameCheckByPass && !M.getName()->isResolved()) {
    // Note: MemberExpr do not allow resolve in outside scope
    Actions.Diag(M.getName()->getLocation().getBegin(), diag::err_undeclared_var_use) << M.getName()->getName();
  }
}

void Sema::resolveIdentifierDecl(SourceUnit &SU) {
  DeclIdentifierResolver DIR(*this);
  SU.accept(DIR);
}
} // namespace soll