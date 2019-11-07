// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Stmt.h"

namespace soll {

class YulForStmt : public Stmt {
  using BlockPtr = std::unique_ptr<Block>;

  BlockPtr Init;
  ExprPtr Cond;
  BlockPtr Loop;
  BlockPtr Body;

public:
  YulForStmt(BlockPtr &&Init, ExprPtr &&Cond, BlockPtr &&Loop, BlockPtr &&Body)
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

class YulSwitchCase : public Stmt {
  ExprPtr SubExpr;
  std::unique_ptr<YulSwitchCase> NextCase;

public:
  YulSwitchCase(ExprPtr &&SubExpr)
      : SubExpr(std::move(SubExpr)), NextCase(nullptr) {}

  Expr *getSubExpr() { return SubExpr.get(); }
  const Expr *getSubExpr() const { return SubExpr.get(); }
  YulSwitchCase *getNextCase() { return NextCase.get(); }
  const YulSwitchCase *getNextCase() const { return NextCase.get(); }
};

class YulCaseStmt final : public YulSwitchCase {
  std::unique_ptr<YulLiteral> LHS;

public:
  inline YulCaseStmt(std::unique_ptr<YulLiteral> &&LHS, ExprPtr &&RHS);

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class YulDefaultStmt final : public YulSwitchCase {
public:
  YulDefaultStmt(ExprPtr &&RHS) : YulSwitchCase(std::move(RHS)) {}

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

class YulSwitchStmt final : public Stmt {
  std::unique_ptr<YulSwitchCase> FirstCase;

public:
  YulSwitchStmt(std::unique_ptr<YulSwitchCase> &&FirstCase)
      : FirstCase(std::move(FirstCase)) {}

  YulSwitchCase *getFirstCase() { return FirstCase.get(); }
  const YulSwitchCase *getFirstCase() const { return FirstCase.get(); }

  void accept(StmtVisitor &) override;
  void accept(ConstStmtVisitor &) const override;
};

} // namespace soll

#include "ExprYul.h"
soll::YulCaseStmt::YulCaseStmt(std::unique_ptr<YulLiteral> &&LHS, ExprPtr &&RHS)
    : YulSwitchCase(std::move(RHS)), LHS(std::move(LHS)) {}
