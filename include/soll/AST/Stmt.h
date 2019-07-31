// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/StmtVisitor.h"
#include <llvm/ADT/APInt.h>
#include <vector>

namespace soll {

class Stmt {
public:
  virtual ~Stmt() noexcept {}

  virtual void accept(StmtVisitor &visitor) = 0;
  virtual void accept(ConstStmtVisitor &visitor) const = 0;
};

class DeclStmt : public Stmt {
  std::vector<VarDeclPtr> VarDecls;
  ExprPtr Value;

public:
  DeclStmt(std::vector<VarDeclPtr> &&VarDecls, ExprPtr &&Value)
      : VarDecls(std::move(VarDecls)), Value(std::move(Value)) {}

  std::vector<VarDecl *> getVarDecls();
  std::vector<const VarDecl *> getVarDecls() const;

  const Expr *getValue() const { return Value.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ExprStmt : public Stmt {};

class Block : public Stmt {
  std::vector<StmtPtr> Stmts;

public:
  Block(std::vector<StmtPtr> &&Stmts) : Stmts(std::move(Stmts)) {}

  /// this setter transfers the ownerships of Stmt from function argument to class instance
  void setStmts(std::vector<StmtPtr> &&Stmts);

  std::vector<Stmt *> getStmts();
  std::vector<const Stmt *> getStmts() const;

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class IfStmt : public Stmt {
  ExprPtr Cond;
  StmtPtr Then;
  StmtPtr Else; /// optional

public:
  IfStmt(ExprPtr Cond, StmtPtr Then, StmtPtr Else)
      : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setThen(StmtPtr &&Then) { this->Then = std::move(Then); }
  void setElse(StmtPtr &&Else) { this->Else = std::move(Else); }

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }

  Stmt *getThen() { return Then.get(); }
  const Stmt *getThen() const { return Then.get(); }

  Stmt *getElse() { return Else.get(); }
  const Stmt *getElse() const { return Else.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BreakableStmt : public Stmt {};

class WhileStmt : public Stmt {
  ExprPtr Cond;
  StmtPtr Body;

public:
  WhileStmt(ExprPtr Cond, StmtPtr Body) : Cond(std::move(Cond)), Body(std::move(Body)) {}

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setBody(StmtPtr &&Body) { this->Body = std::move(Body); }

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  Stmt *getBody() { return Body.get(); }
  const Stmt *getBody() const { return Body.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ForStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ContinueStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BreakStmt : public Stmt {
public:
  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ReturnStmt : public Stmt {
  ExprPtr RetExpr;

public:
  ReturnStmt(ExprPtr RetExpr) : RetExpr(std::move(RetExpr)) {}

  void setRetValue(ExprPtr &&E) { RetExpr = std::move(E); }

  Expr *getRetValue() { return RetExpr.get(); }
  const Expr *getRetValue() const { return RetExpr.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
