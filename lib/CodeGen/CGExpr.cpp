// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include <llvm/IR/CFG.h>

namespace soll::CodeGen {

class ExprEmitter {
public:
  CodeGenFunction &CGF;
  llvm::IRBuilder<llvm::ConstantFolder> &Builder;
  llvm::LLVMContext &VMContext;
  ExprEmitter(CodeGenFunction &CGF)
      : CGF(CGF), Builder(CGF.getBuilder()), VMContext(CGF.getLLVMContext()) {}
  ExprValue visit(const Expr *E) {
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
    if (auto YI = dynamic_cast<const YulIdentifier *>(E)) {
      return visit(YI);
    }
    if (auto YL = dynamic_cast<const YulLiteral *>(E)) {
      return visit(YL);
    }
    if (auto YA = dynamic_cast<const YulAssignment *>(E)) {
      return visit(YA);
    }
    assert(false && "unknown Expr!");
    __builtin_unreachable();
  }

private:
  ExprValue visit(const UnaryOperator *UO) {
    ExprValue V;
    if (UO->isArithmeticOp()) {
      const Expr *SubExpr = UO->getSubExpr();
      ExprValue subVal = visit(SubExpr);
      switch (UO->getOpcode()) {
      case UnaryOperatorKind::UO_Plus:
        V = subVal;
        break;
      case UnaryOperatorKind::UO_Minus:
        V = ExprValue::getRValue(
            SubExpr,
            Builder.CreateNeg(subVal.load(Builder, CGF.getCodeGenModule())));
        break;
      case UnaryOperatorKind::UO_Not:
        V = ExprValue::getRValue(
            SubExpr,
            Builder.CreateNot(subVal.load(Builder, CGF.getCodeGenModule())));
        break;
      case UnaryOperatorKind::UO_LNot:
        V = ExprValue::getRValue(
            SubExpr,
            Builder.CreateICmpEQ(subVal.load(Builder, CGF.getCodeGenModule()),
                                 Builder.getInt1(false)));
        break;
      default:;
      }
    } else {
      const Expr *SubExpr = UO->getSubExpr();
      ExprValue subVal = visit(SubExpr);
      llvm::Value *Value = subVal.load(Builder, CGF.getCodeGenModule());
      switch (UO->getOpcode()) {
      case UnaryOperatorKind::UO_PostInc:
        subVal.store(Builder, CGF.getCodeGenModule(),
                     Builder.CreateAdd(
                         Value, llvm::ConstantInt::get(Value->getType(), 1)));
        V = ExprValue::getRValue(SubExpr, Value);
        break;
      case UnaryOperatorKind::UO_PostDec:
        subVal.store(Builder, CGF.getCodeGenModule(),
                     Builder.CreateSub(
                         Value, llvm::ConstantInt::get(Value->getType(), 1)));
        V = ExprValue::getRValue(SubExpr, Value);
        break;
      case UnaryOperatorKind::UO_PreInc:
        Value = Builder.CreateAdd(Value,
                                  llvm::ConstantInt::get(Value->getType(), 1));
        subVal.store(Builder, CGF.getCodeGenModule(), Value);
        V = ExprValue::getRValue(SubExpr, Value);
        break;
      case UnaryOperatorKind::UO_PreDec:
        Value = Builder.CreateSub(Value,
                                  llvm::ConstantInt::get(Value->getType(), 1));
        subVal.store(Builder, CGF.getCodeGenModule(), Value);
        V = ExprValue::getRValue(SubExpr, Value);
        break;
      default:
        assert(false && "unknown op");
        __builtin_unreachable();
      }
    }
    return V;
  }

