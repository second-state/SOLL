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
  AsmForStmt(BlockPtr &&Init, ExprPtr &&Cond, BlockPtr &&Loop, BlockPtr &&Body)
      : Init(std::move(Init)), Cond(std::move(Cond)), Loop(std::move(Loop)),
        Body(std::move(Body)) {}

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
  std::unique_ptr<AsmSwitchCase> NextCase;

public:
  AsmSwitchCase(BlockPtr &&SubStmt)
      : SubStmt(std::move(SubStmt)), NextCase(nullptr) {}

  void setNextCase(std::unique_ptr<AsmSwitchCase> &&SC) {
    NextCase = std::move(SC);
  }
  Block *getSubStmt() { return SubStmt.get(); }
  const Block *getSubStmt() const { return SubStmt.get(); }
  AsmSwitchCase *getNextCase() { return NextCase.get(); }
  const AsmSwitchCase *getNextCase() const { return NextCase.get(); }
};

class AsmCaseStmt final : public AsmSwitchCase {
  ExprPtr LHS;

public:
  inline AsmCaseStmt(ExprPtr &&LHS, BlockPtr &&SubStmt);

  Expr *getLHS() { return LHS.get(); }
  const Expr *getLHS() const { return LHS.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmDefaultStmt final : public AsmSwitchCase {
public:
  AsmDefaultStmt(BlockPtr &&SubStmt) : AsmSwitchCase(std::move(SubStmt)) {}

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmSwitchStmt final : public Stmt {
  ExprPtr Cond;
  std::unique_ptr<AsmSwitchCase> FirstCase;

public:
  AsmSwitchStmt(ExprPtr &&Cond, std::unique_ptr<AsmSwitchCase> &&FirstCase)
      : Cond(std::move(Cond)), FirstCase(std::move(FirstCase)) {}

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  AsmSwitchCase *getFirstCase() { return FirstCase.get(); }
  const AsmSwitchCase *getFirstCase() const { return FirstCase.get(); }
  std::list<AsmSwitchCase *> getSwitchCaseList();
  std::list<const AsmSwitchCase *> getSwitchCaseList() const;
  void addSwitchCase(std::unique_ptr<AsmSwitchCase> &&SC);

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmAssignmentStmt : public Stmt {
  std::unique_ptr<AsmIdentifierList> LHS;
  ExprPtr RHS;

public:
  AsmAssignmentStmt(std::unique_ptr<AsmIdentifierList> &&LHS, ExprPtr &&RHS)
      : LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  AsmIdentifierList *getLHS() { return LHS.get(); }
  const AsmIdentifierList *getLHS() const { return LHS.get(); }
  Expr *getRHS() { return RHS.get(); }
  const Expr *getRHS() const { return RHS.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class AsmFunctionDeclStmt : public Stmt {
  std::string Name;
  std::unique_ptr<ParamList> Params;
  std::unique_ptr<ParamList> ReturnParams;
  std::unique_ptr<Block> Body;
  bool Implemented;
  TypePtr FuncTy;

public:
  AsmFunctionDeclStmt(llvm::StringRef name, std::unique_ptr<ParamList> &&params,
                      std::unique_ptr<ParamList> &&returnParams,
                      std::unique_ptr<Block> &&body);

  ParamList *getParams() { return Params.get(); }
  const ParamList *getParams() const { return Params.get(); }

  ParamList *getReturnParams() { return ReturnParams.get(); }
  const ParamList *getReturnParams() const { return ReturnParams.get(); }

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }
  void setBody(std::unique_ptr<Block> &&B) { Body = std::move(B); }
  TypePtr getType() const { return FuncTy; }
  llvm::StringRef getName() const { return Name; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll

#include "ExprAsm.h"
soll::AsmCaseStmt::AsmCaseStmt(ExprPtr &&LHS, BlockPtr &&SubStmt)
    : AsmSwitchCase(std::move(SubStmt)), LHS(std::move(LHS)) {}
