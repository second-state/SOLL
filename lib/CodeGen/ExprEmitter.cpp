#include "ExprEmitter.h"

namespace soll::CodeGen {
ExprValuePtr ExprEmitter::visit(const Expr *E) {
  if (auto UO = dynamic_cast<const UnaryOperator *>(E)) {
    return visit(UO);
  }
  if (auto BO = dynamic_cast<const BinaryOperator *>(E)) {
    return visit(BO);
  }
  if (auto CE = dynamic_cast<const CallExpr *>(E)) {
    return visit(CE);
  }
  if (auto CE = dynamic_cast<const CastExpr *>(E)) {
    return visit(CE);
  }
  if (auto NE = dynamic_cast<const NewExpr *>(E)) {
    return visit(NE);
  }
  if (auto ME = dynamic_cast<const MemberExpr *>(E)) {
    return visit(ME);
  }
  if (auto IA = dynamic_cast<const IndexAccess *>(E)) {
    return visit(IA);
  }
  if (auto PE = dynamic_cast<const ParenExpr *>(E)) {
    return visit(PE);
  }
  if (auto CE = dynamic_cast<const ConstantExpr *>(E)) {
    return visit(CE);
  }
  if (auto TE = dynamic_cast<const TupleExpr *>(E)) {
    return visit(TE);
  }
  if (auto DVE = dynamic_cast<const DirectValueExpr *>(E)) {
    return visit(DVE);
  }
  if (auto RTE = dynamic_cast<const ReturnTupleExpr *>(E)) {
    return visit(RTE);
  }
  if (auto I = dynamic_cast<const Identifier *>(E)) {
    return visit(I);
  }
  if (auto ETNE = dynamic_cast<const ElementaryTypeNameExpr *>(E)) {
    return visit(ETNE);
  }
  if (auto BL = dynamic_cast<const BooleanLiteral *>(E)) {
    return visit(BL);
  }
  if (auto SL = dynamic_cast<const StringLiteral *>(E)) {
    return visit(SL);
  }
  if (auto NL = dynamic_cast<const NumberLiteral *>(E)) {
    return visit(NL);
  }
  if (auto AI = dynamic_cast<const AsmIdentifier *>(E)) {
    return visit(AI);
  }
  assert(false && "unknown Expr!");
  __builtin_unreachable();
}

ExprValuePtr ExprEmitter::structIndexAccess(const ExprValuePtr StructValue,
                                            unsigned ElementIndex,
                                            const StructType *STy) {
  auto ET = STy->getElementTypes()[ElementIndex];
  switch (StructValue->getValueKind()) {
  case ValueKind::VK_SValue: {
    llvm::Value *Base = Builder.CreateLoad(StructValue->getValue());
    llvm::Value *Pos = Builder.getIntN(256, STy->getStoragePos(ElementIndex));
    llvm::Value *ElemAddress = Builder.CreateAdd(Base, Pos);
    llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
    Builder.CreateStore(ElemAddress, Address);
    return std::make_shared<ExprValue>(ET.get(), ValueKind::VK_SValue, Address);
  }
  case ValueKind::VK_LValue: {
    llvm::Value *Base = StructValue->getValue();

    std::vector<llvm::Value *> Indices(2);
    Indices[0] = llvm::ConstantInt::get(VMContext, llvm::APInt(32, 0, true));
    Indices[1] =
        llvm::ConstantInt::get(VMContext, llvm::APInt(32, ElementIndex, true));
    llvm::Value *ElementPtr = Builder.CreateGEP(Base, Indices);
    return std::make_shared<ExprValue>(ET.get(), ValueKind::VK_LValue,
                                       ElementPtr);
  }
  case ValueKind::VK_RValue: {
    llvm::Value *Value = StructValue->load(Builder, CGM);
    llvm::Value *Element = Builder.CreateExtractValue(Value, {ElementIndex});
    return std::make_shared<ExprValue>(ET.get(), ValueKind::VK_RValue, Element);
  }
  default:
    assert(false && "Unknow ValueKind");
  }
  return nullptr;
}

ExprValuePtr ExprEmitter::arrayIndexAccess(const ExprValuePtr &Base,
                                           llvm::Value *IndexValue,
                                           const Type *Ty,
                                           const ArrayType *ArrTy,
                                           bool isStateVariable) {
  // Array Type : Fixed Size Mem Array, Fixed Sized Storage Array, Dynamic
  // Sized Storage Array
  // Require Index to be unsigned 256-bit Int
  llvm::Value *Value = Base->getValue();
  if (!isStateVariable) {
    // TODO : Assume only fixSized Integer Array
    llvm::Value *ArraySize = Builder.getInt(ArrTy->getLength());
    emitCheckArrayOutOfBound(ArraySize, IndexValue);
    llvm::Value *Address =
        Builder.CreateInBoundsGEP(CGF.getCodeGenModule().getLLVMType(ArrTy),
                                  Value, {Builder.getIntN(256, 0), IndexValue});
    return std::make_shared<ExprValue>(Ty, ValueKind::VK_LValue, Address);
  }

  llvm::Value *Pos = Builder.CreateLoad(Value);
  if (ArrTy->isDynamicSized()) {
    // load array size and check
    auto LengthTy = IntegerType::getIntN(256);
    ExprValue BaseLength(&LengthTy, ValueKind::VK_SValue, Value);
    llvm::Value *ArraySize = BaseLength.load(Builder, CGF.getCodeGenModule());
    emitCheckArrayOutOfBound(ArraySize, IndexValue);

    // load array position
    llvm::Value *Bytes = CGF.getCodeGenModule().emitConcatBytes(
        {CGF.getCodeGenModule().getEndianlessValue(Pos)});
    Pos = CGF.CGM.emitKeccak256(Bytes);
  } else {
    // Fixed Size Storage Array
    llvm::Value *ArraySize = Builder.getInt(ArrTy->getLength());
    emitCheckArrayOutOfBound(ArraySize, IndexValue);
  }

  unsigned BitPerElement = ArrTy->getElementType()->getBitNum();
  unsigned ElementPerSlot = 256 / BitPerElement;

  if (ElementPerSlot != 1) {
    llvm::Value *StorageIndex =
        Builder.CreateUDiv(Builder.CreateZExtOrTrunc(IndexValue, CGF.Int256Ty),
                           Builder.getIntN(256, ElementPerSlot));
    llvm::Value *Shift =
        Builder.CreateURem(Builder.CreateZExtOrTrunc(IndexValue, CGF.Int256Ty),
                           Builder.getIntN(256, ElementPerSlot));
    llvm::Value *ElemAddress = Builder.CreateAdd(
        Pos, Builder.CreateZExtOrTrunc(StorageIndex, Pos->getType()));
    llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
    Builder.CreateStore(ElemAddress, Address);
    return std::make_shared<ExprValue>(Ty, ValueKind::VK_SValue, Address,
                                       Shift);
  } else {
    llvm::Value *ElemAddress = Builder.CreateAdd(
        Pos, Builder.CreateZExtOrTrunc(IndexValue, Pos->getType()));
    llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
    Builder.CreateStore(ElemAddress, Address);
    return std::make_shared<ExprValue>(Ty, ValueKind::VK_SValue, Address);
  }
}

ExprValuePtr ExprEmitter::visit(const UnaryOperator *UO) {
  const Expr *SubExpr = UO->getSubExpr();
  ExprValuePtr SubVal = visit(SubExpr);
  llvm::Value *Value = SubVal->load(Builder, CGM);
  if (UO->isArithmeticOp()) {
    switch (UO->getOpcode()) {
    case UnaryOperatorKind::UO_Plus:
      break;
    case UnaryOperatorKind::UO_Minus:
      Value = Builder.CreateNeg(Value);
      break;
    case UnaryOperatorKind::UO_Not:
      Value = Builder.CreateNot(Value);
      break;
    case UnaryOperatorKind::UO_LNot:
      Value = Builder.CreateICmpEQ(Value, Builder.getInt1(false));
      break;
    case UnaryOperatorKind::UO_IsZero:
      Value = Builder.CreateICmpEQ(Value,
                                   llvm::ConstantInt::get(Value->getType(), 0));
      break;
    default:
      assert(false && "unknown op");
      __builtin_unreachable();
    }
  } else {
    switch (UO->getOpcode()) {
    case UnaryOperatorKind::UO_PostInc:
      SubVal->store(Builder, CGM,
                    Builder.CreateAdd(
                        Value, llvm::ConstantInt::get(Value->getType(), 1)));
      break;
    case UnaryOperatorKind::UO_PostDec:
      SubVal->store(Builder, CGM,
                    Builder.CreateSub(
                        Value, llvm::ConstantInt::get(Value->getType(), 1)));
      break;
    case UnaryOperatorKind::UO_PreInc:
      Value =
          Builder.CreateAdd(Value, llvm::ConstantInt::get(Value->getType(), 1));
      SubVal->store(Builder, CGM, Value);
      break;
    case UnaryOperatorKind::UO_PreDec:
      Value =
          Builder.CreateSub(Value, llvm::ConstantInt::get(Value->getType(), 1));
      SubVal->store(Builder, CGM, Value);
      break;
    default:
      assert(false && "unknown op");
      __builtin_unreachable();
    }
  }
  return ExprValue::getRValue(SubExpr, Value);
}

bool ExprEmitter::isSigned(const Type *Ty) {
  if (auto TyNow = dynamic_cast<const IntegerType *>(Ty))
    return TyNow->isSigned();
  else if (dynamic_cast<const BooleanType *>(Ty))
    return false;
  else if (dynamic_cast<const AddressType *>(Ty))
    return false;
  else {
    assert(false && "Wrong type in binary operator!");
    __builtin_unreachable();
  }
}

ExprValuePtr ExprEmitter::visit(const BinaryOperator *BO) {
  const Type *Ty = BO->getType().get();
  llvm::Value *V = nullptr;
  if (BO->isAssignmentOp()) {
    ExprValuePtr LHSVar = visit(BO->getLHS());
    ExprValuePtr RHSVar = visit(BO->getRHS());
    llvm::Value *RHSVal =
        RHSVar->isTuple() ||
                (RHSVar->getType()->getCategory() == Type::Category::Struct)
            ? nullptr
            : RHSVar->load(Builder, CGM);
    if (BO->getOpcode() == BO_Assign) {
      if (RHSVar->getType()->getCategory() == Type::Category::Struct &&
          LHSVar->getType() == RHSVar->getType()) {
        auto StructTy = dynamic_cast<const StructType *>(RHSVar->getType());
        for (size_t I = 0; I < StructTy->getElementSize(); ++I) {
          auto RHSVarI = structIndexAccess(RHSVar, I, StructTy);
          assert(RHSVarI->getType()->getCategory() != Type::Category::Struct &&
                 "Recursive struct is not yet supported");
          auto LHSVarI = structIndexAccess(LHSVar, I, StructTy);
          auto RHSValI = RHSVarI->load(Builder, CGM);
          LHSVarI->store(Builder, CGM, RHSValI);
        }
        return RHSVar;
      } else if (LHSVar->isTuple() && RHSVar->isTuple()) {
        assert(LHSVar->isTuple());
        assert(RHSVar->isTuple());

        ExprValueTuple *LHSVarT = dynamic_cast<ExprValueTuple *>(LHSVar.get());
        ExprValueTuple *RHSVarT = dynamic_cast<ExprValueTuple *>(RHSVar.get());

        auto RHSVals = RHSVarT->load(Builder, CGM);
        LHSVarT->store(Builder, CGM, RHSVals);

        RHSVal = nullptr;
        return ExprValueTuple::getRValue(BO, RHSVals);
      } else {
        LHSVar->store(Builder, CGM, RHSVal);
      }
    } else {
      const bool Signed = isSigned(Ty);
      llvm::Value *LHSVal = LHSVar->load(Builder, CGM);
      switch (BO->getOpcode()) {
      case BO_MulAssign:
        LHSVal = Builder.CreateMul(LHSVal, RHSVal, "BO_MulAssign");
        break;
      case BO_DivAssign:
        if (Signed) {
          LHSVal = Builder.CreateSDiv(LHSVal, RHSVal, "BO_DivAssign");
        } else {
          LHSVal = Builder.CreateUDiv(LHSVal, RHSVal, "BO_DivAssign");
        }
        break;
      case BO_RemAssign:
        if (Signed) {
          LHSVal = Builder.CreateSRem(LHSVal, RHSVal, "BO_RemAssign");
        } else {
          LHSVal = Builder.CreateURem(LHSVal, RHSVal, "BO_RemAssign");
        }
        break;
      case BO_AddAssign:
        LHSVal = Builder.CreateAdd(LHSVal, RHSVal, "BO_AddAssign");
        break;
      case BO_SubAssign:
        LHSVal = Builder.CreateSub(LHSVal, RHSVal, "BO_SubAssign");
        break;
      case BO_ShlAssign:
        LHSVal = Builder.CreateShl(LHSVal, RHSVal, "BO_ShlAssign");
        break;
      case BO_ShrAssign:
        if (Signed) {
          LHSVal = Builder.CreateAShr(LHSVal, RHSVal, "BO_ShrAssign");
        } else {
          LHSVal = Builder.CreateLShr(LHSVal, RHSVal, "BO_ShrAssign");
        }
        break;
      case BO_AndAssign:
        LHSVal = Builder.CreateAnd(LHSVal, RHSVal, "BO_AndAssign");
        break;
      case BO_XorAssign:
        LHSVal = Builder.CreateXor(LHSVal, RHSVal, "BO_XorAssign");
        break;
      case BO_OrAssign:
        LHSVal = Builder.CreateOr(LHSVal, RHSVal, "BO_OrAssign");
        break;
      default:;
      }
      LHSVar->store(Builder, CGM, LHSVal);
    }
    V = RHSVal;
  } else if (BO->isAdditiveOp() || BO->isMultiplicativeOp() ||
             BO->isComparisonOp() || BO->isShiftOp() || BO->isBitwiseOp() ||
             BO->isAsmBitwiseOp() || BO->getOpcode() == BO_Exp) {
    const bool Signed = isSigned(Ty);

    using Pred = llvm::CmpInst::Predicate;
    llvm::Value *LHS = visit(BO->getLHS())->load(Builder, CGM);
    llvm::Value *RHS = visit(BO->getRHS())->load(Builder, CGM);
    switch (BO->getOpcode()) {
    case BinaryOperatorKind::BO_Add:
      V = Builder.CreateAdd(LHS, RHS, "BO_ADD");
      break;
    case BinaryOperatorKind::BO_Sub:
      V = Builder.CreateSub(LHS, RHS, "BO_SUB");
      break;
    case BinaryOperatorKind::BO_Mul:
      V = Builder.CreateMul(LHS, RHS, "BO_MUL");
      break;
    case BinaryOperatorKind::BO_Div:
      if (Signed) {
        V = Builder.CreateSDiv(LHS, RHS, "BO_DIV");
      } else {
        V = Builder.CreateUDiv(LHS, RHS, "BO_DIV");
      }
      break;
    case BinaryOperatorKind::BO_Rem:
      if (Signed) {
        V = Builder.CreateSRem(LHS, RHS, "BO_Rem");
      } else {
        V = Builder.CreateURem(LHS, RHS, "BO_Rem");
      }
      break;
    case BinaryOperatorKind::BO_GE:
      V = Builder.CreateICmp(Pred(Pred::ICMP_UGE + (Signed << 2)), LHS, RHS,
                             "BO_GE");
      break;
    case BinaryOperatorKind::BO_GT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_UGT + (Signed << 2)), LHS, RHS,
                             "BO_GT");
      break;
    case BinaryOperatorKind::BO_SGT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_SGT + (Signed << 2)), LHS, RHS,
                             "BO_SGT");
      break;
    case BinaryOperatorKind::BO_LE:
      V = Builder.CreateICmp(Pred(Pred::ICMP_ULE + (Signed << 2)), LHS, RHS,
                             "BO_LE");
      break;
    case BinaryOperatorKind::BO_LT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_ULT + (Signed << 2)), LHS, RHS,
                             "BO_LT");
      break;
    case BinaryOperatorKind::BO_SLT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_SLT + (Signed << 2)), LHS, RHS,
                             "BO_SLT");
      break;
    case BinaryOperatorKind::BO_EQ:
      V = Builder.CreateICmpEQ(LHS, RHS, "BO_EQ");
      break;
    case BinaryOperatorKind::BO_NE:
      V = Builder.CreateICmpNE(LHS, RHS, "BO_NE");
      break;
    case BinaryOperatorKind::BO_Shl: {
      auto LHSType = LHS->getType();
      auto RHSType = RHS->getType();
      auto returnType =
          LHSType->getIntegerBitWidth() > RHSType->getIntegerBitWidth()
              ? LHSType
              : RHSType;
      V = Builder.CreateShl(Builder.CreateZExtOrTrunc(LHS, returnType),
                            Builder.CreateZExtOrTrunc(RHS, returnType),
                            "BO_Shl");
      break;
    }
    case BinaryOperatorKind::BO_Shr: {
      auto LHSType = LHS->getType();
      auto RHSType = RHS->getType();
      auto returnType =
          LHSType->getIntegerBitWidth() > RHSType->getIntegerBitWidth()
              ? LHSType
              : RHSType;
      if (Signed) {
        V = Builder.CreateAShr(Builder.CreateZExtOrTrunc(LHS, returnType),
                               Builder.CreateZExtOrTrunc(RHS, returnType),
                               "BO_Shr");
      } else {
        V = Builder.CreateLShr(Builder.CreateZExtOrTrunc(LHS, returnType),
                               Builder.CreateZExtOrTrunc(RHS, returnType),
                               "BO_Shr");
      }
      break;
    }
    case BinaryOperatorKind::BO_AShr: {
      auto LHSType = LHS->getType();
      auto RHSType = RHS->getType();
      auto returnType =
          LHSType->getIntegerBitWidth() > RHSType->getIntegerBitWidth()
              ? LHSType
              : RHSType;
      V = Builder.CreateAShr(Builder.CreateZExtOrTrunc(LHS, returnType),
                             Builder.CreateZExtOrTrunc(RHS, returnType),
                             "BO_AShr");
      break;
    }
    case BinaryOperatorKind::BO_And:
    case BinaryOperatorKind::BO_AsmAnd:
      V = Builder.CreateAnd(LHS, RHS, "BO_And");
      break;
    case BinaryOperatorKind::BO_Xor:
    case BinaryOperatorKind::BO_AsmXor:
      V = Builder.CreateXor(LHS, RHS, "BO_Xor");
      break;
    case BinaryOperatorKind::BO_Or:
    case BinaryOperatorKind::BO_AsmOr:
      V = Builder.CreateOr(LHS, RHS, "BO_Or");
      break;
    case BinaryOperatorKind::BO_Exp:
      V = CGM.emitExp(LHS, RHS, Signed);
      break;
    default:
      assert(false);
      __builtin_unreachable();
    }
  } else if (BO->isLogicalOp()) {
    if (BO->getOpcode() == BO_LAnd) {
      llvm::BasicBlock *ContBlock = CGF.createBasicBlock("land.end");
      llvm::BasicBlock *RHSBlock = CGF.createBasicBlock("land.rhs");
      CGF.emitBranchOnBoolExpr(BO->getLHS(), RHSBlock, ContBlock);

      llvm::PHINode *PN = llvm::PHINode::Create(CGF.BoolTy, 2, "", ContBlock);
      for (llvm::pred_iterator PI = pred_begin(ContBlock),
                               PE = pred_end(ContBlock);
           PI != PE; ++PI)
        PN->addIncoming(Builder.getFalse(), *PI);

      Builder.SetInsertPoint(RHSBlock);
      llvm::Value *RHSCond = CGF.emitBoolExpr(BO->getRHS())->load(Builder, CGM);
      Builder.CreateBr(ContBlock);
      Builder.SetInsertPoint(ContBlock);
      PN->addIncoming(RHSCond, RHSBlock);

      V = PN;
    } else if (BO->getOpcode() == BO_LOr) {
      llvm::BasicBlock *ContBlock = CGF.createBasicBlock("lor.end");
      llvm::BasicBlock *RHSBlock = CGF.createBasicBlock("lor.rhs");
      CGF.emitBranchOnBoolExpr(BO->getLHS(), ContBlock, RHSBlock);

      llvm::PHINode *PN = llvm::PHINode::Create(CGF.BoolTy, 2, "", ContBlock);
      for (llvm::pred_iterator PI = pred_begin(ContBlock),
                               PE = pred_end(ContBlock);
           PI != PE; ++PI)
        PN->addIncoming(Builder.getTrue(), *PI);

      Builder.SetInsertPoint(RHSBlock);
      llvm::Value *RHSCond = CGF.emitBoolExpr(BO->getRHS())->load(Builder, CGM);
      Builder.CreateBr(ContBlock);
      Builder.SetInsertPoint(ContBlock);
      PN->addIncoming(RHSCond, RHSBlock);

      V = PN;
    } else if (BO->getOpcode() == BO_LXor) {
      // TODO: implement LXor.
      assert(false && "logical xor not implemented");
      __builtin_unreachable();
    } else {
      assert(false && "unknown logical op");
      __builtin_unreachable();
    }
  }
  return ExprValue::getRValue(BO, V);
}

