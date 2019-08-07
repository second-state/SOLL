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

  /// this setter transfers the ownerships of Stmt from function argument to
  /// class instance
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
  IfStmt(ExprPtr &&Cond, StmtPtr &&Then, StmtPtr &&Else)
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
  bool DoWhile;

public:
  WhileStmt(ExprPtr &&Cond, StmtPtr &&Body, bool DoWhile)
      : Cond(std::move(Cond)), Body(std::move(Body)), DoWhile(DoWhile) {}

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setBody(StmtPtr &&Body) { this->Body = std::move(Body); }
  void setDoWhile(bool DoWhile) { this->DoWhile = DoWhile; }

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  Stmt *getBody() { return Body.get(); }
  const Stmt *getBody() const { return Body.get(); }
  const bool isDoWhile() const { return DoWhile; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ForStmt : public Stmt {
  StmtPtr Init; /// optional
  ExprPtr Cond; /// optional
  ExprPtr Loop; /// optional
  StmtPtr Body;

public:
  ForStmt(StmtPtr &&Init, ExprPtr &&Cond, ExprPtr &&Loop, StmtPtr &&Body)
      : Init(std::move(Init)), Cond(std::move(Cond)), Loop(std::move(Loop)),
        Body(std::move(Body)) {}

  void setInit(StmtPtr &&Init) { this->Init = std::move(Init); }
  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setLoop(ExprPtr &&Loop) { this->Loop = std::move(Loop); }
  void setBody(StmtPtr &&Body) { this->Body = std::move(Body); }

  Stmt *getInit() { return Init.get(); }
  const Stmt *getInit() const { return Init.get(); }
  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  Expr *getLoop() { return Loop.get(); }
  const Expr *getLoop() const { return Loop.get(); }
  Stmt *getBody() { return Body.get(); }
  const Stmt *getBody() const { return Body.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ContinueStmt : public Stmt {
  /// The loop statement that this ContinueStmt is in
  Stmt *LoopStmt;

public:
  ContinueStmt(Stmt *LS) : LoopStmt(LS) {}

  void setLoopStmt(Stmt *LS) { LoopStmt = LS; }
  Stmt *getLoopStmt() { return LoopStmt; }
  const Stmt *getLoopStmt() const { return LoopStmt; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BreakStmt : public Stmt {
  /// The loop statement that this BreakStmt is in
  Stmt *LoopStmt;

public:
  BreakStmt(Stmt *LS) : LoopStmt(LS) {}

  void setLoopStmt(Stmt *LS) { LoopStmt = LS; }
  Stmt *getLoopStmt() { return LoopStmt; }
  const Stmt *getLoopStmt() const { return LoopStmt; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ReturnStmt : public Stmt {
  ExprPtr RetExpr;

public:
  ReturnStmt(ExprPtr &&RetExpr) : RetExpr(std::move(RetExpr)) {}

  void setRetValue(ExprPtr &&E) { RetExpr = std::move(E); }

  Expr *getRetValue() { return RetExpr.get(); }
  const Expr *getRetValue() const { return RetExpr.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
