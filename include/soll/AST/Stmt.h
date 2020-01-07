// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/StmtVisitor.h"
#include "soll/Basic/SourceLocation.h"
#include <llvm/ADT/APInt.h>
#include <vector>

namespace soll {

class Stmt {
  SourceRange Location;

public:
  explicit Stmt(SourceRange L) : Location(L) {}
  virtual ~Stmt() noexcept {}

  virtual void accept(StmtVisitor &visitor) = 0;
  virtual void accept(ConstStmtVisitor &visitor) const = 0;

  const SourceRange &getLocation() const { return Location; }
};

class EmitStmt : public Stmt {
  std::unique_ptr<CallExpr> EventCall;

public:
  inline EmitStmt(SourceRange L, std::unique_ptr<CallExpr> &&EventCall);

  CallExpr *getCall() { return EventCall.get(); }
  const CallExpr *getCall() const { return EventCall.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class DeclStmt : public Stmt {
  std::vector<VarDeclBasePtr> VarDecls;
  ExprPtr Value;

public:
  inline DeclStmt(SourceRange L, std::vector<VarDeclBasePtr> &&VarDecls,
                  ExprPtr &&Value);

  std::vector<VarDeclBase *> getVarDecls();
  std::vector<const VarDeclBase *> getVarDecls() const;
  Expr *getValue() { return Value.get(); }
  const Expr *getValue() const { return Value.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ExprStmt : public Stmt {
public:
  explicit ExprStmt(SourceRange L) : Stmt(L) {}
};

class Block : public Stmt {
  std::vector<StmtPtr> Stmts;

public:
  Block(SourceRange L, std::vector<StmtPtr> &&Stmts)
      : Stmt(L), Stmts(std::move(Stmts)) {}

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
  IfStmt(SourceRange L, ExprPtr &&Cond, StmtPtr &&Then, StmtPtr &&Else)
      : Stmt(L), Cond(std::move(Cond)), Then(std::move(Then)),
        Else(std::move(Else)) {}

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

class WhileStmt : public Stmt {
  ExprPtr Cond;
  StmtPtr Body;
  bool DoWhile;

public:
  WhileStmt(SourceRange L, ExprPtr &&Cond, StmtPtr &&Body, bool DoWhile)
      : Stmt(L), Cond(std::move(Cond)), Body(std::move(Body)),
        DoWhile(DoWhile) {}

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setBody(StmtPtr &&Body) { this->Body = std::move(Body); }
  void setDoWhile(bool DoWhile) { this->DoWhile = DoWhile; }

  Expr *getCond() { return Cond.get(); }
  const Expr *getCond() const { return Cond.get(); }
  Stmt *getBody() { return Body.get(); }
  const Stmt *getBody() const { return Body.get(); }
  bool isDoWhile() const { return DoWhile; }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ForStmt : public Stmt {
  StmtPtr Init; /// optional
  ExprPtr Cond; /// optional
  ExprPtr Loop; /// optional
  StmtPtr Body;

public:
  ForStmt(SourceRange L, StmtPtr &&Init, ExprPtr &&Cond, ExprPtr &&Loop,
          StmtPtr &&Body)
      : Stmt(L), Init(std::move(Init)), Cond(std::move(Cond)),
        Loop(std::move(Loop)), Body(std::move(Body)) {}

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
public:
  ContinueStmt(SourceRange L) : Stmt(L) {}

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BreakStmt : public Stmt {
public:
  BreakStmt(SourceRange L) : Stmt(L){};

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ReturnStmt : public Stmt {
  ExprPtr RetExpr;

public:
  ReturnStmt(SourceRange L, ExprPtr &&RetExpr)
      : Stmt(L), RetExpr(std::move(RetExpr)) {}

  void setRetValue(ExprPtr &&E) { RetExpr = std::move(E); }

  Expr *getRetValue() { return RetExpr.get(); }
  const Expr *getRetValue() const { return RetExpr.get(); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll

#include "Expr.h"
soll::EmitStmt::EmitStmt(SourceRange L, std::unique_ptr<CallExpr> &&EventCall)
    : Stmt(L), EventCall(std::move(EventCall)) {}

#include "Decl.h"
soll::DeclStmt::DeclStmt(SourceRange L, std::vector<VarDeclBasePtr> &&VarDecls,
                         ExprPtr &&Value)
    : Stmt(L), VarDecls(std::move(VarDecls)), Value(std::move(Value)) {}
