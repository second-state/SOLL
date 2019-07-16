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
  ExprPtr Condition;
	StmtPtr TrueBody;
	StmtPtr FalseBody; ///< "else" part, optional

  const Expr *getCond() const {
      return Condition.get();
  }

  void setCond(ExprPtr Cond) {
    // TODO
  }
};

class BreakableStmt : public Stmt {};

class ForStmt : public Stmt {};

class ContinueStmt : public Stmt {};

class BreakStmt : public Stmt {};

class ReturnStmt : public Stmt {};

} // namespace soll
