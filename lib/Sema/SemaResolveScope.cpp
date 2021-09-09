// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"

#include <vector>

namespace soll {
namespace {
class ScopeResolver : public DeclVisitor, public StmtVisitor {
  struct ScopeOpenHelper {
    explicit ScopeOpenHelper(ASTNode &Node) { Scopes.emplace_back(&Node); }
    ~ScopeOpenHelper() { Scopes.pop_back(); }
    inline static std::vector<const ASTNode *> Scopes;
  };

  const ASTNode *currentScope() {
    return ScopeOpenHelper::Scopes.empty() ? nullptr
                                           : ScopeOpenHelper::Scopes.back();
  }

  void SetScope(ASTNode *A) {
    if (auto D = dynamic_cast<Decl *>(A)) {
      D->setScope(currentScope());
    }
  }

  ContractDecl *Cont;

public:
  void Resolve(SourceUnit &SU) {
    ScopeOpenHelper::Scopes.clear();
    Cont = nullptr;
    SU.accept(*this);
  }

  void visit(ContractDecl &Node) override {
    assert(Cont == nullptr);
    Cont = &Node;
    ScopeOpenHelper S(Node);
    DeclVisitor::visit(Node);

    assert(Cont == &Node);
    Cont = nullptr;
  }

  void visit(FunctionDecl &Node) override {
    SetScope(&Node);
    ScopeOpenHelper S(Node);
    DeclVisitor::visit(Node);

    if (Node.getBody())
      Node.getBody()->accept(*this);
  }

  void visit(EventDeclType &Node) override {
    SetScope(&Node);
    ScopeOpenHelper S(Node);
    DeclVisitor::visit(Node);
  }

  void visit(VarDecl &Node) override {
    SetScope(&Node);
    DeclVisitor::visit(Node);
  }

  void visit(StructDecl &Node) override {
    SetScope(&Node);
    ScopeOpenHelper S(Node);
    DeclVisitor::visit(Node);
  }

  void visit(Block &Node) override {
    SetScope(&Node);
    ScopeOpenHelper S(Node);
    StmtVisitor::visit(Node);
  }

  void visit(ForStmt &Node) override {
    SetScope(&Node);
    ScopeOpenHelper S(Node);
    StmtVisitor::visit(Node);
  }

  void visit(DeclStmt &ST) override {
    for (auto D : ST.getVarDecls())
      D->setScope(currentScope());
  }
};
} // namespace

void Sema::resolveScope(SourceUnit &SU) {
  ScopeResolver SR;
  SR.Resolve(SU);
}
} // namespace soll