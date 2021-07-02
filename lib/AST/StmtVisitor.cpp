// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/ExprAsm.h"
#include "soll/AST/StmtAsm.h"

namespace soll {

template <bool Const> void StmtVisitorBase<Const>::visit(BlockType &block) {
  for (auto stmt : block.getStmts())
    stmt->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(EmitStmtType &stmt) {
  stmt.getCall()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(DeclStmtType &stmt) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(IfStmtType &stmt) {
  stmt.getCond()->accept(*this);
  stmt.getThen()->accept(*this);
  if (stmt.getElse())
    stmt.getElse()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(WhileStmtType &stmt) {
  stmt.getCond()->accept(*this);
  stmt.getBody()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(ForStmtType &stmt) {
  if (stmt.getInit())
    stmt.getInit()->accept(*this);
  if (stmt.getCond())
    stmt.getCond()->accept(*this);
  if (stmt.getLoop())
    stmt.getLoop()->accept(*this);
  stmt.getBody()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(ContinueStmtType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(BreakStmtType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(ReturnStmtType &stmt) {
  if (stmt.getRetValue()) {
    stmt.getRetValue()->accept(*this);
  }
}

template <bool Const> void StmtVisitorBase<Const>::visit(TupleExprType &op) {
  for (auto Comps : op.getComponents())
    if (Comps)
      Comps->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(TypesTupleExprType &op) {
  // leaf, do nothing
}

template <bool Const>
void StmtVisitorBase<Const>::visit(DirectValueExprType &op) {
  // leaf, do nothing
}

template <bool Const>
void StmtVisitorBase<Const>::visit(ReturnTupleExprType &op) {
  op.getCalleeExpr()->accept(*this);
  op.getTupleExpr()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(UnaryOperatorType &op) {
  op.getSubExpr()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(BinaryOperatorType &op) {
  op.getLHS()->accept(*this);
  op.getRHS()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(CallExprType &expr) {
  expr.getCalleeExpr()->accept(*this);
  for (auto &args : expr.getRawArguments())
    args->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(ImplicitCastExprType &expr) {
  expr.getSubExpr()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(ExplicitCastExprType &expr) {
  expr.getSubExpr()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(ParenExprType &expr) {
  expr.getSubExpr()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(MemberExprType &expr) {
  expr.getBase()->accept(*this);
  expr.getName()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(IndexAccessType &expr) {
  expr.getBase()->accept(*this);
  expr.getIndex()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(IdentifierType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(BooleanLiteralType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(StringLiteralType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(NumberLiteralType &) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(AsmForStmtType &S) {
  S.getInit()->accept(*this);
  S.getCond()->accept(*this);
  S.getLoop()->accept(*this);
  S.getBody()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(AsmCaseStmtType &C) {
  C.getSubStmt()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(AsmDefaultStmtType &C) {
  C.getSubStmt()->accept(*this);
}

template <bool Const> void StmtVisitorBase<Const>::visit(AsmSwitchStmtType &S) {
  S.getCond()->accept(*this);
  for (auto Case : S.getCases()) {
    Case->accept(*this);
  }
}

template <bool Const>
void StmtVisitorBase<Const>::visit(AsmAssignmentStmtType &A) {
  A.getLHS()->accept(*this);
  A.getRHS()->accept(*this);
}

template <bool Const>
void StmtVisitorBase<Const>::visit(AsmFunctionDeclStmtType &D) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(AsmLeaveStmtType &D) {
  // leaf, do nothing
}

template <bool Const> void StmtVisitorBase<Const>::visit(AsmIdentifierType &) {
  // leaf, do nothing
}

template <bool Const>
void StmtVisitorBase<Const>::visit(AsmIdentifierListType &L) {
  for (auto I : L.getIdentifiers())
    I->accept(*this);
}

} // namespace soll

/// Explicit instantiation
template class soll::StmtVisitorBase<true>;
template class soll::StmtVisitorBase<false>;
