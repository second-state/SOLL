#pragma once

#include "llvm/ADT/APInt.h"

#include <memory>
#include <vector>

#include "soll/AST/StmtVisitor.h"

namespace soll {

class Expr;

class Stmt {
public:
  virtual ~Stmt() {}

  virtual void accept(StmtVisitor &visitor) = 0;
  virtual void accept(ConstStmtVisitor &visitor) const = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;

class DeclStmt : public Stmt {};

class ExprStmt : public Stmt {};

class Block : public Stmt {
  std::vector<StmtPtr> Stmts;

public:
  /// this setter transfers the ownerships of Stmt from function argument to class instance
  void setStmts(std::vector<StmtPtr> &&Stmts) {
    this->Stmts.clear();
    for(auto &&S: Stmts)
      this->Stmts.emplace_back(std::move(S));
  }

  std::vector<const Stmt *> getStmts() {
    std::vector<const Stmt *> Stmts;
    for (auto &S : this->Stmts)
      Stmts.emplace_back(S.get());
    return Stmts;
  }
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

  void setCond(ExprPtr &&Cond) { this->Cond = std::move(Cond); }
  void setThen(StmtPtr &&Then) { this->Then = std::move(Then); }
  void setElse(StmtPtr &&Else) { this->Else = std::move(Else); }
};

class BreakableStmt : public Stmt {};

class ForStmt : public Stmt {};

class ContinueStmt : public Stmt {};

class BreakStmt : public Stmt {};

class ReturnStmt : public Stmt {
  ExprPtr RetExpr;
public:
  const Expr *getRetValue() { return RetExpr.get(); }
  void setRetValue(ExprPtr &&E) { RetExpr = std::move(E); }
};

} // namespace soll
