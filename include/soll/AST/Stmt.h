#pragma once

#include "soll/AST/StmtVisitor.h"
#include <llvm/ADT/APInt.h>
#include <memory>
#include <vector>

namespace soll {

class Expr;
class Decl;

class Stmt {
public:
  virtual ~Stmt() noexcept {}

  virtual void accept(StmtVisitor &visitor) = 0;
  virtual void accept(ConstStmtVisitor &visitor) const = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;
using DeclPtr = std::unique_ptr<Decl>;

class DeclStmt : public Stmt {
  std::vector<DeclPtr> Decls;
  ExprPtr Value;

public:
  DeclStmt(std::vector<DeclPtr> &&Decls, ExprPtr &&Value);

  std::vector<const Decl*> getDecls() const;
  std::vector<Decl*> getDecls();

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class ExprStmt : public Stmt {};

class Block : public Stmt {
  std::vector<StmtPtr> Stmts;

public:
  Block(std::vector<StmtPtr> &&Stmts) : Stmts(std::move(Stmts)) {}

  /// this setter transfers the ownerships of Stmt from function argument to class instance
  void setStmts(std::vector<StmtPtr> &&Stmts) {
    this->Stmts.clear();
    for(auto &&S: Stmts)
      this->Stmts.emplace_back(std::move(S));
  }

  std::vector<const Stmt *> getStmts() const {
    std::vector<const Stmt *> Stmts;
    for (auto &S : this->Stmts)
      Stmts.emplace_back(S.get());
    return Stmts;
  }

  std::vector<Stmt *> getStmts() {
    std::vector<Stmt *> Stmts;
    for (auto &S : this->Stmts)
      Stmts.emplace_back(S.get());
    return Stmts;
  }

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

  const Expr *getCond() const { return Cond.get(); }
  const Stmt *getThen() const { return Then.get(); }
  const Stmt *getElse() const { return Else.get(); }

  Expr *getCond() { return Cond.get(); }
  Stmt *getThen() { return Then.get(); }
  Stmt *getElse() { return Else.get(); }

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setThen(StmtPtr &&Then) { this->Then = std::move(Then); }
  void setElse(StmtPtr &&Else) { this->Else = std::move(Else); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

class BreakableStmt : public Stmt {};

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

  Expr *getRetValue() { return RetExpr.get(); }
  const Expr *getRetValue() const { return RetExpr.get(); }
  void setRetValue(ExprPtr &&E) { RetExpr = std::move(E); }

  void accept(StmtVisitor &visitor) override;
  void accept(ConstStmtVisitor &visitor) const override;
};

} // namespace soll