ExprValuePtr ExprEmitter::visit(const CastExpr *CE) {
  ExprValuePtr InVal = visit(CE->getSubExpr());
  if (CE->getCastKind() == CastKind::None) {
    return InVal;
  }

  const Type *OrigInTy = CE->getSubExpr()->getType().get();
  const Type *OrigOutTy = CE->getType().get();
  llvm::Value *In = nullptr;
  if (!InVal->isTuple())
    In = InVal->load(Builder, CGM);
  switch (CE->getCastKind()) {
  case CastKind::None:
    assert(false);
    __builtin_unreachable();
  case CastKind::LValueToRValue: {
    if (InVal->isTuple()) {
      auto InValT = dynamic_cast<const ExprValueTuple *>(InVal.get());
      assert(InValT);
      auto Ins = InValT->load(Builder, CGM);
      return ExprValueTuple::getRValue(CE, Ins);
    }
    return ExprValue::getRValue(CE, In);
  }
  case CastKind::IntegralCast: {
    auto OutTy = dynamic_cast<const IntegerType *>(OrigOutTy);
    auto InTy = dynamic_cast<const IntegerType *>(OrigInTy);
    assert(InTy != nullptr && OutTy != nullptr &&
           "IntegralCast should have int");
    llvm::Type *OutLLVMTy = CGM.getLLVMType(OutTy);
    llvm::Value *Out;
    if (InTy->isSigned()) {
      Out = Builder.CreateSExtOrTrunc(In, OutLLVMTy);
    } else {
      Out = Builder.CreateZExtOrTrunc(In, OutLLVMTy);
    }
    return ExprValue::getRValue(CE, Out);
  }
  case CastKind::FixedBytesCast: {
    auto OutTy = dynamic_cast<const FixedBytesType *>(OrigOutTy);
    auto InTy = dynamic_cast<const FixedBytesType *>(OrigInTy);
    assert(InTy != nullptr && OutTy != nullptr &&
           "FixedBytesCast should have FixedBytes");
    llvm::Type *OutLLVMTy = CGM.getLLVMType(OutTy);
    llvm::Value *Out;
    Out = Builder.CreateZExtOrTrunc(In, OutLLVMTy);
    return ExprValue::getRValue(CE, Out);
  }
  case CastKind::TypeCast: {
    if (OrigInTy->getCategory() == OrigOutTy->getCategory()) {
      if (OrigInTy->getCategory() == Type::Category::Tuple) {
        auto InValT = dynamic_cast<const ExprValueTuple *>(InVal.get());
        assert(InValT);
        auto Ins = InValT->load(Builder, CGM);
        return ExprValueTuple::getRValue(CE, Ins);
      }
      return ExprValue::getRValue(CE, In);
    }
    if (OrigInTy->getCategory() == Type::Category::Tuple &&
        OrigOutTy->getCategory() == Type::Category::ReturnTuple) {
      auto InValT = dynamic_cast<const ExprValueTuple *>(InVal.get());
      assert(InValT);
      auto Ins = InValT->load(Builder, CGM);
      auto RTy = dynamic_cast<const ReturnTupleType *>(OrigOutTy);
      llvm::Value *Out = llvm::ConstantAggregateZero::get(RTy->getLLVMType());
      unsigned Index = 0;
      for (auto Val : Ins) {
        Out = Builder.CreateInsertValue(Out, Val, {Index++});
      }
      return ExprValue::getRValue(CE, Out);
    }
    if (dynamic_cast<const AddressType *>(OrigInTy) ||
        dynamic_cast<const ContractType *>(OrigInTy) ||
        dynamic_cast<const AddressType *>(OrigOutTy)) {
      return ExprValue::getRValue(
          CE, Builder.CreateZExtOrTrunc(
                  In, Builder.getIntNTy(OrigOutTy->getBitNum())));
    }
    if (dynamic_cast<const BooleanType *>(OrigInTy)) {
      llvm::Value *Out = Builder.CreateZExt(In, CGM.getLLVMType(OrigOutTy));
      return ExprValue::getRValue(CE, Out);
    }
    if (dynamic_cast<const BooleanType *>(OrigOutTy)) {
      llvm::Value *Out = Builder.CreateICmpNE(
          In, llvm::ConstantInt::getNullValue(In->getType()));
      return ExprValue::getRValue(CE, Out);
    }
    if ((dynamic_cast<const StringType *>(OrigInTy) &&
         dynamic_cast<const BytesType *>(OrigOutTy)) ||
        (dynamic_cast<const BytesType *>(OrigInTy) &&
         dynamic_cast<const StringType *>(OrigOutTy))) {
      llvm::Value *Out =
          llvm::ConstantAggregateZero::get(CGM.getLLVMType(OrigOutTy));
      Out = Builder.CreateInsertValue(Out, Builder.CreateExtractValue(In, {0}),
                                      {0});
      Out = Builder.CreateInsertValue(Out, Builder.CreateExtractValue(In, {1}),
                                      {1});
      return ExprValue::getRValue(CE, Out);
    }
    if (dynamic_cast<const IntegerType *>(OrigOutTy) &&
        dynamic_cast<const StringType *>(OrigInTy)) {
      // XXX: it's should be allowed in assembly only
      llvm::Value *Dst = Builder.CreateAlloca(CGM.Int8Ty, Builder.getInt16(32));
      llvm::Value *Ptr = Builder.CreateBitCast(Dst, CGM.Int256PtrTy);
      Builder.CreateStore(Builder.getIntN(256, 0), Ptr);
      llvm::Value *Length = Builder.CreateExtractValue(In, {0});
      llvm::Value *Src = Builder.CreateExtractValue(In, {1});
      CGM.emitMemcpy(Dst, Src, Builder.CreateZExtOrTrunc(Length, CGM.Int32Ty));
      return ExprValue::getRValue(
          CE, CGM.getEndianlessValue(Builder.CreateLoad(Ptr)));
    }
    assert(false);
    break;
  }
  }
  return ExprValue::getRValue(CE, nullptr);
}

