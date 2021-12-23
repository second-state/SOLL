#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/DeclarationContainer.h"
#include "soll/Sema/GlobalContent.h"
#include "soll/Sema/NameAndTypeResolver.h"
#include "soll/Sema/ReferenceResolver.h"
#include "soll/Sema/Sema.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace soll {

class DeclarationRegistrationHelper : public DeclVisitor, public StmtVisitor {
public:
  DeclarationRegistrationHelper(
      std::map<const ASTNode *, std::shared_ptr<DeclarationContainer>> &Scopes,
      Decl *ASTRoot, Sema &Action, GlobalContext &GC,
      const ASTNode *CurrentScope = nullptr)
      : Scopes(Scopes), Action(Action), GC(GC), CurrentScope(nullptr),
        CurrentFunc(nullptr), CurrentCont(nullptr) {
    ASTRoot->accept(*this);
  }

  void enterNewSubScope(ASTNode *SubScope) {
    if (Scopes.count(SubScope)) {
      auto D = dynamic_cast<Decl *>(SubScope);
      assert(D && dynamic_cast<SourceUnit *>(D) && "Unexpected scope type.");
    } else {
      bool NewlyAdded =
          Scopes
              .emplace(SubScope, std::make_shared<DeclarationContainer>(
                                     CurrentScope, Scopes[CurrentScope].get()))
              .second;
      assert(NewlyAdded && "Unable to add new scope.");
    }
    CurrentScope = SubScope;
  }

  void closeCurrentScope() {
    assert(CurrentScope && Scopes.count(CurrentScope) &&
           "Closed non-existing scope.");
    CurrentScope = Scopes[CurrentScope]->enclosingNode();
  }

  void registerDeclaration(Decl *D) {
    assert(D);
    assert(CurrentScope && Scopes.count(CurrentScope));

    bool Inactive = false;

    if (auto ST = dynamic_cast<const Stmt *>(CurrentScope)) {
      if (dynamic_cast<const Block *>(ST) || dynamic_cast<const ForStmt *>(ST))
        Inactive = false;
    }

    registerDeclaration(Scopes[CurrentScope].get(), D, llvm::StringRef(),
                        Inactive);
    assert(D->scope() == CurrentScope);
  }

  bool registerDeclaration(DeclarationContainer *DeclCont, const Decl *D,
                           llvm::StringRef Name_, bool Inactive) {
    llvm::StringRef Name = Name_.empty() ? D->getName() : Name_;

    // We use "invisible" for both inactive variables in blocks and for members
    // invisible in contracts. They cannot both be true at the same time.
    assert(!(Inactive && D->getVisibility() != Decl::Visibility::External));

    static std::set<std::string> IllegalNames{"_", "super", "this"};

    if (IllegalNames.count(Name.str())) {
      Action.Diag(D->getLocation().getBegin(), diag::err_use_illegalnames)
          << Name;
    }

    if (!DeclCont->registerDeclaration(D, Name, &D->getLocation(), Inactive,
                                       false)) {
      // Bypass to next pass
      // TODO: Action.Diag(D->getLocation().getBegin(),
      // diag::err_already_declared) << Name;
      return false;
    }

    return true;
  }

  void visit(SourceUnit &SU) override {
    if (!Scopes[&SU])
      // By importing, it is possible that the container already exists.
      Scopes[&SU] = std::make_shared<DeclarationContainer>(
          CurrentScope, Scopes[CurrentScope].get());
    DeclVisitor::visit(SU);
  }

  // void visit(ImportDirective &ID) override;

  void visit(ContractDecl &CD) override {
    GC.setCurrentContract(&CD);

    Scopes[nullptr]->registerDeclaration(GC.currentThis(), false, true);
    // Scopes[nullptr]->registerDeclaration(GC.currentSuper(), false, true);
    CurrentCont = &CD;

    enterNewSubScope(&CD);
    DeclVisitor::visit(CD);
    closeCurrentScope();

    Scopes[nullptr]->registerDeclaration(GC.currentThis(), true, true);
    // Scopes[nullptr]->registerDeclaration(GC.currentSuper(), true, true);
    CurrentCont = nullptr;

    GC.resetCurrentContract();
  }

  void visit(VarDecl &VD) override {
    registerDeclaration(&VD);
    DeclVisitor::visit(VD);
  }

  void visit(FunctionDecl &FD) override {
    registerDeclaration(&FD);
    enterNewSubScope(&FD);
    CurrentFunc = &FD;

    DeclVisitor::visit(FD);
    if (FD.getBody()) {
      FD.getBody()->accept(*this);
    }

    CurrentFunc = nullptr;
    closeCurrentScope();
  }

