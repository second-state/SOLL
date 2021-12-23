// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/DeclarationContainer.h"
#include "soll/Sema/NameAndTypeResolver.h"
#include "soll/Sema/Sema.h"

namespace soll {
class ReferencesResolver : public DeclVisitor, public StmtVisitor {
public:
  ReferencesResolver(Sema &Action, NameAndTypeResolver &NTR,
                     bool ResolveInsideCode);
  bool resolve(ASTNode *Node) {
    Status = true;
    if (auto D = dynamic_cast<Decl *>(Node))
      D->accept(*this);
    else if (auto S = dynamic_cast<Stmt *>(Node))
      S->accept(*this);
    return Status;
  }
  void visit(Block &B) override;
  void visit(ForStmt &F) override;
  void visit(DeclStmt &D) override;
  void visit(Identifier &I) override;
  void visit(MemberExpr &M) override;

  void visit(FunctionDecl &FD) override;

private:
  Sema &Action;
  NameAndTypeResolver &Resolver;
  bool ResolveInsideCode;
  bool Status;
};

} // namespace soll