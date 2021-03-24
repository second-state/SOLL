// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <map>
#include <vector>

namespace soll {
namespace {

class InheritGraphSolver {
  Sema &Action;
  using NodeType = ContractDecl *;
  std::vector<NodeType> ToplogicOrderContracts;

  std::map<NodeType, int> Indegree;
  std::map<NodeType, std::vector<NodeType>> Bases;
  std::map<NodeType, std::vector<NodeType>> ResolvedBaseList;

  class MergeHelper {
    std::vector<std::vector<NodeType>> CandidateList;
    std::vector<size_t> Idx;
    std::vector<NodeType> Result;

    bool isEmpty() {
      for (size_t I = 0; I < Idx.size(); ++I) {
        if (Idx[I] < CandidateList[I].size())
          return false;
      }
      return true;
    }

    bool findNext(NodeType &Next) {
      // TODO: It can be done in  O(n)
      for (size_t I = 0; I < Idx.size(); ++I) {
        if (Idx[I] == CandidateList[I].size())
          continue;

        Next = CandidateList[I][Idx[I]];

        bool Reject = false;
        for (size_t I = 0; I < Idx.size(); ++I) {
          if (Idx[I] == CandidateList[I].size())
            continue;
          if (std::count(CandidateList[I].begin() + Idx[I] + 1,
                         CandidateList[I].end(), Next) > 0)
            Reject = true;
        }
        if (!Reject)
          return true;
      }
      return false;
    }

  public:
    MergeHelper() {}

    std::vector<NodeType> getResult() const { return Result; }

    void join(const std::vector<NodeType> &list) {
      CandidateList.emplace_back(list);
      Idx.push_back(0);
    }

    bool merge() {
      while (!isEmpty()) {
        NodeType Next;

        if (!findNext(Next)) {
          return false;
        }

        for (size_t I = 0; I < Idx.size(); ++I) {
          if (Idx[I] < CandidateList[I].size() &&
              CandidateList[I][Idx[I]] == Next)
            Idx[I]++;
        }

        Result.emplace_back(Next);
      }
      return true;
    }
  };

public:
  InheritGraphSolver(Sema &S) : Action(S) {}
  void addDirectedEdge(NodeType ChildContName, NodeType BaseContName) {
    Bases[ChildContName].emplace_back(BaseContName);
    Indegree[BaseContName] += 1;
    // This ensure that Indegree[childContName] exists during solve().
    Indegree[ChildContName] += 0;
  }

  // C3 linearization algorithm
  bool generator(NodeType Cont) {
    // already solved
    if (ResolvedBaseList.find(Cont) != ResolvedBaseList.end())
      return true;

    ResolvedBaseList[Cont] = {Cont};

    MergeHelper Mh;
    for (auto Base : Bases[Cont]) {
      if (!generator(Base)) {
        return false;
      }
      Mh.join(ResolvedBaseList[Base]);
    }
    Mh.join(Bases[Cont]);

    if (!Mh.merge()) {
      Action.Diag(Cont->getLocation().getBegin(), diag::err_c3_algorithm_fail)
          << Cont->getName();
      return false;
    }

    for (auto Base : Mh.getResult())
      ResolvedBaseList[Cont].emplace_back(Base);

    ToplogicOrderContracts.emplace_back(Cont);
    return true;
  };

  bool solve() {
    bool Status = true;
    ToplogicOrderContracts.clear();

    for (auto &[Cont, Deg] : Indegree) {
      if (Deg != 0)
        continue;
      Status &= generator(Cont);
    }

    return Status;
  }

  std::vector<NodeType> getResolvedBaseList(NodeType Cont) const {
    auto Res = ResolvedBaseList.find(Cont);
    if (Res == ResolvedBaseList.end())
      return {};
    return Res->second;
  }

  std::vector<NodeType> getToplogicOrderContracts() const {
    return ToplogicOrderContracts;
  }
};

class DeclInheritResolver : public DeclVisitor {
  std::map<llvm::StringRef, ContractDeclType *> Str2Cont;

public:
  DeclInheritResolver() {}

  void visit(ContractDeclType &CD) override {
    Str2Cont.emplace(CD.getName(), &CD);
  }