  static bool isSigned(const Type *Ty) {
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

  ExprValue visit(const BinaryOperator *BO) {
    const Type *Ty = BO->getType().get();
    llvm::Value *V = nullptr;
    if (BO->isAssignmentOp()) {
      ExprValue LHSVar = visit(BO->getLHS());
      ExprValue RHSVar = visit(BO->getRHS());
      llvm::Value *RHSVal = RHSVar.load(Builder, CGF.getCodeGenModule());
      if (BO->getOpcode() == BO_Assign) {
        LHSVar.store(Builder, CGF.getCodeGenModule(), RHSVal);
      } else {
        const bool Signed = isSigned(Ty);
        llvm::Value *LHSVal = LHSVar.load(Builder, CGF.getCodeGenModule());
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
        LHSVar.store(Builder, CGF.getCodeGenModule(), LHSVal);
      }
      V = RHSVal;
    } else if (BO->isAdditiveOp() || BO->isMultiplicativeOp() ||
               BO->isComparisonOp() || BO->isShiftOp() || BO->isBitwiseOp()) {
      const bool Signed = isSigned(Ty);

      using Pred = llvm::CmpInst::Predicate;
      llvm::Value *LHS =
          visit(BO->getLHS()).load(Builder, CGF.getCodeGenModule());
      llvm::Value *RHS =
          visit(BO->getRHS()).load(Builder, CGF.getCodeGenModule());
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
      case BinaryOperatorKind::BO_LE:
        V = Builder.CreateICmp(Pred(Pred::ICMP_ULE + (Signed << 2)), LHS, RHS,
                               "BO_LE");
        break;
      case BinaryOperatorKind::BO_LT:
        V = Builder.CreateICmp(Pred(Pred::ICMP_ULT + (Signed << 2)), LHS, RHS,
                               "BO_LT");
        break;
      case BinaryOperatorKind::BO_EQ:
        V = Builder.CreateICmpEQ(LHS, RHS, "BO_EQ");
        break;
      case BinaryOperatorKind::BO_NE:
        V = Builder.CreateICmpNE(LHS, RHS, "BO_NE");
        break;
      case BinaryOperatorKind::BO_Shl:
        V = Builder.CreateShl(LHS, RHS, "BO_Shl");
        break;
      case BinaryOperatorKind::BO_Shr:
        if (Signed) {
          V = Builder.CreateAShr(LHS, RHS, "BO_Shr");
        } else {
          V = Builder.CreateLShr(LHS, RHS, "BO_Shr");
        }
        break;
      case BinaryOperatorKind::BO_And:
        V = Builder.CreateAnd(LHS, RHS, "BO_And");
        break;
      case BinaryOperatorKind::BO_Xor:
        V = Builder.CreateXor(LHS, RHS, "BO_Xor");
        break;
      case BinaryOperatorKind::BO_Or:
        V = Builder.CreateOr(LHS, RHS, "BO_Or");
        break;
      default:;
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
        llvm::Value *RHSCond = CGF.emitBoolExpr(BO->getRHS())
                                   .load(Builder, CGF.getCodeGenModule());
        RHSBlock = Builder.GetInsertBlock();

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
        llvm::Value *RHSCond = CGF.emitBoolExpr(BO->getRHS())
                                   .load(Builder, CGF.getCodeGenModule());
        RHSBlock = Builder.GetInsertBlock();

        Builder.SetInsertPoint(ContBlock);
        PN->addIncoming(RHSCond, RHSBlock);

        V = PN;
      } else {
        assert(false && "unknown logical op");
        __builtin_unreachable();
      }
    }
    return ExprValue::getRValue(BO, V);
  }

  ExprValue visit(const CastExpr *CE) {
    const Type *OrigInTy = CE->getSubExpr()->getType().get();
    const Type *OrigOutTy = CE->getType().get();
    llvm::Value *In =
        visit(CE->getSubExpr()).load(Builder, CGF.getCodeGenModule());

    switch (CE->getCastKind()) {
    case CastKind::LValueToRValue: {
      return ExprValue::getRValue(CE, In);
    }
    case CastKind::IntegralCast: {
      auto OutTy = dynamic_cast<const IntegerType *>(OrigOutTy);
      auto InTy = dynamic_cast<const IntegerType *>(OrigInTy);
      assert(InTy != nullptr && OutTy != nullptr &&
             "IntegralCast should have int");
      llvm::Type *OutLLVMTy = CGF.getCodeGenModule().getLLVMType(OutTy);
      llvm::Value *Out;
      if (InTy->isSigned()) {
        Out = Builder.CreateSExtOrTrunc(In, OutLLVMTy);
      } else {
        Out = Builder.CreateZExtOrTrunc(In, OutLLVMTy);
      }
      return ExprValue::getRValue(CE, Out);
    }
    case CastKind::TypeCast: {
      // TODO : address to sign
      if (auto InTy = dynamic_cast<const AddressType *>(OrigInTy)) {
        if (auto OutTy = dynamic_cast<const IntegerType *>(OrigOutTy)) {
          assert(InTy->getBitNum() <= OutTy->getBitNum() &&
                 "Integer Type too small");
          llvm::Value *Out = Builder.CreateZExtOrTrunc(
              In, Builder.getIntNTy(OutTy->getBitNum()));
          return ExprValue::getRValue(CE, Out);
        }
      }
      if (auto InTy = dynamic_cast<const IntegerType *>(OrigInTy)) {
        assert(!InTy->isSigned() && "Cannot cast from signed to address");
        if (auto OutTy = dynamic_cast<const AddressType *>(OrigOutTy)) {
          llvm::Value *Out = Builder.CreateZExtOrTrunc(
              In, Builder.getIntNTy(OutTy->getBitNum()));
          return ExprValue::getRValue(CE, Out);
        }
      }
      break;
    }
    }
    return ExprValue::getRValue(CE, nullptr);
  }