  void visit(EventDeclType &ED) override {
    registerDeclaration(&ED);
    enterNewSubScope(&ED);

    DeclVisitor::visit(ED);

    closeCurrentScope();
  }

  void visit(StructDecl &SD) override { registerDeclaration(&SD); }

  // StmtVisitor Part

  void visit(Block &B) override {
    enterNewSubScope(&B);
    StmtVisitor::visit(B);
    closeCurrentScope();
  }

  void visit(ForStmt &B) override {
    enterNewSubScope(&B);
    StmtVisitor::visit(B);
    closeCurrentScope();
  }

  void visit(DeclStmt &DS) override {
    StmtVisitor::visit(DS);
    for (auto Decl : DS.getVarDecls()) {
      registerDeclaration(Decl);
    }
  }

private:
  std::map<const ASTNode *, std::shared_ptr<DeclarationContainer>> &Scopes;
  Sema &Action;
  GlobalContext &GC;
  const ASTNode *CurrentScope;
  FunctionDecl *CurrentFunc;
  ContractDecl *CurrentCont;
};

NameAndTypeResolver::NameAndTypeResolver(Sema &Action, GlobalContext &GC)
    : Action(Action), GC(GC) {
  Scopes[nullptr] = std::make_shared<DeclarationContainer>();
  // TODO
  // inject GC.decl to Scopes
}

void NameAndTypeResolver::Register(SourceUnit &SU) {
  DeclarationRegistrationHelper DRH(Scopes, &SU, Action, GC);
}

bool NameAndTypeResolver::Resolve(SourceUnit &SU) {
  for (auto Node : SU.getNodes()) {
    setScope(&SU);
    if (!resolveNamesAndTypesInternal(Node, true))
      return false;
  }

  return true;
}

bool NameAndTypeResolver::resolveNamesAndTypesInternal(ASTNode *Node,
                                                       bool ResolveCode) {
  if (auto Cont = dynamic_cast<ContractDecl *>(Node)) {
    bool Succ = true;
    setScope(Cont->scope());

    assert(!!CurrentScope);
    assert(ResolveCode);

    GC.setCurrentContract(Cont);
    if (Cont->getKind() != ContractDecl::ContractKind::Library) {
      // TODO : Super
      // updateDeclaration(GC.currentSuper());
    }
    updateDeclaration(GC.currentThis());

    for (auto Base : Cont->getInheritNodes())
      if (!resolveNamesAndTypesInternal(Base, true))
        Succ = false;

    setScope(Cont);

    // TODO : Handle  InheritNodes Scope

    // these can contain code, only resolve parameters for now
    for (auto SubNode : Cont->getSubNodes()) {
      setScope(Cont);
      if (!resolveNamesAndTypesInternal(SubNode, false))
        Succ = false;
    }

    if (!Succ)
      return false;
    setScope(Cont);

    // now resolve references inside the code
    for (auto SubNode : Cont->getSubNodes()) {
      setScope(Cont);
      if (!resolveNamesAndTypesInternal(SubNode, true))
        Succ = false;
    }

    // make "this" and "super" invisible.
    Scopes[nullptr]->registerDeclaration(GC.currentThis(), true, true);
    // Scopes[nullptr]->registerDeclaration(GC.currentSuper(), true, true);
    GC.resetCurrentContract();

    return Succ;

  } else {
    if (Scopes.count(Node))
      setScope(Node);
    return ReferencesResolver(Action, *this, ResolveCode).resolve(Node);
  }
}

std::vector<const Decl *>
NameAndTypeResolver::resolveName(llvm::StringRef Name,
                                 const ASTNode *Scope) const {
  auto It = Scopes.find(Scope);
  if (It == std::end(Scopes))
    return {};
  return It->second->resolveName(Name, false);
}

std::vector<const Decl *>
NameAndTypeResolver::nameFromCurrentScope(llvm::StringRef Name,
                                          bool IncludeInvisibles) const {
  if (CurrentScope)
    return CurrentScope->resolveName(Name, true, IncludeInvisibles);
  return {};
}

void Sema::registerDeclarations(SourceUnit &SU) {
  if (NTR == nullptr)
    NTR = std::make_shared<NameAndTypeResolver>(*this, GC);
  NTR->Register(SU);
}

void Sema::resolveNameAndType(SourceUnit &SU) {
  if (NTR == nullptr)
    NTR = std::make_shared<NameAndTypeResolver>(*this, GC);
  NTR->Resolve(SU);
}
} // namespace soll
