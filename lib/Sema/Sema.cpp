// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Sema.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/Lex/Lexer.h"

namespace soll {

/// handle int arithemetic converisons
/// helper function of UsualArithmeticConversions
static TypePtr handleIntegerConversion(Sema &S, ExprPtr &LHS, ExprPtr &RHS,
                                       bool isCompAssign) {
  assert(LHS->getType() != nullptr);
  assert(RHS->getType() != nullptr);
  auto LTy = dynamic_cast<IntegerType *>(LHS->getType().get());
  auto RTy = dynamic_cast<IntegerType *>(RHS->getType().get());

  if (LTy->getKind() == RTy->getKind())
    return LHS->getType();
  auto commonTy = IntegerType::common(*LTy, *RTy);

  if (commonTy->getKind() != LTy->getKind())
    LHS = std::make_unique<ImplicitCastExpr>(std::move(LHS),
                                             CastKind::IntegralCast, commonTy);
  if (commonTy->getKind() != RTy->getKind())
    RHS = std::make_unique<ImplicitCastExpr>(std::move(RHS),
                                             CastKind::IntegralCast, commonTy);
  return std::make_shared<IntegerType>(IntegerType::IntKind::U64);
}

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()) {}

ExprPtr Sema::CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS,
                          ExprPtr &&RHS) {
  TypePtr ResultTy;
  if (BinaryOperator::isAdditiveOp(Opc)) {
    ResultTy = CheckAdditiveOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isMultiplicativeOp(Opc)) {
    ResultTy = CheckMultiplicativeOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isComparisonOp(Opc)) {
    ResultTy = CheckCompareOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isAssignmentOp(Opc)) {
    ResultTy = LHS->getType();
  }
  // TODO: a lot of binary operations
  return std::move(std::make_unique<BinaryOperator>(
      std::move(LHS), std::move(RHS), Opc, ResultTy));
}

ExprPtr Sema::CreateIndexAccess(ExprPtr &&LHS, ExprPtr &&RHS) {
  // TODO check whether LHS is LValue
  RHS = DefaultLvalueConversion(std::move(RHS));

  TypePtr ResultTy;
  if (auto MT = dynamic_cast<MappingType *>(LHS->getType().get())) {
    ResultTy = MT->getValueType();
  } else if (auto AT = dynamic_cast<ArrayType *>(LHS->getType().get())) {
    // TODO: replace this with Diagonistic
    assert(RHS->getType()->getCategory() == Type::Category::Integer);
    ResultTy = AT->getElementType();
  } else
    assert(false);
  return std::make_unique<IndexAccess>(std::move(LHS), std::move(RHS),
                                       ResultTy);
}

ExprPtr Sema::CreateCallExpr(ExprPtr &&Func, std::vector<ExprPtr> &&Args) {
  TypePtr ResultTy;
  // TODO: resolve returnType
  return std::make_unique<CallExpr>(std::move(Func), std::move(Args), ResultTy);
}

ExprPtr Sema::CreateIdentifier(const std::string Name) {
  return std::make_unique<Identifier>(Name, findIdentifierDecl(Name));
}

TypePtr Sema::CheckAdditiveOperands(ExprPtr &LHS, ExprPtr &RHS,
                                    BinaryOperatorKind Opc, TypePtr CompLHSTy) {
  TypePtr compType = UsualArithmeticConversions(LHS, RHS, CompLHSTy != nullptr);
  // TODO: replace this, should impl. Additive Diagonostic
  assert(LHS->getType()->getCategory() == Type::Category::Integer);
  assert(RHS->getType()->getCategory() == Type::Category::Integer);

  return std::move(compType);
}

TypePtr Sema::CheckMultiplicativeOperands(ExprPtr &LHS, ExprPtr &RHS,
                                          BinaryOperatorKind Opc,
                                          TypePtr CompLHSTy) {
  TypePtr compType = UsualArithmeticConversions(LHS, RHS, CompLHSTy != nullptr);
  // TODO: replace this, should impl. Multiplicative Diagonostic
  assert(LHS->getType()->getCategory() == Type::Category::Integer);
  assert(RHS->getType()->getCategory() == Type::Category::Integer);

  return std::move(compType);
}

TypePtr Sema::CheckCompareOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  // TODO: get common Type
  LHS = UsualUnaryConversions(std::move(LHS));
  RHS = UsualUnaryConversions(std::move(RHS));
  // TODO: replace this, should impl. Compare Diagonostic
  assert(LHS->getType()->getCategory() == Type::Category::Integer);
  assert(RHS->getType()->getCategory() == Type::Category::Integer);

  return std::make_shared<BooleanType>();
}

TypePtr Sema::UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS,
                                         bool IsCompAssign) {
  if (!IsCompAssign) {
    LHS = UsualUnaryConversions(std::move(LHS));
  }

  RHS = UsualUnaryConversions(std::move(RHS));

  return handleIntegerConversion(*this, LHS, RHS, IsCompAssign);
}

ExprPtr Sema::UsualUnaryConversions(ExprPtr &&E) {
  return std::move(Sema::DefaultLvalueConversion(std::move(E)));
}

ExprPtr Sema::DefaultLvalueConversion(ExprPtr &&E) {
  if (E->isRValue())
    return std::move(E);

  auto Res = std::move(std::make_unique<ImplicitCastExpr>(
      std::move(E), CastKind::LValueToRValue, E->getType()));

  return std::move(Res);
}

void Sema::resolveBreak(FunctionDecl &FD) { BreakableVisitor().check(FD); }

} // namespace soll
