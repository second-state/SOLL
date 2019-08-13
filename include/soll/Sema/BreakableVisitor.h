// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Stmt.h"
#include "soll/AST/Decl.h"
#include "soll/AST/StmtVisitor.h"

namespace soll {

class BreakableVisitor : public StmtVisitor {
private:
  std::vector<Stmt *> breakablePath;

public:
  BreakableVisitor() {}

  void check(FunctionDecl &);

  void visit(WhileStmtType &) override;
  void visit(ForStmtType &) override;
  void visit(ContinueStmtType &) override;
  void visit(BreakStmtType &) override;
};

} // namespace soll