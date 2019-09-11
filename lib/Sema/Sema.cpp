// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Sema.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
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
  return commonTy;
}

static void TryImplicitCast(TypePtr DstTy, ExprPtr &Vaule) {
  auto SrcTy = Vaule->getType();
  if (SrcTy->isImplicitlyConvertibleTo(*DstTy)) {
    if (DstTy->getCategory() == Type::Category::Integer &&
        SrcTy->getCategory() == Type::Category::Integer) {
      auto DITy = static_cast<IntegerType *>(DstTy.get());
      auto SITy = static_cast<IntegerType *>(SrcTy.get());
      if (SITy->getKind() != DITy->getKind())
        Vaule = std::make_unique<ImplicitCastExpr>(
            std::move(Vaule), CastKind::IntegralCast, DstTy);
      // TODO: signed/unsigned check
    }
    // TODO: other type conversion such as address->int
  } else {
    assert(false && "no implicit conversion");
  }
}

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()) {}

StmtPtr Sema::CreateReturnStmt(ExprPtr &&Value) {
  TryImplicitCast(FunRtnTys.front(), Value);
  return std::make_unique<ReturnStmt>(
      Sema::DefaultLvalueConversion(std::move(Value)));
}

std::unique_ptr<FunctionDecl> Sema::CreateFunctionDecl(
    llvm::StringRef Name, FunctionDecl::Visibility Vis, StateMutability SM,
    bool IsConstructor, bool IsFallback, std::unique_ptr<ParamList> &&Params,
    std::vector<std::unique_ptr<ModifierInvocation>> &&Modifiers,
    std::unique_ptr<ParamList> &&ReturnParams, std::unique_ptr<Block> &&Body) {
  auto FD = std::make_unique<FunctionDecl>(
      Name, Vis, SM, IsConstructor, IsFallback, std::move(Params),
      std::move(Modifiers), std::move(ReturnParams), std::move(Body));
  resolveBreak(*FD);
  addIdentifierDecl(FD->getName(), *FD);
  return std::move(FD);
}

std::unique_ptr<CallableVarDecl>
Sema::CreateEventDecl(llvm::StringRef Name, std::unique_ptr<ParamList> &&Params,
                      bool Anonymous) {
  auto ED = std::make_unique<CallableVarDecl>(Name, Decl::Visibility::Default,
                                              std::move(Params));
  addIdentifierDecl(ED->getName(), *ED.get());
  return std::move(ED);
}

ExprPtr Sema::CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS,
                          ExprPtr &&RHS) {
  TypePtr ResultTy;
  if (BinaryOperator::isAdditiveOp(Opc)) {
    ResultTy = CheckAdditiveOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isMultiplicativeOp(Opc)) {
    ResultTy = CheckMultiplicativeOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isComparisonOp(Opc)) {
    ResultTy = CheckCompareOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isLogicalOp(Opc)) {
    ResultTy = CheckLogicalOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isBitwiseOp(Opc)) {
    ResultTy = CheckBitwiseOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isShiftOp(Opc)) {
    ResultTy = CheckShiftOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isAssignmentOp(Opc)) {
    // TODO: get common type
    ResultTy = LHS->getType();
    switch (Opc) {
    case BinaryOperatorKind::BO_AddAssign: {
      CheckAdditiveOperands(LHS, RHS, Opc, ResultTy);
      break;
    }
    default:;
    }
    CheckAssignmentOperands(LHS, RHS, ResultTy);
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

ExprPtr Sema::CreateCallExpr(ExprPtr &&Callee, std::vector<ExprPtr> &&Args) {
  TypePtr ResultTy = Callee->getType();
  if (auto CalleeIden = dynamic_cast<Identifier *>(Callee.get())) {
    if (auto FD =
            dynamic_cast<FunctionDecl *>(CalleeIden->getCorrespondDecl())) {
      // TODO: refactor this
      // This assumes function only returns one value,
      // because tuple type not impl. yet.
      auto FDTy = static_cast<FunctionType *>(FD->getType().get());
      if (auto ReturnTy = FDTy->getReturnTypes(); ReturnTy.size() == 1)
        ResultTy = ReturnTy[0];
    } else if (auto ED = dynamic_cast<CallableVarDecl *>(
                   CalleeIden->getCorrespondDecl())) {
    } else {
      assert(isMagicFuncName(CalleeIden->getName()) &&
             "callee is not FuncDecl");
    }
  } else
    assert(false && "only support Identifier callee");

  for (ExprPtr &Arg : Args) {
    Arg = DefaultLvalueConversion(std::move(Arg));
  }
  return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                    ResultTy);
}

std::unique_ptr<Identifier> Sema::CreateIdentifier(const std::string Name) {
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

TypePtr Sema::CheckShiftOperands(ExprPtr &LHS, ExprPtr &RHS,
                                 BinaryOperatorKind Opc, bool IsCompAssign) {
  if (!IsCompAssign)
    LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Integer &&
         "LHS is not int.");
  assert(RHS->getType()->getCategory() == Type::Category::Integer &&
         "RHS is not int.");
  return LHS->getType();
}

TypePtr Sema::CheckCompareOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  auto LTy = LHS->getType();
  auto RTy = RHS->getType();

  if (LTy->getCategory() == Type::Category::Integer &&
      RTy->getCategory() == Type::Category::Integer)
    UsualArithmeticConversions(LHS, RHS, false);
  else if (LTy->getCategory() == Type::Category::Address &&
           RTy->getCategory() == Type::Category::Address) {
    LHS = UsualUnaryConversions(std::move(LHS));
    RHS = UsualUnaryConversions(std::move(RHS));
  } else
    // TODO: replace this, should impl. Compare Diagonostic
    assert(false && "cannot compare");

  return std::make_shared<BooleanType>();
}

TypePtr Sema::CheckLogicalOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Bool &&
         "LHS is not boolean.");
  assert(RHS->getType()->getCategory() == Type::Category::Bool &&
         "RHS is not boolean.");
  return std::make_shared<BooleanType>();
}

TypePtr Sema::CheckBitwiseOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Integer &&
         "LHS is not int.");
  assert(RHS->getType()->getCategory() == Type::Category::Integer &&
         "RHS is not int.");
  return LHS->getType();
}

TypePtr Sema::CheckAssignmentOperands(ExprPtr &LHS, ExprPtr &RHS,
                                      TypePtr CompoundType) {
  RHS = UsualUnaryConversions(std::move(RHS));
  auto LTy = LHS->getType();
  TryImplicitCast(LHS->getType(), RHS);
  return LHS->getType();
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

void Sema::addIdentifierDecl(const std::string &S, Decl &D) {
  ID2DeclTable.emplace(S, &D);
}

// TODO: refactor this
// current impl. assumes no name scope
Decl *Sema::findIdentifierDecl(const std::string &S) {
  if (auto iter = ID2DeclTable.find(S); iter != ID2DeclTable.end())
    return iter->second;
  else
    return nullptr;
}

} // namespace soll