  std::map<llvm::StringRef, ContractDeclType *> getStr2Cont() const {
    return Str2Cont;
  }
};

class DeclInheritResolverPost : public DeclVisitor {
  std::map<llvm::StringRef, ContractDeclType *> ContMap;
  InheritGraphSolver &IGS;

public:
  DeclInheritResolverPost(
      const std::map<llvm::StringRef, ContractDeclType *> &ContMap,
      InheritGraphSolver &I)
      : ContMap(ContMap), IGS(I) {}

  void visit(ContractDeclType &CD) override {
    auto Bases = CD.getBaseContracts();
    // the given bases are searched from right to left (left to right in Python)
    std::reverse(Bases.begin(), Bases.end());
    for (auto &Base : Bases) {
      // TODO : Diag it
      assert(ContMap.find(Base->getBaseName()) != ContMap.end());
      Base->setBaseCont(ContMap[Base->getBaseName()]);
      IGS.addDirectedEdge(&CD, Base->getBaseCont());
    }
  }
};

class DeclContractKindChecker : public DeclVisitor {
  Sema &Actions;

public:
  DeclContractKindChecker(Sema &S) : Actions(S) {}

  void checkInterface(ContractDeclType &CD) {
    for (auto Func : CD.getFuncs())
      Func->markVirtual();
    // They cannot inherit from other contracts, but they can inherit from other
    // interfaces.
    for (auto Base : CD.getBaseContracts()) {
      if (Base->getBaseCont()->getKind() !=
          ContractDecl::ContractKind::Interface) {
        Actions.Diag(CD.getLocation().getBegin(),
                     diag::err_interface_inherit_from_other_contracts)
            << Base->getBaseName();
      }
    }

    // all functions should be external
    for (auto Func : CD.getFuncs()) {
      if (Func->getVisibility() != Decl::Visibility::External) {
        Actions.Diag(Func->getLocation().getBegin(),
                     diag::err_interface_function_is_not_an_external)
            << Func->getName();
      }
    }

    // cannot declare a constructor.
    if (CD.getConstructor() != nullptr) {
      Actions.Diag(CD.getLocation().getBegin(),
                   diag::err_interface_constructor_is_not_allowed)
          << CD.getName();
    }

    // cannot declare state variables
    for (auto Var : CD.getVars()) {
      Actions.Diag(Var->getLocation().getBegin(),
                   diag::err_interface_state_variables_is_not_allowed)
          << Var->getName() << CD.getName();
    }
  }

  void visit(ContractDeclType &CD) override {
    if (CD.getKind() == ContractDeclType::ContractKind::Interface)
      checkInterface(CD);

    for (auto ASTNode : CD.getSubNodes())
      ASTNode->accept(*this);
  }

  void visit(FunctionDeclType &FD) override {
    if (FD.isVirtual()) {
      if (FD.getVisibility() == Decl::Visibility::Private) {
        Actions.Diag(FD.getLocation().getBegin(),
                     diag::err_private_function_can_not_be_virtual)
            << FD.getName();
      }
    }
  }
};

class InheritStatusVarResolver : public StmtVisitor {
  ContractDecl *Cont;
  std::map<llvm::StringRef, Decl *> VarDecls;

public:
  InheritStatusVarResolver(ContractDecl *CD) : Cont(CD) {
    for (auto Var : Cont->getVars())
      VarDecls[Var->getName()] = Var;
    for (auto Func : Cont->getFuncs())
      VarDecls[Func->getName()] = Func;
  }

