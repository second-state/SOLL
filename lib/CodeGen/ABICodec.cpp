#include "ABICodec.h"

namespace soll::CodeGen {

llvm::Value *AbiEmitter::getEncodePackedTupleSize(
    const std::vector<std::pair<ExprValuePtr, bool>> &Values) {
  llvm::Value *Size = Builder.getIntN(32, 0);
  for (const auto &V : Values) {
    ExprValuePtr Value;
    bool IsStateVariable;
    std::tie(Value, IsStateVariable) = V;
    Size = Builder.CreateAdd(Size, getEncodePackedSize(Value, IsStateVariable));
  }
  return Size;
}
llvm::Value *AbiEmitter::getEncodeTupleSize(
    const std::vector<std::pair<ExprValuePtr, bool>> &Values) {
  llvm::Value *Size = Builder.getIntN(32, 0);
  for (const auto &V : Values) {
    ExprValuePtr Value;
    bool IsStateVariable;
    std::tie(Value, IsStateVariable) = V;
    if (Value->getType()->isDynamic()) {
      Size = Builder.CreateAdd(Size, Builder.getIntN(32, 32));
    }
    Size = Builder.CreateAdd(Size, getEncodeSize(Value, IsStateVariable));
  }
  return Size;
}
llvm::Value *AbiEmitter::emitEncodePackedTuple(
    llvm::Value *Int8Ptr,
    const std::vector<std::pair<ExprValuePtr, bool>> &Values) {
  for (const auto &V : Values) {
    ExprValuePtr Value;
    bool IsStateVariable;
    std::tie(Value, IsStateVariable) = V;
    Int8Ptr = emitEncodePacked(Int8Ptr, Value, IsStateVariable);
  }
  return Int8Ptr;
}
llvm::Value *AbiEmitter::emitEncodeTuple(
    llvm::Value *Int8Ptr,
    const std::vector<std::pair<ExprValuePtr, bool>> &Values) {
  llvm::Value *Int8PtrBegin = Int8Ptr;
  std::vector<std::pair<llvm::Value *, size_t>> DynamicPos;
  for (size_t I = 0; I < Values.size(); ++I) {
    ExprValuePtr Value;
    bool IsStateVariable;
    std::tie(Value, IsStateVariable) = Values[I];
    if (Value->getType()->isDynamic()) {
      DynamicPos.emplace_back(Int8Ptr, I);
      Int8Ptr = Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr,
                                          {Builder.getIntN(32, 32)});
    } else {
      Int8Ptr = emitEncode(Int8Ptr, Value, IsStateVariable);
    }
  }
  for (auto &DP : DynamicPos) {
    llvm::Value *Head;
    size_t I;
    std::tie(Head, I) = DP;
    ExprValuePtr Value;
    bool IsStateVariable;
    std::tie(Value, IsStateVariable) = Values[I];
    llvm::Value *TailPos = Builder.CreateZExtOrTrunc(
        Builder.CreatePtrDiff(Int8Ptr, Int8PtrBegin), CGM.Int256Ty);
    copyToInt8Ptr(Head, TailPos, false);
    Int8Ptr = emitEncode(Int8Ptr, Value, IsStateVariable);
  }
  return Int8Ptr;
}