  ExprValue visit(const ParenExpr *PE) { return visit(PE->getSubExpr()); }

  ExprValue visit(const Identifier *ID) {
    const Decl *D = ID->getCorrespondDecl();

    if (auto *VD = dynamic_cast<const VarDecl *>(D)) {
      const Type *Ty = VD->GetType().get();
      if (VD->isStateVariable()) {
        return ExprValue(Ty, ValueKind::VK_SValue,
                         CGF.getCodeGenModule().getStateVarAddr(VD));
      } else {
        return ExprValue(Ty, ValueKind::VK_LValue, CGF.getAddrOfLocalVar(VD));
      }
    }
    assert(false && "unknown Identifier");
    __builtin_unreachable();
  }

  ExprValue visit(const CallExpr *CE) { return CGF.emitCallExpr(CE); }

  llvm::Value *emitConcateBytes(llvm::ArrayRef<llvm::Value *> Values) {
    unsigned ArrayLength = 0;
    for (llvm::Value *Value : Values) {
      llvm::Type *Ty = Value->getType();
      ArrayLength += (Ty->getIntegerBitWidth() + 255) / 256;
    }

    llvm::Value *Array = Builder.CreateAlloca(
        CGF.Int256Ty, Builder.getInt32(ArrayLength), "concat");

    unsigned Index = 0;
    for (llvm::Value *Value : Values) {
      llvm::Type *Ty = Value->getType();
      llvm::Value *Ptr =
          Builder.CreateInBoundsGEP(Array, {Builder.getInt32(Index)});
      Builder.CreateStore(Builder.CreateZExt(Value, CGF.Int256Ty), Ptr);
      Index += (Ty->getIntegerBitWidth() + 255) / 256;
    }

    llvm::Value *Bytes = llvm::ConstantAggregateZero::get(CGF.BytesTy);
    Bytes = Builder.CreateInsertValue(
        Bytes, Builder.getIntN(256, ArrayLength * 32), {0});
    Bytes = Builder.CreateInsertValue(
        Bytes, Builder.CreateBitCast(Array, CGF.Int8PtrTy), {1});
    return Bytes;
  }

  void emitCheckArrayOutOfBound(llvm::Value *ArrSz, llvm::Value *Index) {
    using namespace std::string_literals;
    static const std::string Message = "Array out of bound"s;
    llvm::Value *MessageValue = createGlobalStringPtr(
        CGF.getLLVMContext(), CGF.getCodeGenModule().getModule(), Message);

    llvm::BasicBlock *Continue = CGF.createBasicBlock("continue");
    llvm::BasicBlock *Revert = CGF.createBasicBlock("revert");

    llvm::Value *OutOfBound = Builder.CreateICmpUGE(Index, ArrSz);
    Builder.CreateCondBr(OutOfBound, Revert, Continue);

    Builder.SetInsertPoint(Revert);
    CGF.getCodeGenModule().emitRevert(MessageValue,
                                      Builder.getInt32(Message.size()));
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(Continue);
  }