ExprValuePtr ExprEmitter::visit(const TupleExpr *TE) {
  if (TE->isInlineArray()) {
    assert(false && "InlineArray is not yet supported");
  } else {
    std::vector<ExprValuePtr> Values;
    std::vector<TypePtr> Types;
    for (auto const &comp : TE->getComponents()) {
      if (comp) {
        auto Expr = visit(comp);
        Values.emplace_back(std::move(Expr));
      } else {
        Values.emplace_back(std::make_shared<ExprValueSlot>());
      }
    }
    return std::make_shared<ExprValueTuple>(
        TE->getType().get(), ValueKind::VK_SValue, std::move(Values));
  }
}

ExprValuePtr ExprEmitter::visit(const DirectValueExpr *DVE) {
  return ExprValue::getRValue(DVE, DVE->getValue());
}

ExprValuePtr ExprEmitter::visit(const ReturnTupleExpr *RTE) {
  assert(RTE->getCalleeExpr()->getType()->getCategory() ==
         Type::Category::ReturnTuple);
  auto Callee = visit(RTE->getCalleeExpr());
  auto &DirectValues = RTE->getDirectValues();
  if (Callee->isTuple()) {
    auto Vals =
        dynamic_cast<ExprValueTuple *>(Callee.get())->load(Builder, CGM);
    assert(Vals.size() == DirectValues.size());
    for (unsigned I = 0; I < DirectValues.size(); ++I)
      DirectValues[I]->setValue(Vals[I]);
  } else {
    auto CalleeValue = Callee->load(Builder, CGM);
    for (unsigned I = 0; I < DirectValues.size(); ++I)
      DirectValues[I]->setValue(Builder.CreateExtractValue(CalleeValue, {I}));
  }
  return visit(RTE->getTupleExpr());
}

