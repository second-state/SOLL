// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"
#include "soll/AST/Expr.h"
#include <llvm/IR/Value.h>

namespace soll::CodeGen {

class ExprValue {
  const Type *Ty;
  ValueKind Kind;
  llvm::Value *V;
  llvm::Value *Shift;

public:
  ExprValue() {}
  ExprValue(const Type *Ty, ValueKind Kind, llvm::Value *V,
            llvm::Value *Shift = nullptr)
      : Ty(Ty), Kind(Kind), V(V), Shift(Shift) {}
  static ExprValue getRValue(const Expr *E, llvm::Value *V) {
    return ExprValue(E->getType().get(), ValueKind::VK_RValue, V);
  }
  const Type *getType() const { return Ty; }
  llvm::Value *getValue() const { return V; }
  template <typename T>
  llvm::Value *load(T &Builder, CodeGenModule &CGM,
                    llvm::StringRef Name = "") const {
    switch (Kind) {
    case ValueKind::VK_RValue:
      return V;
    case ValueKind::VK_LValue:
      return Builder.CreateLoad(V, Name);
    case ValueKind::VK_SValue:
      llvm::Function *StorageLoad =
          CGM.getModule().getFunction("ethereum.storageLoad");
      llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
      Builder.CreateCall(StorageLoad, {V, ValPtr});
      llvm::Value *Val = CGM.emitEndianConvert(Builder.CreateLoad(ValPtr));
      switch (Ty->getCategory()) {
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::FixedBytes:
      case Type::Category::Integer:
      case Type::Category::RationalNumber: {
        if (Shift != nullptr) {
          Val = Builder.CreateLShr(Val, Shift);
        }
        return Builder.CreateTruncOrBitCast(Val, CGM.getLLVMType(Ty), Name);
      }
      case Type::Category::String:
      case Type::Category::Bytes: {
        llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
        llvm::Function *Keccak256 =
            CGM.getModule().getFunction("solidity.keccak256");
        llvm::BasicBlock *LoopInit = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "loop_init", ThisFunc);
        llvm::BasicBlock *Loop =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "loop", ThisFunc);
        llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "loop_end", ThisFunc);
        llvm::ConstantInt *WordWidth = Builder.getIntN(256, 32);

        Builder.CreateBr(LoopInit);

        Builder.SetInsertPoint(LoopInit);
        llvm::Value *PaddedLength =
            Builder.CreateAnd(Builder.CreateAdd(Val, Builder.getIntN(256, 31)),
                              llvm::APInt::getHighBitsSet(256, 256 - 5));
        llvm::Value *Bytes = emitConcateBytes(Builder, CGM, {V});
        llvm::Value *Address = Builder.CreateCall(Keccak256, {Bytes});
        llvm::Value *Ptr = Builder.CreateAlloca(CGM.Int8PtrTy, PaddedLength);
        llvm::Value *AddressPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
        Builder.CreateStore(Address, AddressPtr);
        Builder.CreateBr(Loop);

        Builder.SetInsertPoint(Loop);
        llvm::PHINode *PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIPtr = Builder.CreatePHI(CGM.Int8PtrTy, 2);
        Builder.CreateCall(StorageLoad, {AddressPtr, PHIPtr});
        llvm::Value *CurrentAddress = Builder.CreateLoad(AddressPtr);
        llvm::Value *NextRemain =
            Builder.CreateSub(PHIRemain, Builder.getIntN(256, 32));
        llvm::Value *NextAddress =
            Builder.CreateAdd(CurrentAddress, Builder.getIntN(256, 32));
        Builder.CreateStore(NextAddress, AddressPtr);
        llvm::Value *NextPtr = Builder.CreateInBoundsGEP(
            Ptr, {Builder.getInt32(0), Builder.getInt32(32)});
        llvm::Value *Condition =
            Builder.CreateICmpNE(NextRemain, Builder.getIntN(256, 0));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);

        Builder.SetInsertPoint(LoopEnd);
        llvm::Value *Value = llvm::UndefValue::get(CGM.getLLVMType(Ty));
        Bytes = Builder.CreateInsertValue(Bytes, Val, {0});
        Bytes = Builder.CreateInsertValue(Bytes, Ptr, {1});

        PHIRemain->addIncoming(PaddedLength, LoopInit);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(Ptr, LoopInit);
        PHIPtr->addIncoming(NextPtr, Loop);