  ExprValue visit(const IndexAccess *IA) {
    ExprValue Base = visit(IA->getBase());
    ExprValue Index = visit(IA->getIndex());
    const Type *Ty = IA->getType().get();

    llvm::Function *Sha256 =
        CGF.getCodeGenModule().getModule().getFunction("solidity.sha256");

    if (dynamic_cast<const MappingType *>(Base.getType())) {
      // mapping : store i256 hash value in TempValueTable
      llvm::Value *MapAddress = Builder.CreateLoad(Base.getValue());
      llvm::Value *Bytes = emitConcateBytes(
          {CGF.getCodeGenModule().getEndianlessValue(Builder.CreateZExtOrTrunc(
               Index.load(Builder, CGF.getCodeGenModule()), CGF.Int256Ty)),
           CGF.getCodeGenModule().getEndianlessValue(MapAddress)});
      llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
      Builder.CreateStore(Builder.CreateCall(Sha256, {Bytes}), Address);
      return ExprValue(Ty, ValueKind::VK_SValue, Address);
    }
    if (const auto *ArrTy = dynamic_cast<const ArrayType *>(Base.getType())) {
      // Array Type : Fixed Size Mem Array, Fixed Sized Storage Array, Dynamic
      // Sized Storage Array
      // Require Index to be unsigned 256-bit Int

      llvm::Value *IndexValue = Index.load(Builder, CGF.getCodeGenModule());
      if (!IA->isStateVariable()) {
        // TODO : Assume only Integer Array
        llvm::Value *ArraySize = Builder.getIntN(256, ArrTy->getLength());
        emitCheckArrayOutOfBound(ArraySize, IndexValue);
        llvm::Value *Address = Builder.CreateInBoundsGEP(
            CGF.getCodeGenModule().getLLVMType(Ty), Base.getValue(),
            {Builder.getIntN(256, 0), IndexValue});
        return ExprValue(Ty, ValueKind::VK_LValue, Address);
      }

      llvm::Value *ArrayAddress = Builder.CreateLoad(Base.getValue());
      if (ArrTy->isDynamicSized()) {
        // load array size and check
        auto LengthTy = IntegerType::getIntN(256);
        ExprValue BaseLength(&LengthTy, ValueKind::VK_SValue, Base.getValue());
        llvm::Value *ArraySize =
            BaseLength.load(Builder, CGF.getCodeGenModule());
        emitCheckArrayOutOfBound(ArraySize, IndexValue);

        // load array position
        llvm::Value *Bytes = emitConcateBytes(
            {CGF.getCodeGenModule().getEndianlessValue(ArrayAddress)});
        ArrayAddress = Builder.CreateCall(Sha256, {Bytes});
      } else {
        // Fixed Size Storage Array
        llvm::Value *ArraySize = Builder.getIntN(256, ArrTy->getLength());
        emitCheckArrayOutOfBound(ArraySize, IndexValue);
      }

      unsigned BitPerElement = ArrTy->getElementType()->getBitNum();
      unsigned ElementPerSlot = 256 / BitPerElement;

      if (ElementPerSlot != 1) {
        llvm::Value *StorageIndex = Builder.CreateUDiv(
            IndexValue, Builder.getIntN(256, ElementPerSlot));
        llvm::Value *Shift = Builder.CreateURem(
            IndexValue, Builder.getIntN(256, ElementPerSlot));
        llvm::Value *ElemAddress =
            Builder.CreateAdd(ArrayAddress, StorageIndex);
        llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
        Builder.CreateStore(ElemAddress, Address);
        return ExprValue(Ty, ValueKind::VK_SValue, Address, Shift);
      } else {
        llvm::Value *ElemAddress = Builder.CreateAdd(ArrayAddress, IndexValue);
        llvm::Value *Address = Builder.CreateAlloca(CGF.Int256Ty);
        Builder.CreateStore(ElemAddress, Address);
        return ExprValue(Ty, ValueKind::VK_SValue, Address);
      }
    }
    assert(false && "unknown IndexAccess type");
    __builtin_unreachable();
  }

