// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/Stmt.h"
#include <list>

namespace soll {

class AsmForStmt : public Stmt {

  BlockPtr Init;
  ExprPtr Cond;
  BlockPtr Loop;
  BlockPtr Body;

public:
  AsmForStmt(SourceRange L, BlockPtr &&Init, ExprPtr &&Cond, BlockPtr &&Loop,
             BlockPtr &&Body)
      : Stmt(L), Init(std::move(Init)), Cond(std::move(Cond)),
        Loop(std::move(Loop)), Body(std::move(Body)) {}

  Block *getInit() { return Init.get(); }
  const Block *getInit() const { return Init.get(); }
  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  Block *getLoop() { return Loop.get(); }
  const Block *getLoop() const { return Loop.get(); }
  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmSwitchCase : public Stmt {
  BlockPtr SubStmt;

public:
  AsmSwitchCase(SourceRange L, BlockPtr &&SubStmt)
      : Stmt(L), SubStmt(std::move(SubStmt)) {}

  Block *getSubStmt() { return SubStmt.get(); }
  const Block *getSubStmt() const { return SubStmt.get(); }
};

class AsmCaseStmt final : public AsmSwitchCase {
  ExprPtr LHS;

public:
  inline AsmCaseStmt(SourceRange L, ExprPtr &&LHS, BlockPtr &&SubStmt);

  Expr *getLHS() { return LHS.get(); }
  const Expr *getLHS() const { return LHS.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmDefaultStmt final : public AsmSwitchCase {
public:
  AsmDefaultStmt(SourceRange L, BlockPtr &&SubStmt)
      : AsmSwitchCase(L, std::move(SubStmt)) {}

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmSwitchStmt final : public Stmt {
  ExprPtr Cond;
  std::vector<std::unique_ptr<AsmSwitchCase>> Cases;

public:
  AsmSwitchStmt(SourceRange L, ExprPtr &&Cond,
                std::vector<std::unique_ptr<AsmSwitchCase>> &&Cases)
      : Stmt(L), Cond(std::move(Cond)), Cases(std::move(Cases)) {}

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  std::vector<AsmSwitchCase *> getCases();
  std::vector<const AsmSwitchCase *> getCases() const;

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmAssignmentStmt : public Stmt {
  std::unique_ptr<AsmIdentifierList> LHS;
  ExprPtr RHS;

public:
  AsmAssignmentStmt(SourceRange L, std::unique_ptr<AsmIdentifierList> &&LHS,
                    ExprPtr &&RHS)
      : Stmt(L), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  AsmIdentifierList *getLHS() { return LHS.get(); }
  const AsmIdentifierList *getLHS() const { return LHS.get(); }
  Expr *getRHS() { return RHS.get(); }
  const Expr *getRHS() const { return RHS.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmFunctionDeclStmt : public Stmt {
  std::unique_ptr<AsmFunctionDecl> FuncDecl;

public:
  AsmFunctionDeclStmt(SourceRange L, std::unique_ptr<AsmFunctionDecl> &&FD)
      : Stmt(L), FuncDecl(std::move(FD)) {}

  AsmFunctionDecl *getDecl() { return FuncDecl.get(); }
  const AsmFunctionDecl *getDecl() const { return FuncDecl.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

} // namespace soll

#include "DeclAsm.h"
#include "ExprAsm.h"
soll::AsmCaseStmt::AsmCaseStmt(SourceRange L, ExprPtr &&LHS, BlockPtr &&SubStmt)
    : AsmSwitchCase(L, std::move(SubStmt)), LHS(std::move(LHS)) {}