        return Bytes;
      }
      default:
        assert(false);
      }
    }
    assert(false && "unknown kind!");
    __builtin_unreachable();
  }
  template <typename T>
  void store(T &Builder, CodeGenModule &CGM, llvm::Value *Value) const {
    switch (Kind) {
    case ValueKind::VK_RValue:
      assert(false && "store on RValue");
      __builtin_unreachable();
    case ValueKind::VK_LValue:
      Builder.CreateStore(Value, V);
      return;
    case ValueKind::VK_SValue:
      llvm::Function *StorageStore =
          CGM.getModule().getFunction("ethereum.storageStore");
      switch (Ty->getCategory()) {
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::FixedBytes:
      case Type::Category::Integer:
      case Type::Category::RationalNumber: {
        llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
        if (Shift != nullptr) {
          llvm::Function *StorageLoad =
              CGM.getModule().getFunction("ethereum.storageLoad");
          llvm::ConstantInt *Mask = Builder.getInt(
              llvm::APInt::getHighBitsSet(256, 256 - Ty->getBitNum()));
          llvm::Value *Mask1 = Builder.CreateShl(Mask, Shift);
          llvm::Value *Mask2 =
              Builder.CreateShl(Builder.CreateZExt(Value, CGM.Int256Ty), Shift);

          Builder.CreateCall(StorageLoad, {V, ValPtr});
          llvm::Value *Val = CGM.emitEndianConvert(Builder.CreateLoad(ValPtr));
          Value = Builder.CreateOr(Builder.CreateAnd(Val, Mask1), Mask2);
        }
        Builder.CreateStore(CGM.emitEndianConvert(Value), ValPtr);
        Builder.CreateCall(StorageStore, {V, ValPtr});
        return;
      }
      case Type::Category::String:
      case Type::Category::Bytes: {
        llvm::Type *Array32Int8Ptr =
            llvm::PointerType::getUnqual(llvm::ArrayType::get(CGM.Int8Ty, 32));
        llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
        llvm::Function *Keccak256 =
            CGM.getModule().getFunction("solidity.keccak256");
        llvm::BasicBlock *LoopInit = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "loop_init", ThisFunc);
        llvm::BasicBlock *Loop =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "loop", ThisFunc);
        llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "loop_end", ThisFunc);
        llvm::ConstantInt *WordWidth = Builder.getIntN(256, 32);

        llvm::Value *Length = Builder.CreateExtractValue(Value, {0});
        llvm::Value *Ptr = Builder.CreateExtractValue(Value, {1});
        Ptr = Builder.CreateBitCast(Ptr, Array32Int8Ptr);

        llvm::Value *LengthPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
        Builder.CreateStore(CGM.emitEndianConvert(Length), LengthPtr);
        Builder.CreateCall(StorageStore, {V, LengthPtr});
        Builder.CreateBr(LoopInit);

        Builder.SetInsertPoint(LoopInit);
        llvm::Value *PaddedLength = Builder.CreateAnd(
            Builder.CreateAdd(Length, Builder.getIntN(256, 31)),
            llvm::APInt::getHighBitsSet(256, 256 - 5));
        llvm::Value *Bytes =
            emitConcateBytes(Builder, CGM, {Builder.CreateLoad(V)});
        llvm::Value *Address = Builder.CreateCall(Keccak256, {Bytes});
        llvm::Value *AddressPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
        Builder.CreateStore(Address, AddressPtr);
        Builder.CreateBr(Loop);

        Builder.SetInsertPoint(Loop);
        llvm::PHINode *PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIPtr = Builder.CreatePHI(Array32Int8Ptr, 2);
        Builder.CreateCall(
            StorageStore,
            {AddressPtr, Builder.CreateBitCast(PHIPtr, CGM.Int256PtrTy)});
        llvm::Value *CurrentAddress = Builder.CreateLoad(AddressPtr);
        llvm::Value *NextRemain =
            Builder.CreateSub(PHIRemain, Builder.getIntN(256, 32));
        llvm::Value *NextAddress =
            Builder.CreateAdd(CurrentAddress, Builder.getIntN(256, 32));
        Builder.CreateStore(NextAddress, AddressPtr);
        llvm::Value *NextPtr = Builder.CreateInBoundsGEP(
            PHIPtr, {Builder.getInt32(0), Builder.getInt32(32)});
        NextPtr = Builder.CreateBitCast(NextPtr, Array32Int8Ptr);
        llvm::Value *Condition =
            Builder.CreateICmpNE(NextRemain, Builder.getIntN(256, 0));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);

        Builder.SetInsertPoint(LoopEnd);
        PHIRemain->addIncoming(PaddedLength, LoopInit);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(Ptr, LoopInit);
        PHIPtr->addIncoming(NextPtr, Loop);
        return;
      }
      default:
        assert(false);
      }
    }
    assert(false && "unknown kind!");
    __builtin_unreachable();
  }

private:
  template <typename T>
  llvm::Value *emitConcateBytes(T &Builder, CodeGenModule &CGM,
                                llvm::ArrayRef<llvm::Value *> Values) const {
    unsigned ArrayLength = 0;
    for (llvm::Value *Value : Values) {
      llvm::Type *Ty = Value->getType();
      ArrayLength += Ty->getIntegerBitWidth() / 8;
    }

    llvm::ArrayType *ArrayTy = llvm::ArrayType::get(CGM.Int8Ty, ArrayLength);
    llvm::Value *Array = Builder.CreateAlloca(ArrayTy, nullptr, "concat");

    unsigned Index = 0;
    for (llvm::Value *Value : Values) {
      llvm::Type *Ty = Value->getType();
      llvm::Value *Ptr = Builder.CreateInBoundsGEP(
          Array, {Builder.getInt32(0), Builder.getInt32(Index)});
      llvm::Value *CPtr =
          Builder.CreatePointerCast(Ptr, llvm::PointerType::getUnqual(Ty));
      Builder.CreateStore(Value, CPtr);
      Index += Ty->getIntegerBitWidth() / 8;
    }

    llvm::Value *Bytes = llvm::ConstantAggregateZero::get(CGM.BytesTy);
    Bytes = Builder.CreateInsertValue(Bytes, Builder.getIntN(256, ArrayLength),
                                      {0});
    Bytes = Builder.CreateInsertValue(
        Bytes,
        Builder.CreateInBoundsGEP(Array,
                                  {Builder.getInt32(0), Builder.getInt32(0)}),
        {1});
    return Bytes;
  }
};

} // namespace soll::CodeGen