std::pair<ExprValuePtr, llvm::Value *>
AbiEmitter::getDecode(llvm::Value *Int8Ptr, const Type *Ty) {
  auto Int32Ty = IntegerType::getIntN(32);
  if (!Ty)
    return {nullptr, nullptr};
  llvm::Type *ValueTy = CGM.getLLVMType(Ty);
  switch (Ty->getCategory()) {
  case Type::Category::String:
  case Type::Category::Bytes: {
    ExprValuePtr LengthExprValue;
    std::tie(LengthExprValue, Int8Ptr) = getDecode(Int8Ptr, &Int32Ty);
    llvm::Value *Length = LengthExprValue->load(Builder, CGM);

    llvm::Value *Array = Builder.CreateAlloca(CGF.Int8Ty, Length, "decode");
    llvm::Function *Memcpy = CGM.getModule().getFunction("solidity.memcpy");
    Builder.CreateCall(
        Memcpy, {Builder.CreateBitCast(Array, CGM.Int8PtrTy), Int8Ptr, Length});
    llvm::Value *Val = llvm::ConstantAggregateZero::get(ValueTy);
    Val = Builder.CreateInsertValue(
        Val, Builder.CreateZExtOrTrunc(Length, CGF.Int256Ty), {0});
    Val = Builder.CreateInsertValue(Val, Array, {1});

    llvm::Value *EncodeLength = Builder.CreateMul(
        Builder.CreateUDiv(Builder.CreateAdd(Length, Builder.getIntN(32, 31)),
                           Builder.getIntN(32, 32)),
        Builder.getIntN(32, 32));
    llvm::Value *NextInt8Ptr =
        Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr, {EncodeLength});
    return {ExprValue::getRValue(Ty, Val), NextInt8Ptr};
  }
  case Type::Category::Array: {
    assert(false && "This type is not available currently for abi.decode");
    __builtin_unreachable();
  }
  case Type::Category::ReturnTuple:
  case Type::Category::Tuple: {
    const auto *TupleTy = dynamic_cast<const TupleType *>(Ty);
    std::vector<llvm::Value *> Vals;
    llvm::Value *NextInt8Ptr;
    std::tie(Vals, NextInt8Ptr) = getDecodeTuple(Int8Ptr, TupleTy);
    return {ExprValueTuple::getRValue(TupleTy, Vals), NextInt8Ptr};
  }
  case Type::Category::Struct: {
    const auto *TupleTy = dynamic_cast<const TupleType *>(Ty);
    std::vector<llvm::Value *> Vals;
    llvm::Value *NextInt8Ptr;
    std::tie(Vals, NextInt8Ptr) = getDecodeTuple(Int8Ptr, TupleTy);
    const auto *StructTy = dynamic_cast<const StructType *>(Ty);
    llvm::Value *ReturnStruct =
        llvm::ConstantAggregateZero::get(StructTy->getLLVMType());
    unsigned Index = 0;
    for (auto Val : Vals) {
      ReturnStruct = Builder.CreateInsertValue(ReturnStruct, Val, {Index++});
    }
    return {ExprValue::getRValue(Ty, ReturnStruct), NextInt8Ptr};
  }
  case Type::Category::Contract:
  case Type::Category::Function:
  case Type::Category::Mapping:
    assert(false && "This type is not available currently for abi.decode");
    __builtin_unreachable();
  case Type::Category::FixedBytes: {
    llvm::Value *ValPtr = Builder.CreatePointerCast(
        Int8Ptr, llvm::PointerType::getUnqual(ValueTy));
    llvm::Value *Val = Builder.CreateLoad(ValueTy, ValPtr);
    llvm::Value *NextInt8Ptr =
        Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr, {Builder.getInt32(32)});
    return {ExprValue::getRValue(Ty, Val), NextInt8Ptr};
  }
  default: {
    llvm::Value *ValPtr = Builder.CreatePointerCast(
        Int8Ptr, llvm::PointerType::getUnqual(CGM.Int256Ty));
    llvm::Value *Val = Builder.CreateLoad(CGM.Int256Ty, ValPtr);
    Val = CGM.getEndianlessValue(Val);
    Val = Builder.CreateZExtOrTrunc(Val, ValueTy);
    llvm::Value *NextInt8Ptr = Builder.CreateInBoundsGEP(
        CGM.Int8Ty, Int8Ptr,
        {Builder.getInt32(ValueTy->getIntegerBitWidth() / 8)});
    return {ExprValue::getRValue(Ty, Val), NextInt8Ptr};
  }
  }
}
std::pair<std::vector<llvm::Value *>, llvm::Value *>
AbiEmitter::getDecodeTuple(llvm::Value *Int8Ptr, const TupleType *Ty) {
  auto Int32Ty = IntegerType::getIntN(32);
  const auto &ElementTypes = Ty->getElementTypes();
  std::vector<std::pair<llvm::Value *, size_t>> DynamicPos;
  std::vector<llvm::Value *> Vals(ElementTypes.size());
  llvm::Value *NextInt8Ptr = Int8Ptr;
  for (size_t I = 0; I < ElementTypes.size(); ++I) {
    if (ElementTypes[I]->isDynamic()) {
      ExprValuePtr PosExprValue;
      std::tie(PosExprValue, NextInt8Ptr) = getDecode(NextInt8Ptr, &Int32Ty);
      llvm::Value *Pos = PosExprValue->load(Builder, CGM);
      DynamicPos.emplace_back(
          Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr, {Pos}), I);
    } else {
      ExprValuePtr ValExprValue;
      std::tie(ValExprValue, NextInt8Ptr) =
          getDecode(NextInt8Ptr, ElementTypes[I].get());
      Vals[I] = ValExprValue->load(Builder, CGM);
    }
  }
  for (auto &DP : DynamicPos) {
    ExprValuePtr ValExprValue;
    llvm::Value *Pos;
    size_t I;
    std::tie(Pos, I) = DP;
    std::tie(ValExprValue, NextInt8Ptr) = getDecode(Pos, ElementTypes[I].get());
    Vals[I] = ValExprValue->load(Builder, CGM);
  }
  return {Vals, NextInt8Ptr};
}
llvm::Value *AbiEmitter::getArrayLength(const ExprValuePtr &Base,
                                        const ArrayType *ArrTy) {
  if (ArrTy->isDynamicSized()) {
    auto LengthTy = IntegerType::getIntN(256);
    llvm::Value *Value = Base->load(Builder, CGM);
    ExprValue BaseLength(&LengthTy, ValueKind::VK_SValue, Value);
    return BaseLength.load(Builder, CGM);
  } else {
    return Builder.getInt(ArrTy->getLength());
  }
}
unsigned AbiEmitter::getElementPerSlot(bool isStateVariable,
                                       const ArrayType *ArrTy) {
  if (!isStateVariable)
    return 1;
  unsigned BitPerElement = ArrTy->getElementType()->getBitNum();
  unsigned ElementPerSlot = 256 / BitPerElement;
  return ElementPerSlot;
}
llvm::Value *AbiEmitter::getEncodePackedSize(const ExprValuePtr &Value,
                                             bool IsStateVariable) {
  const Type *Ty = Value->getType();
  switch (Ty->getCategory()) {
  case Type::Category::String:
  case Type::Category::Bytes: {
    llvm::Value *Bytes = Value->load(Builder, CGM);
    llvm::Value *Length = Builder.CreateZExtOrTrunc(
        Builder.CreateExtractValue(Bytes, {0}), CGF.Int32Ty);
    return Length;
  }
  case Type::Category::Array: {
    const auto *ArrTy = dynamic_cast<const ArrayType *>(Ty);
    const auto ArrElementTy = ArrTy->getElementType();
    llvm::Value *Size = Builder.getIntN(32, 0);
    llvm::Value *ArrayLength =
        Builder.CreateZExtOrTrunc(getArrayLength(Value, ArrTy), CGM.Int32Ty);
    if (!ArrElementTy->isDynamic()) {
      llvm::Value *ArraySize = nullptr;
      if (ArrTy->isDynamicSized()) {
        ArraySize = Builder.CreateMul(
            ArrayLength, Builder.getIntN(32, ArrElementTy->getABIStaticSize()));
      } else {
        ArraySize = Builder.getIntN(32, ArrTy->getABIStaticSize());
      }
      return ArraySize;
    }

    llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *Start = Builder.GetInsertBlock();
    llvm::BasicBlock *Loop =
        llvm::BasicBlock::Create(VMContext, "loop", ThisFunc);
    llvm::BasicBlock *LoopEnd =
        llvm::BasicBlock::Create(VMContext, "loop_end", ThisFunc);
    llvm::BasicBlock *EndRecursive =
        llvm::BasicBlock::Create(VMContext, "recursive_end", ThisFunc);

    llvm::Value *Index = Builder.getIntN(32, 0);

    llvm::Value *Condition = Builder.CreateICmpULT(Index, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    Builder.SetInsertPoint(Loop);
    llvm::PHINode *PHIIndex = Builder.CreatePHI(CGM.Int32Ty, 2);
    llvm::PHINode *PHISize = Builder.CreatePHI(CGM.Int32Ty, 2);

    ExprValuePtr IndexAccessValue = ExprEmitter(CGF).arrayIndexAccess(
        Value, PHIIndex, ArrTy->getElementType().get(), ArrTy, IsStateVariable);

    llvm::Value *ResSize =
        getEncodePackedSize(IndexAccessValue, IsStateVariable);
    llvm::Value *NextIndex =
        Builder.CreateAdd(PHIIndex, Builder.getIntN(32, 1));
    llvm::Value *NextSize = Builder.CreateAdd(PHISize, ResSize);
    Builder.CreateBr(EndRecursive);
    Builder.SetInsertPoint(EndRecursive);

    Condition = Builder.CreateICmpULT(NextIndex, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    PHIIndex->addIncoming(Index, Start);
    PHIIndex->addIncoming(NextIndex, EndRecursive);
    PHISize->addIncoming(Size, Start);
    PHISize->addIncoming(NextSize, EndRecursive);

    Builder.SetInsertPoint(LoopEnd);
    PHISize = Builder.CreatePHI(CGM.Int32Ty, 2);
    PHISize->addIncoming(Size, Start);
    PHISize->addIncoming(NextSize, EndRecursive);
    return PHISize;
  }
  case Type::Category::Struct:
  case Type::Category::Tuple:
  case Type::Category::Contract:
  case Type::Category::Function:
  case Type::Category::Mapping:
    assert(false &&
           "This type is not available currently for abi.encodePacked");
    __builtin_unreachable();
  case Type::Category::Bool:
    return Builder.getIntN(32, 1);
  default:
    return Builder.getIntN(32, Ty->getBitNum() / 8);
  }
}
llvm::Value *AbiEmitter::getEncodeSize(const ExprValuePtr &Value,
                                       bool IsStateVariable) {
  const Type *Ty = Value->getType();
  switch (Ty->getCategory()) {
  case Type::Category::String:
  case Type::Category::Bytes: {
    llvm::Value *Bytes = Value->load(Builder, CGM);
    llvm::Value *Length = Builder.CreateZExtOrTrunc(
        Builder.CreateExtractValue(Bytes, {0}), CGF.Int32Ty);
    return Builder.CreateMul(
        Builder.CreateUDiv(Builder.CreateAdd(Length, Builder.getIntN(32, 63)),
                           Builder.getIntN(32, 32)),
        Builder.getIntN(32, 32));
  }
  case Type::Category::Array: {
    const auto *ArrTy = dynamic_cast<const ArrayType *>(Ty);
    const auto ArrElementTy = ArrTy->getElementType();
    llvm::Value *Size = Builder.getIntN(32, ArrTy->isDynamicSized() ? 32 : 0);
    llvm::Value *ArrayLength =
        Builder.CreateZExtOrTrunc(getArrayLength(Value, ArrTy), CGM.Int32Ty);
    if (!ArrElementTy->isDynamic()) {
      llvm::Value *ArraySize = nullptr;
      if (ArrTy->isDynamicSized()) {
        ArraySize = Builder.CreateMul(
            ArrayLength, Builder.getIntN(32, ArrElementTy->getABIStaticSize()));
      } else {
        ArraySize = Builder.getIntN(32, ArrTy->getABIStaticSize());
      }
      return Builder.CreateAdd(Size, ArraySize);
    }
    Size = Builder.CreateAdd(
        Size, Builder.CreateMul(ArrayLength, Builder.getIntN(32, 32)));

    llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *Start = Builder.GetInsertBlock();
    llvm::BasicBlock *Loop =
        llvm::BasicBlock::Create(VMContext, "loop", ThisFunc);
    llvm::BasicBlock *LoopEnd =
        llvm::BasicBlock::Create(VMContext, "loop_end", ThisFunc);
    llvm::BasicBlock *EndRecursive =
        llvm::BasicBlock::Create(VMContext, "recursive_end", ThisFunc);

    llvm::Value *Index = Builder.getIntN(32, 0);

    llvm::Value *Condition = Builder.CreateICmpULT(Index, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    Builder.SetInsertPoint(Loop);
    llvm::PHINode *PHIIndex = Builder.CreatePHI(CGM.Int32Ty, 2);
    llvm::PHINode *PHISize = Builder.CreatePHI(CGM.Int32Ty, 2);

    ExprValuePtr IndexAccessValue = ExprEmitter(CGF).arrayIndexAccess(
        Value, PHIIndex, ArrTy->getElementType().get(), ArrTy, IsStateVariable);

    llvm::Value *ResSize = getEncodeSize(IndexAccessValue, IsStateVariable);
    llvm::Value *NextIndex =
        Builder.CreateAdd(PHIIndex, Builder.getIntN(32, 1));
    llvm::Value *NextSize = Builder.CreateAdd(PHISize, ResSize);
    Builder.CreateBr(EndRecursive);
    Builder.SetInsertPoint(EndRecursive);

    Condition = Builder.CreateICmpULT(NextIndex, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    PHIIndex->addIncoming(Index, Start);
    PHIIndex->addIncoming(NextIndex, EndRecursive);
    PHISize->addIncoming(Size, Start);
    PHISize->addIncoming(NextSize, EndRecursive);

    Builder.SetInsertPoint(LoopEnd);
    PHISize = Builder.CreatePHI(CGM.Int32Ty, 2);
    PHISize->addIncoming(Size, Start);
    PHISize->addIncoming(NextSize, EndRecursive);
    return PHISize;
  }
  case Type::Category::Struct: {
    const auto *STy = dynamic_cast<const StructType *>(Ty);
    std::vector<std::pair<ExprValuePtr, bool>> Values;
    bool IsStateVariable = Value->getValueKind() == ValueKind::VK_SValue;
    unsigned ElementSize = STy->getElementSize();
    for (unsigned I = 0; I < ElementSize; ++I) {
      Values.emplace_back(ExprEmitter(CGF).structIndexAccess(Value, I, STy),
                          IsStateVariable);
    }
    return getEncodeTupleSize(Values);
  }
  case Type::Category::Tuple:
  case Type::Category::Contract:
  case Type::Category::Function:
  case Type::Category::Mapping:
    assert(false &&
           "This type is not available currently for abi.encodePacked");
    __builtin_unreachable();
  default:
    return Builder.getIntN(32, Ty->getABIStaticSize());
  }
}
llvm::Value *AbiEmitter::emitEncodePacked(llvm::Value *Int8Ptr,
                                          const ExprValuePtr &Value,
                                          bool IsStateVariable,
                                          bool IsArrayElement) {
  const Type *Ty = Value->getType();
  switch (Ty->getCategory()) {
  case Type::Category::String:
  case Type::Category::Bytes: {
    if (IsArrayElement) {
      assert(false &&
             "This type is not available currently for abi.encodePacked");
      __builtin_unreachable();
    }
    llvm::Value *Bytes = Value->load(Builder, CGM);
    return copyToInt8Ptr(Int8Ptr, Bytes, true);
  }
  case Type::Category::Array: {
    llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *Start = Builder.GetInsertBlock();
    llvm::BasicBlock *Loop =
        llvm::BasicBlock::Create(VMContext, "loop", ThisFunc);
    llvm::BasicBlock *LoopEnd =
        llvm::BasicBlock::Create(VMContext, "loop_end", ThisFunc);
    llvm::BasicBlock *EndRecursive =
        llvm::BasicBlock::Create(VMContext, "recursive_end", ThisFunc);

    const auto *ArrTy = dynamic_cast<const ArrayType *>(Ty);
    if (IsArrayElement && ArrTy->isDynamicSized()) {
      assert(false &&
             "This type is not available currently for abi.encodePacked");
      __builtin_unreachable();
    }
    llvm::Value *ArrayLength =
        Builder.CreateZExtOrTrunc(getArrayLength(Value, ArrTy), CGM.Int32Ty);
    llvm::Value *Index = Builder.getIntN(32, 0);

    llvm::Value *Condition = Builder.CreateICmpULT(Index, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    Builder.SetInsertPoint(Loop);
    llvm::PHINode *PHIIndex = Builder.CreatePHI(CGM.Int32Ty, 2);
    llvm::PHINode *PHIInt8Ptr = Builder.CreatePHI(CGM.Int8PtrTy, 2);

    ExprValuePtr IndexAccessValue = ExprEmitter(CGF).arrayIndexAccess(
        Value, PHIIndex, ArrTy->getElementType().get(), ArrTy, IsStateVariable);

    llvm::Value *NextInt8Ptr =
        emitEncodePacked(PHIInt8Ptr, IndexAccessValue, IsStateVariable, true);
    llvm::Value *NextIndex =
        Builder.CreateAdd(PHIIndex, Builder.getIntN(32, 1));
    Builder.CreateBr(EndRecursive);
    Builder.SetInsertPoint(EndRecursive);

    Condition = Builder.CreateICmpULT(NextIndex, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    PHIIndex->addIncoming(Index, Start);
    PHIIndex->addIncoming(NextIndex, EndRecursive);
    PHIInt8Ptr->addIncoming(Int8Ptr, Start);
    PHIInt8Ptr->addIncoming(NextInt8Ptr, EndRecursive);

    Builder.SetInsertPoint(LoopEnd);
    PHIInt8Ptr = Builder.CreatePHI(CGM.Int8PtrTy, 2);
    PHIInt8Ptr->addIncoming(Int8Ptr, Start);
    PHIInt8Ptr->addIncoming(NextInt8Ptr, EndRecursive);
    return PHIInt8Ptr;
  }
  case Type::Category::Struct:
  case Type::Category::Tuple:
  case Type::Category::Contract:
  case Type::Category::Function:
  case Type::Category::Mapping:
    assert(false &&
           "This type is not available currently for abi.encodePacked");
    __builtin_unreachable();
  case Type::Category::FixedBytes: {
    llvm::Value *Result = Value->load(Builder, CGM);
    Int8Ptr = copyToInt8Ptr(Int8Ptr, Result, true);
    if (IsArrayElement) {
      const auto *FixedBytesTy = dynamic_cast<const FixedBytesType *>(Ty);
      unsigned PadRightLength = 32 - FixedBytesTy->getBitNum() / 8;
      if (PadRightLength % 32)
        Int8Ptr = Builder.CreateInBoundsGEP(
            CGM.Int8Ty, Int8Ptr, {Builder.getIntN(32, PadRightLength)});
    }
    return Int8Ptr;
  }
  case Type::Category::Bool: {
    llvm::Value *Result = Value->load(Builder, CGM);
    if (IsArrayElement) {
      Result = Builder.CreateZExtOrTrunc(Result, CGF.Int256Ty);
    }
    return copyToInt8Ptr(Int8Ptr, Builder.CreateZExtOrTrunc(Result, CGF.Int8Ty),
                         true);
  }
  default: {
    llvm::Value *Result = Value->load(Builder, CGM);
    if (IsArrayElement) {
      Result = Builder.CreateZExtOrTrunc(Result, CGF.Int256Ty);
    }
    return copyToInt8Ptr(Int8Ptr, Result, true);
  }
  }
}
llvm::Value *AbiEmitter::emitEncode(llvm::Value *Int8Ptr,
                                    const ExprValuePtr &Value,
                                    bool IsStateVariable) {
  const Type *Ty = Value->getType();
  switch (Ty->getCategory()) {
  case Type::Category::String:
  case Type::Category::Bytes: {
    llvm::Value *Bytes = Value->load(Builder, CGM);
    llvm::Value *Length = Builder.CreateZExtOrTrunc(
        Builder.CreateExtractValue(Bytes, {0}), CGF.Int32Ty);
    llvm::Value *PadRightLength = Builder.CreateURem(
        Builder.CreateSub(Builder.getIntN(32, 32),
                          Builder.CreateURem(Length, Builder.getIntN(32, 32))),
        Builder.getIntN(32, 32));
    Int8Ptr = copyToInt8Ptr(
        Int8Ptr, Builder.CreateZExtOrTrunc(Length, CGF.Int256Ty), true);
    Int8Ptr = copyToInt8Ptr(Int8Ptr, Bytes, true);
    return Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr, {PadRightLength});
  }
  case Type::Category::Array: {
    llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *Start = Builder.GetInsertBlock();
    llvm::BasicBlock *Loop =
        llvm::BasicBlock::Create(VMContext, "loop", ThisFunc);
    llvm::BasicBlock *LoopEnd =
        llvm::BasicBlock::Create(VMContext, "loop_end", ThisFunc);
    llvm::BasicBlock *EndRecursive =
        llvm::BasicBlock::Create(VMContext, "recursive_end", ThisFunc);

    const auto *ArrTy = dynamic_cast<const ArrayType *>(Ty);
    llvm::Value *ArrayLengthInt256 =
        Builder.CreateZExtOrTrunc(getArrayLength(Value, ArrTy), CGM.Int256Ty);
    llvm::Value *ArrayLength =
        Builder.CreateZExtOrTrunc(ArrayLengthInt256, CGM.Int32Ty);
    if (ArrTy->isDynamicSized()) {
      Int8Ptr = copyToInt8Ptr(Int8Ptr, ArrayLengthInt256, true);
    }
    llvm::Value *Int8PtrBegin = Int8Ptr;
    llvm::Value *Head = nullptr;
    llvm::Value *Tail = nullptr;
    unsigned ABIStaticSize = ArrTy->getABIStaticSize();
    llvm::Value *ABIStaticSizeValue = Builder.getIntN(32, ABIStaticSize);
    if (ArrTy->getElementType()->isDynamic()) {
      Head = Int8Ptr;
      Tail = Builder.CreateInBoundsGEP(
          CGM.Int8Ty, Head,
          {Builder.CreateMul(ArrayLength, ABIStaticSizeValue)});
    } else {
      Tail = Int8Ptr;
    }
    llvm::Value *Index = Builder.getIntN(32, 0);

    llvm::Value *Condition = Builder.CreateICmpULT(Index, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    Builder.SetInsertPoint(Loop);
    llvm::PHINode *PHIIndex = Builder.CreatePHI(CGM.Int32Ty, 2);
    llvm::PHINode *PHIHead = nullptr;
    if (ArrTy->getElementType()->isDynamic())
      PHIHead = Builder.CreatePHI(CGM.Int8PtrTy, 2);
    llvm::PHINode *PHITail = Builder.CreatePHI(CGM.Int8PtrTy, 2);

    ExprValuePtr IndexAccessValue = ExprEmitter(CGF).arrayIndexAccess(
        Value, PHIIndex, ArrTy->getElementType().get(), ArrTy, IsStateVariable);

    llvm::Value *NextHead = nullptr;
    if (ArrTy->getElementType()->isDynamic()) {
      llvm::Value *TailPos = Builder.CreateZExtOrTrunc(
          Builder.CreatePtrDiff(PHITail, Int8PtrBegin), CGM.Int256Ty);
      NextHead = copyToInt8Ptr(PHIHead, TailPos, true);
    }
    llvm::Value *NextTail =
        emitEncode(PHITail, IndexAccessValue, IsStateVariable);
    llvm::Value *NextIndex =
        Builder.CreateAdd(PHIIndex, Builder.getIntN(32, 1));
    Builder.CreateBr(EndRecursive);
    Builder.SetInsertPoint(EndRecursive);

    Condition = Builder.CreateICmpULT(NextIndex, ArrayLength);
    Builder.CreateCondBr(Condition, Loop, LoopEnd);

    PHIIndex->addIncoming(Index, Start);
    PHIIndex->addIncoming(NextIndex, EndRecursive);
    if (ArrTy->getElementType()->isDynamic()) {
      PHIHead->addIncoming(Head, Start);
      PHIHead->addIncoming(NextHead, EndRecursive);
    }
    PHITail->addIncoming(Tail, Start);
    PHITail->addIncoming(NextTail, EndRecursive);

    Builder.SetInsertPoint(LoopEnd);
    PHITail = Builder.CreatePHI(CGM.Int8PtrTy, 2);
    PHITail->addIncoming(Tail, Start);
    PHITail->addIncoming(NextTail, EndRecursive);
    return PHITail;
  }
  case Type::Category::Struct: {
    const auto *STy = dynamic_cast<const StructType *>(Ty);
    std::vector<std::pair<ExprValuePtr, bool>> Values;
    bool IsStateVariable = Value->getValueKind() == ValueKind::VK_SValue;
    unsigned ElementSize = STy->getElementSize();
    for (unsigned I = 0; I < ElementSize; ++I) {
      Values.emplace_back(ExprEmitter(CGF).structIndexAccess(Value, I, STy),
                          IsStateVariable);
    }
    return emitEncodeTuple(Int8Ptr, Values);
  }
  case Type::Category::Tuple:
    assert(false && "This type is not supported currently for abi.encode");
    __builtin_unreachable();
  case Type::Category::Contract:
  case Type::Category::Function:
  case Type::Category::Mapping:
    assert(false && "This type is not available currently for abi.encode");
    __builtin_unreachable();
  case Type::Category::FixedBytes: {
    llvm::Value *Result = Value->load(Builder, CGM);
    Int8Ptr = copyToInt8Ptr(Int8Ptr, Result, true);
    const auto *FixedBytesTy = dynamic_cast<const FixedBytesType *>(Ty);
    unsigned PadRightLength = 32 - FixedBytesTy->getBitNum() / 8;
    if (PadRightLength % 32)
      Int8Ptr = Builder.CreateInBoundsGEP(
          CGM.Int8Ty, Int8Ptr, {Builder.getIntN(32, PadRightLength)});
    return Int8Ptr;
  }
  default: {
    llvm::Value *Result = Value->load(Builder, CGM);
    Result = Builder.CreateZExtOrTrunc(Result, CGF.Int256Ty);
    return copyToInt8Ptr(Int8Ptr, Result, true);
  }
  }
}
llvm::Value *AbiEmitter::copyToInt8Ptr(llvm::Value *Int8Ptr, llvm::Value *Value,
                                       bool IncreasePtr) {
  llvm::Function *Memcpy = CGM.getModule().getFunction("solidity.memcpy");
  llvm::Type *Ty = Value->getType();
  if (CGM.isDynamicType(Ty)) {
    llvm::Value *Length = Builder.CreateZExtOrTrunc(
        Builder.CreateExtractValue(Value, {0}), CGM.Int32Ty);
    llvm::Value *SrcBytes = Builder.CreateExtractValue(Value, {1});
    Builder.CreateCall(Memcpy, {Builder.CreateBitCast(Int8Ptr, CGM.Int8PtrTy),
                                SrcBytes, Length});
    if (IncreasePtr)
      return Builder.CreateInBoundsGEP(CGM.Int8Ty, Int8Ptr, {Length});
  } else {
    Value = CGM.getEndianlessValue(Value);
    llvm::Value *CPtr =
        Builder.CreatePointerCast(Int8Ptr, llvm::PointerType::getUnqual(Ty));
    Builder.CreateStore(Value, CPtr);
    if (IncreasePtr)
      return Builder.CreateInBoundsGEP(
          CGM.Int8Ty, Int8Ptr,
          {Builder.getInt32(Ty->getIntegerBitWidth() / 8)});
  }
  return nullptr;
}

} // namespace soll::CodeGen