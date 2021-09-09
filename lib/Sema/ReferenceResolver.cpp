// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/ReferenceResolver.h"

namespace soll {
ReferencesResolver::ReferencesResolver(Sema &Action, NameAndTypeResolver &NTR,
                                       bool ResolveInsideCode = false)
    : Action(Action), Resolver(NTR), ResolveInsideCode(ResolveInsideCode) {}

void ReferencesResolver::visit(Block &B) {
  if (!ResolveInsideCode)
    return;
  Resolver.setScope(&B);
  StmtVisitor::visit(B);
  Resolver.setScope(&B);
}

void ReferencesResolver::visit(ForStmt &F) {
  if (!ResolveInsideCode)
    return;
  Resolver.setScope(&F);
  StmtVisitor::visit(F);
  Resolver.setScope(&F);
}

void ReferencesResolver::visit(DeclStmt &D) {
  StmtVisitor::visit(D);

  if (!ResolveInsideCode)
    return;

  for (auto Decl : D.getVarDecls())
    Resolver.activateVariable(Decl->getName());
}

void ReferencesResolver::visit(MemberExpr &M) {
  M.getBase()->accept(*this);
  // XXX : We need a new pass to check if the member is availible.
  //       Before we finish that, we should ignore right side.
}

void ReferencesResolver::visit(Identifier &I) {
  auto Decls = Resolver.nameFromCurrentScope(I.getName());
  if (Decls.empty()) {
    // Bypass to next pass
    // TODO: Action.Diag(I.getLocation().getBegin(),
    // diag::err_declaration_not_found) << I.getName();
  } else if (Decls.size() == 1) {
    I.setCorrespondDecl(const_cast<Decl *>(Decls.front()));
  } else {
    // Can not be resolved yet.
  }
  return;
}

void ReferencesResolver::visit(FunctionDecl &FD) {
  DeclVisitor::visit(FD);

  if (FD.getBody())
    FD.getBody()->accept(*this);
}
} // namespace soll