ExprValuePtr ExprEmitter::visit(const ParenExpr *PE) {
  return visit(PE->getSubExpr());
}

ExprValuePtr ExprEmitter::visit(const Identifier *ID) {
  if (ID->isSpecialIdentifier()) {
    switch (ID->getSpecialIdentifier()) {
    case Identifier::SpecialIdentifier::this_: {
      llvm::Value *Val = CGM.getEndianlessValue(CGM.emitGetAddress());
      return ExprValue::getRValue(ID, Val);
    }
    default:
      assert(false && "only SpecialIdentifier::this can change to address");
      __builtin_unreachable();
    }
  }
  const Decl *D = ID->getCorrespondDecl();

  if (auto *VD = dynamic_cast<const VarDecl *>(D)) {
    const Type *Ty = VD->getType().get();
    if (VD->isStateVariable()) {
      return std::make_shared<ExprValue>(Ty, ValueKind::VK_SValue,
                                         CGM.getStateVarAddr(VD));
    } else {
      return std::make_shared<ExprValue>(Ty, ValueKind::VK_LValue,
                                         CGF.getAddrOfLocalVar(VD));
    }
  }
  assert(false && "unknown Identifier");
  __builtin_unreachable();
}

ExprValuePtr ExprEmitter::visit(const CallExpr *CE) {
  return CGF.emitCallExpr(CE);
}

