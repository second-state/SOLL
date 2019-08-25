// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/BreakableVisitor.h"
#include <cassert>
#include <vector>

using namespace soll;

void BreakableVisitor::check(FunctionDecl &FD) { FD.getBody()->accept(*this); }

void BreakableVisitor::visit(WhileStmtType &While) {
  breakablePath.push_back(&While);
  StmtVisitor::visit(While);
  breakablePath.pop_back();
}

void BreakableVisitor::visit(ForStmtType &FS) {
  breakablePath.push_back(&FS);
  StmtVisitor::visit(FS);
  breakablePath.pop_back();
}

void BreakableVisitor::visit(ContinueStmtType &CS) {
  // TODO: should be replaced by error message
  assert(!breakablePath.empty());
  CS.setLoopStmt(breakablePath.back());
  StmtVisitor::visit(CS);
}

void BreakableVisitor::visit(BreakStmtType &BS) {
  // TODO: should be replaced by error message
  assert(!breakablePath.empty());
  BS.setLoopStmt(breakablePath.back());
  StmtVisitor::visit(BS);
}