  ExprValue visit(const MemberExpr *ME) {
    if (ME->getName()->isSpecialIdentifier()) {
      switch (ME->getName()->getSpecialIdentifier()) {
      case Identifier::SpecialIdentifier::msg_sender: {
        return ExprValue::getRValue(ME, CGF.getCodeGenModule().emitGetCaller());
      }
      case Identifier::SpecialIdentifier::msg_value: {
        llvm::Value *Val =
            Builder.CreateZExt(CGF.getCodeGenModule().getEndianlessValue(
                                   CGF.getCodeGenModule().emitGetCallValue()),
                               CGF.Int256Ty);
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::msg_data: {
        llvm::Function *getCallDataSize =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getCallDataSize");
        llvm::Function *callDataCopy =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.callDataCopy");

        llvm::Value *CallDataSize = Builder.CreateCall(getCallDataSize, {});
        llvm::Constant *AllocSize = llvm::ConstantExpr::getSizeOf(CGF.Int8Ty);
        llvm::Instruction *Malloc = llvm::CallInst::CreateMalloc(
            Builder.GetInsertBlock(), CGF.Int64Ty, CGF.Int8Ty, AllocSize,
            CallDataSize, nullptr);
        llvm::Value *ValPtr = Builder.Insert(Malloc);
        Builder.CreateCall(callDataCopy,
                           {ValPtr, Builder.getInt32(0), CallDataSize});

        llvm::Value *Bytes = llvm::ConstantAggregateZero::get(CGF.BytesTy);
        Bytes = Builder.CreateInsertValue(
            Bytes, Builder.CreateZExt(CallDataSize, CGF.Int256Ty), {0});
        Bytes = Builder.CreateInsertValue(Bytes, ValPtr, {1});
        return ExprValue::getRValue(ME, Bytes);
      }
      case Identifier::SpecialIdentifier::msg_sig: {
        llvm::Value *ValPtr = Builder.CreateAlloca(Builder.getInt32Ty());
        llvm::Function *callDataCopy =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.callDataCopy");
        Builder.CreateCall(callDataCopy,
                           {Builder.CreateBitCast(ValPtr, CGF.Int8PtrTy),
                            Builder.getInt32(0), Builder.getInt32(4)});
        llvm::Value *Val = Builder.CreateLoad(ValPtr);
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::tx_gasprice: {
        llvm::Function *getTxGasPrice =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getTxGasPrice");
        llvm::Value *ValPtr = Builder.CreateAlloca(CGF.Int128Ty);
        Builder.CreateCall(getTxGasPrice, {ValPtr});
        llvm::Value *Val =
            Builder.CreateZExt(CGF.getCodeGenModule().getEndianlessValue(
                                   Builder.CreateLoad(ValPtr)),
                               CGF.Int256Ty);
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::tx_origin: {
        llvm::Function *getTxOrigin =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getTxOrigin");
        llvm::Value *ValPtr = Builder.CreateAlloca(CGF.AddressTy);
        Builder.CreateCall(getTxOrigin, {ValPtr});
        llvm::Value *Val = CGF.getCodeGenModule().getEndianlessValue(
            Builder.CreateLoad(ValPtr));
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::block_coinbase: {
        llvm::Function *getBlockCoinbase =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getBlockCoinbase");
        llvm::Value *ValPtr = Builder.CreateAlloca(CGF.AddressTy);
        Builder.CreateCall(getBlockCoinbase, {ValPtr});
        llvm::Value *Val = CGF.getCodeGenModule().getEndianlessValue(
            Builder.CreateLoad(ValPtr));
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::block_difficulty: {
        llvm::Function *getBlockDifficulty =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getBlockDifficulty");
        llvm::Value *ValPtr = Builder.CreateAlloca(CGF.Int256Ty);
        Builder.CreateCall(getBlockDifficulty, {ValPtr});

        llvm::Value *Val = CGF.getCodeGenModule().getEndianlessValue(
            Builder.CreateLoad(ValPtr));
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::block_gaslimit: {
        llvm::Function *getBlockGasLimit =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getBlockGasLimit");
        llvm::Value *Val = Builder.CreateZExt(
            Builder.CreateCall(getBlockGasLimit, {}), CGF.Int256Ty);
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::block_number: {
        llvm::Function *getBlockNumber =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getBlockNumber");
        llvm::Value *Val = Builder.CreateZExt(
            Builder.CreateCall(getBlockNumber, {}), CGF.Int256Ty);
        return ExprValue::getRValue(ME, Val);
      }
      case Identifier::SpecialIdentifier::now:
      case Identifier::SpecialIdentifier::block_timestamp: {
        llvm::Function *getBlockTimestamp =
            CGF.getCodeGenModule().getModule().getFunction(
                "ethereum.getBlockTimestamp");
        llvm::Value *Val = Builder.CreateZExt(
            Builder.CreateCall(getBlockTimestamp, {}), CGF.Int256Ty);
        return ExprValue::getRValue(ME, Val);
      }
      default:
        assert(false && "unsupported special member access");
        __builtin_unreachable();
      }
    }

    assert(false && "unsupported non-special member access");
    __builtin_unreachable();
  }

  ExprValue visit(const BooleanLiteral *BL) {
    return ExprValue::getRValue(BL, Builder.getInt1(BL->getValue()));
  }

  ExprValue visit(const StringLiteral *SL) {
    const std::string &StringData = SL->getValue();
    llvm::Value *String = llvm::ConstantAggregateZero::get(CGF.StringTy);
    llvm::Constant *Ptr = createGlobalStringPtr(
        CGF.getLLVMContext(), CGF.getCodeGenModule().getModule(), StringData);
    String = Builder.CreateInsertValue(
        String, Builder.getIntN(256, StringData.size()), {0});
    String = Builder.CreateInsertValue(String, Ptr, {1});

    return ExprValue::getRValue(SL, String);
  }

  ExprValue visit(const NumberLiteral *NL) {
    return ExprValue::getRValue(
        NL, Builder.getIntN(NL->getType()->getBitNum(), NL->getValue()));
  }

  ExprValue visit(const YulIdentifier *YId) {
    // TODO: implement this
  }

  ExprValue visit(const YulLiteral *YL) {
    // TODO: implement this
  }

  ExprValue visit(const YulAssignment *YAssign) {
    // TODO: implement this
  }

  const Identifier *resolveIdentifier(const Expr *E) {
    if (auto Id = dynamic_cast<const Identifier *>(E)) {
      return Id;
    }
    return nullptr;
  }
};

ExprValue CodeGenFunction::emitExpr(const Expr *E) {
  return ExprEmitter(*this).visit(E);
}

void CodeGenFunction::emitCallRequire(const CallExpr *CE) {
  // require function
  auto Arguments = CE->getArguments();

  std::string Message =
      dynamic_cast<const StringLiteral *>(Arguments[1])->getValue();
  llvm::Constant *MessageValue = createGlobalStringPtr(
      getLLVMContext(), getCodeGenModule().getModule(), Message);

  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");

  emitBranchOnBoolExpr(Arguments[0], Continue, Revert);

  Builder.SetInsertPoint(Revert);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
}

void CodeGenFunction::emitCallAssert(const CallExpr *CE) {
  // assert function
  auto Arguments = CE->getArguments();

  using namespace std::string_literals;
  static const std::string Message = "Assertion Fail"s;
  llvm::Constant *MessageValue = createGlobalStringPtr(
      getLLVMContext(), getCodeGenModule().getModule(), Message);

  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");

  emitBranchOnBoolExpr(Arguments[0], Continue, Revert);

  Builder.SetInsertPoint(Revert);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
}

void CodeGenFunction::emitCallRevert(const CallExpr *CE) {
  // revert function
  auto Arguments = CE->getArguments();
  llvm::StringRef Message =
      dynamic_cast<const StringLiteral *>(Arguments[0])->getValue();

  llvm::Constant *MessageValue = createGlobalStringPtr(
      getLLVMContext(), getCodeGenModule().getModule(), Message);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();
}

llvm::Value *CodeGenFunction::emitAddmod(const CallExpr *CE) {
  // addmod
  auto Arguments = CE->getArguments();
  llvm::Value *A = emitExpr(Arguments[0]).load(Builder, CGM);
  llvm::Value *B = emitExpr(Arguments[1]).load(Builder, CGM);
  llvm::Value *K = emitExpr(Arguments[2]).load(Builder, CGM);
  llvm::Value *addAB = Builder.CreateAdd(A, B);
  llvm::Value *res = Builder.CreateURem(addAB, K);
  return res;
}

llvm::Value *CodeGenFunction::emitMulmod(const CallExpr *CE) {
  // mulmod
  auto Arguments = CE->getArguments();
  llvm::Value *A = emitExpr(Arguments[0]).load(Builder, CGM);
  llvm::Value *B = emitExpr(Arguments[1]).load(Builder, CGM);
  llvm::Value *K = emitExpr(Arguments[2]).load(Builder, CGM);
  llvm::Value *addAB = Builder.CreateMul(A, B);
  llvm::Value *res = Builder.CreateURem(addAB, K);
  return res;
}

llvm::Value *CodeGenFunction::emitCallkeccak256(const CallExpr *CE) {
  // keccak256 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0]).load(Builder, CGM);
  return Builder.CreateCall(CGM.getModule().getFunction("solidity.keccak256"),
                            {MessageValue});
}

llvm::Value *CodeGenFunction::emitCallsha256(const CallExpr *CE) {
  // sha256 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0]).load(Builder, CGM);
  return Builder.CreateCall(CGM.getModule().getFunction("solidity.sha256"),
                            {MessageValue});
}

llvm::Value *CodeGenFunction::emitCallripemd160(const CallExpr *CE) {
  // ripemd160 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0]).load(Builder, CGM);
  return Builder.CreateCall(CGM.getModule().getFunction("solidity.ripemd160"),
                            {MessageValue});
}

ExprValue CodeGenFunction::emitCallExpr(const CallExpr *CE) {
  auto Callee = dynamic_cast<const Identifier *>(CE->getCalleeExpr());
  if (Callee->isSpecialIdentifier()) {
    switch (Callee->getSpecialIdentifier()) {
    case Identifier::SpecialIdentifier::require:
      emitCallRequire(CE);
      return ExprValue();
    case Identifier::SpecialIdentifier::assert_:
      emitCallAssert(CE);
      return ExprValue();
    case Identifier::SpecialIdentifier::revert:
      emitCallRevert(CE);
      return ExprValue();
    case Identifier::SpecialIdentifier::gasleft:
      assert(CE->getArguments().empty() && "gasleft require no arguments");
      return ExprValue::getRValue(CE, CGM.emitGetGasLeft());
    case Identifier::SpecialIdentifier::addmod:
      return ExprValue::getRValue(CE, emitAddmod(CE));
    case Identifier::SpecialIdentifier::mulmod:
      return ExprValue::getRValue(CE, emitMulmod(CE));
    case Identifier::SpecialIdentifier::keccak256:
      return ExprValue::getRValue(CE, emitCallkeccak256(CE));
    case Identifier::SpecialIdentifier::sha256:
      return ExprValue::getRValue(CE, emitCallsha256(CE));
    case Identifier::SpecialIdentifier::ripemd160:
      return ExprValue::getRValue(CE, emitCallripemd160(CE));
    case Identifier::SpecialIdentifier::ecrecover:
      assert(false && "ecrecover not supported yet");
      __builtin_unreachable();
    case Identifier::SpecialIdentifier::blockhash:
      assert(false && "blockhash not supported yet");
      __builtin_unreachable();
    default:
      assert(false && "special function not supported yet");
      __builtin_unreachable();
    }
  }
  std::vector<llvm::Value *> Args;
  for (auto Argument : CE->getArguments()) {
    Args.push_back(emitExpr(Argument).load(Builder, CGM));
  }
  if (auto FD =
          dynamic_cast<const FunctionDecl *>(Callee->getCorrespondDecl())) {
    llvm::Function *F = CGM.getModule().getFunction(CGM.getMangledName(FD));
    assert(F != nullptr && "undefined function");
    llvm::Value *Result = Builder.CreateCall(F, Args);
    return ExprValue::getRValue(CE, Result);
  }
  if (auto ED = dynamic_cast<const EventDecl *>(Callee->getCorrespondDecl())) {
    auto Params = ED->getParams()->getParams();
    auto Arguments = CE->getArguments();
    std::vector<llvm::Value *> Data(
        1, llvm::ConstantPointerNull::get(Int256PtrTy));
    std::vector<llvm::Value *> Topics(
        4, llvm::ConstantPointerNull::get(Int256PtrTy));
    uint IndexedCnt = 0;
    uint DataCnt = 0;

    if (ED->isAnonymous() == false) {
      Topics[IndexedCnt++] =
          Builder.CreateBitCast(CGM.getModule().getNamedGlobal(
                                    "solidity.event." + CGM.getMangledName(ED)),
                                Int256PtrTy);
    }

    // XXX: Multiple args and complex data type encoding not implemented yet.
    for (size_t i = 0; i < Arguments.size(); i++) {
      llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
      Builder.CreateStore(
          CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(Args[i], Int256Ty)),
          ValPtr);
      if (Params[i]->isIndexed()) {
        Topics[IndexedCnt++] = ValPtr;
      } else {
        Data[DataCnt++] = ValPtr;
      }
    }
    CGM.emitLog(Data[0], Builder.getInt32(32), Topics);
    return ExprValue();
  }
  assert(false && "Unhandle CallExprType CodeGen case.");
  __builtin_unreachable();
}

} // namespace soll::CodeGen