void ExprEmitter::emitCheckArrayOutOfBound(llvm::Value *ArrSz,
                                           llvm::Value *Index) {
  using namespace std::string_literals;
  static const std::string Message = "Array out of bound"s;
  llvm::Value *MessageValue =
      createGlobalStringPtr(CGF.getLLVMContext(), CGM.getModule(), Message);

  llvm::BasicBlock *Continue = CGF.createBasicBlock("continue");
  llvm::BasicBlock *Revert = CGF.createBasicBlock("revert");

  llvm::Value *OutOfBound = Builder.CreateICmpUGE(
      Index, Builder.CreateZExtOrTrunc(ArrSz, Index->getType()));
  Builder.CreateCondBr(OutOfBound, Revert, Continue);

  Builder.SetInsertPoint(Revert);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
}

ExprValuePtr ExprEmitter::visit(const IndexAccess *IA) {
  ExprValuePtr Base = visit(IA->getBase());
  ExprValuePtr Index = visit(IA->getIndex());
  const Type *Ty = IA->getType().get();

  if (const auto *MType = dynamic_cast<const MappingType *>(Base->getType())) {
    llvm::Value *Pos =
        CGM.getEndianlessValue(Builder.CreateLoad(Base->getValue()));
    llvm::Value *Key;
    if (MType->getKeyType()->isDynamic()) {
      Key = Index->load(Builder, CGM);
    } else {
      Key = CGM.getEndianlessValue(
          Builder.CreateZExtOrTrunc(Index->load(Builder, CGM), CGF.Int256Ty));
    }
    llvm::Value *Bytes = CGM.emitConcatBytes({Key, Pos});
    llvm::Value *AddressPtr = Builder.CreateAlloca(CGF.Int256Ty);
    Builder.CreateStore(CGM.emitKeccak256(Bytes), AddressPtr);
    return std::make_shared<ExprValue>(Ty, ValueKind::VK_SValue, AddressPtr);
  }
  if (const auto *ArrTy = dynamic_cast<const ArrayType *>(Base->getType())) {
    return arrayIndexAccess(Base, Index->load(Builder, CGM),
                            ArrTy->getElementType().get(), ArrTy,
                            IA->isStateVariable());
  }
  assert(false && "unknown IndexAccess type");
  __builtin_unreachable();
}

