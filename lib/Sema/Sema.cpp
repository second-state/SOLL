// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Sema.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/Lex/Lexer.h"

namespace soll {

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()) {}

ExprPtr Sema::CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS, ExprPtr &&RHS) {
  TypePtr ResultTy;
  switch (Opc) {
  case BO_Add:
      ResultTy = CheckAdditionOperands(LHS, RHS, Opc);
    break;
  // TODO: a lot of binary operations
  }
  return std::move(std::make_unique<BinaryOperator>(std::move(LHS), std::move(RHS), Opc));
}

TypePtr Sema::CheckAdditionOperands(ExprPtr &LHS, ExprPtr &RHS, BinaryOperatorKind Opc, TypePtr CompLHSTy) {
  TypePtr compType = UsualArithmeticConversions(LHS, RHS, CompLHSTy != nullptr);
  return std::move(compType);
}

TypePtr Sema::UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS, bool IsCompAssign) {
  if (!IsCompAssign) {
    LHS = UsualUnaryConversions(std::move(LHS));
  }

  RHS = UsualUnaryConversions(std::move(RHS));
  // TODO: get common Type
  return std::make_shared<IntegerType>(IntegerType::IntKind::U64);
}

ExprPtr Sema::UsualUnaryConversions(ExprPtr &&E) {
  return std::move(Sema::DefaultLvalueConversion(std::move(E))); 
}

ExprPtr Sema::DefaultLvalueConversion(ExprPtr &&E) {
  if(E->isRValue()) return std::move(E);

  auto Res = std::move(std::make_unique<ImplicitCastExpr>(std::move(E), CastKind::LValueToRValue));

  return std::move(Res);
}

void Sema::resolveBreak(FunctionDecl &FD) {
  BreakableVisitor().check(FD);
}

} // namespace soll
