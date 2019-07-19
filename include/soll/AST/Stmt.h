#pragma once

#include <memory>
#include <vector>

namespace soll {

class Expr;

class Stmt {};

using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;

class DeclStmt : public Stmt {};

class ExprStmt : public Stmt {};

class Block : public Stmt {
  std::vector<StmtPtr> stmts;
};

class IfStmt : public Stmt {
  ExprPtr Cond;
	StmtPtr Then;
	StmtPtr Else; /// optional

public:

  IfStmt(ExprPtr Cond, StmtPtr Then, StmtPtr Else): Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}
  
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

class ReturnStmt : public Stmt {};

} // namespace soll