ExprValuePtr ExprEmitter::visit(const MemberExpr *ME) {
  if (ME->getName()->isSpecialIdentifier()) {
    switch (ME->getName()->getSpecialIdentifier()) {
    case Identifier::SpecialIdentifier::msg_sender: {
      llvm::Value *Val = CGM.getEndianlessValue(CGM.emitGetCaller());
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::msg_value: {
      llvm::Value *Val = Builder.CreateZExt(
          CGM.getEndianlessValue(CGM.emitGetCallValue()), CGF.Int256Ty);
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::msg_data: {
      llvm::Value *CallDataSize = CGM.emitGetCallDataSize();
      llvm::Value *ValPtr = Builder.CreateAlloca(CGF.Int8Ty, CallDataSize);
      CGM.emitCallDataCopy(ValPtr, Builder.getInt32(0), CallDataSize);

      llvm::Value *Bytes = llvm::ConstantAggregateZero::get(CGF.BytesTy);
      Bytes = Builder.CreateInsertValue(
          Bytes, Builder.CreateZExt(CallDataSize, CGF.Int256Ty), {0});
      Bytes = Builder.CreateInsertValue(Bytes, ValPtr, {1});
      return ExprValue::getRValue(ME, Bytes);
    }
    case Identifier::SpecialIdentifier::msg_sig: {
      llvm::Value *ValPtr = Builder.CreateAlloca(Builder.getInt32Ty());
      CGM.emitCallDataCopy(Builder.CreateBitCast(ValPtr, CGF.Int8PtrTy),
                           Builder.getInt32(0), Builder.getInt32(4));
      llvm::Value *Val = Builder.CreateLoad(ValPtr);
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::tx_gasprice: {
      llvm::Value *Val =
          Builder.CreateZExt(CGM.emitGetTxGasPrice(), CGF.Int256Ty);
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::tx_origin: {
      llvm::Value *Val = CGM.getEndianlessValue(CGM.emitGetTxOrigin());
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::block_coinbase: {
      llvm::Value *Val = CGM.getEndianlessValue(CGM.emitGetBlockCoinbase());
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::block_difficulty: {
      llvm::Value *Val = CGM.emitGetBlockDifficulty();
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::block_gaslimit: {
      llvm::Value *Val = CGM.emitGetBlockGasLimit();
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::block_number: {
      llvm::Value *Val = CGM.emitGetBlockNumber();
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::now:
    case Identifier::SpecialIdentifier::block_timestamp: {
      llvm::Value *Val = CGM.emitGetBlockTimestamp();
      return ExprValue::getRValue(ME, Val);
    }
    case Identifier::SpecialIdentifier::address_balance: {
      llvm::Value *Address = CGF.emitExpr(ME->getBase())->load(Builder, CGM);
      llvm::Value *Val =
          CGM.emitGetExternalBalance(CGM.getEndianlessValue(Address));
      return ExprValue::getRValue(ME, Builder.CreateZExt(Val, CGF.Int256Ty));
    }
    default:
      assert(false && "unsupported special member access");
      __builtin_unreachable();
    }
  } else {
    ExprValuePtr StructValue = visit(ME->getBase());
    auto ST = dynamic_cast<const StructType *>(ME->getBase()->getType().get());

    assert(ST && "StructType is expected here.");
    assert(ST->hasElement(ME->getName()->getName().str()));

    unsigned ElementIndex = ST->getElementIndex(ME->getName()->getName().str());
    return structIndexAccess(StructValue, ElementIndex, ST);
  }

  assert(false && "unsupported non-special member access");
  __builtin_unreachable();
}

ExprValuePtr ExprEmitter::visit(const BooleanLiteral *BL) {
  return ExprValue::getRValue(BL, Builder.getInt1(BL->getValue()));
}

ExprValuePtr ExprEmitter::visit(const StringLiteral *SL) {
  const std::string &StringData = SL->getValue();
  llvm::Value *String = llvm::ConstantAggregateZero::get(CGF.StringTy);
  llvm::Constant *Ptr =
      createGlobalStringPtr(CGF.getLLVMContext(), CGM.getModule(), StringData);
  String = Builder.CreateInsertValue(
      String, Builder.getIntN(256, StringData.size()), {0});
  String = Builder.CreateInsertValue(String, Ptr, {1});

  return ExprValue::getRValue(SL, String);
}

ExprValuePtr ExprEmitter::visit(const NumberLiteral *NL) {
  return ExprValue::getRValue(NL, Builder.getInt(NL->getValue()));
}

ExprValuePtr ExprEmitter::visit(const AsmIdentifier *YI) {
  const Decl *D = YI->getCorrespondDecl();

  if (auto *VD = dynamic_cast<const AsmVarDecl *>(D)) {
    return std::make_shared<ExprValue>(
        VD->getType().get(), ValueKind::VK_LValue, CGF.getAddrOfLocalVar(VD));
  }
  __builtin_unreachable();
}

const Identifier *ExprEmitter::resolveIdentifier(const Expr *E) {
  if (auto Id = dynamic_cast<const Identifier *>(E)) {
    return Id;
  }
  return nullptr;
}
} // namespace soll::CodeGen