  void visit(Identifier &I) override {
    if (I.getType() == nullptr) {
      auto VarPtr = VarDecls.find(I.getName());
      if (VarPtr != VarDecls.end()) {
        I.setCorrespondDecl(VarPtr->second);
      }
    }
  }
};
} // namespace

void Sema::InjectInheritContract(std::vector<Decl *> &BaseDecl,
                                 const std::vector<Decl *> &Childs,
                                 bool AppendChild) {

  for (auto ChildDecl : Childs) {
    auto ParentDeclIt = std::find_if(
        BaseDecl.begin(), BaseDecl.end(), [ChildDecl](const Decl *Node) {
          return Node->getName() == ChildDecl->getName();
        });

    if (ParentDeclIt == BaseDecl.end()) {
      if (AppendChild)
        BaseDecl.emplace_back(ChildDecl);
      continue;
    }

    if (auto ParentDecl = dynamic_cast<FunctionDecl *>(*ParentDeclIt)) {
      // check cDecl is marked override
      // TODO: function can be overrided by status var....
      if (auto cFDecl = dynamic_cast<FunctionDecl *>(ChildDecl)) {
        if (cFDecl->getOverrideSpecifier() == nullptr) {
          Diag(cFDecl->getLocation().getBegin(),
               diag::err_decl_need_to_be_overrided)
              << ParentDecl->getName();
        } else {
          // TODO: check compatiblity for both cont
        }
      } else {
        Diag(cFDecl->getLocation().getBegin(),
             diag::err_func_to_var_overrided_unsoupport);
      }

      // check ParentDecl is virtual
      if (!ParentDecl->isVirtual()) {
        Diag(ParentDecl->getLocation().getBegin(),
             diag::err_decl_need_to_be_virtual)
            << ParentDecl->getName();
      }

      // check visibility
      // only external to public is allowed
      if (!(ParentDecl->getVisibility() == ChildDecl->getVisibility() ||
            (ParentDecl->getVisibility() == Decl::Visibility::External &&
             ChildDecl->getVisibility() == Decl::Visibility::Public))) {
        Diag(ChildDecl->getLocation().getBegin(),
             diag::err_visibility_not_match)
            << ParentDecl->getName();
      }

      // check mutability
      {
        bool Allowed = false;
        auto ChildFuncDecl = dynamic_cast<FunctionDecl *>(ChildDecl);
        assert(ChildFuncDecl);
        switch (ParentDecl->getStateMutability()) {
        case StateMutability::NonPayable:
          Allowed |= ChildFuncDecl->getStateMutability() ==
                     StateMutability::NonPayable;
          [[fallthrough]];
        case StateMutability::View:
          Allowed |=
              ChildFuncDecl->getStateMutability() == StateMutability::View;
          [[fallthrough]];
        case StateMutability::Pure:
          Allowed |=
              ChildFuncDecl->getStateMutability() == StateMutability::Pure;
          break;
        case StateMutability::Payable:
          Allowed |=
              ChildFuncDecl->getStateMutability() == StateMutability::Payable;
          break;
        default:
          __builtin_unreachable();
        }
        if (!Allowed) {
          Diag(ChildDecl->getLocation().getBegin(),
               diag::err_mutability_overriding_not_allow)
              << ChildDecl->getName();
        }
      }

      BaseDecl.erase(ParentDeclIt);
    } else if (auto pDecl = dynamic_cast<VarDecl *>(*ParentDeclIt)) {
      (void)!pDecl; // silence compiler warning
      Diag(ChildDecl->getLocation().getBegin(),
           diag::err_statevar_cannot_be_overrided)
          << ChildDecl->getName();
    }

    if (AppendChild)
      BaseDecl.emplace_back(ChildDecl);
  }

  // todo: the order of the Decl is important?
}

void Sema::resolveInherit(SourceUnit &SU) {
  InheritGraphSolver IGS(*this);
  DeclInheritResolver DIR;
  SU.accept(DIR);

  DeclInheritResolverPost DIRP(DIR.getStr2Cont(), IGS);
  SU.accept(DIRP);

  if (!IGS.solve()) {
    Diag(SU.getLocation().getBegin(), diag::err_resolve_inherit_fail);
    return;
  }

  // check before inject
  DeclContractKindChecker DCC(*this);
  SU.accept(DCC);

  // inject elements
  for (auto cont : IGS.getToplogicOrderContracts()) {
    auto LList = IGS.getResolvedBaseList(cont);
    cont->setResolvedBaseContracts(LList);
    std::vector<Decl *> DeclResult;
    // TODO: C++20 IGS.getResolvedBaseList(cont) | views::reverse
    // It is form base to child
    std::reverse(LList.begin(), LList.end());
    for (auto base : LList) {
      bool appendData = (cont != base);
      InjectInheritContract(DeclResult, base->getSubNodes(), appendData);
    }
    cont->setInheritNodes(std::move(DeclResult));
    InheritStatusVarResolver ISVR(cont);
    for (auto func : cont->getFuncs())
      if (func->getBody())
        func->getBody()->accept(ISVR);
  }

  // check after inject
}

} // namespace soll
