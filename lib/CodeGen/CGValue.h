// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"
#include "soll/AST/Expr.h"
#include "soll/AST/ExprAsm.h"
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
  ValueKind getValueKind() const { return Kind; }
  void setValue(llvm::Value *Value) { this->V = Value; }
  void setShift(llvm::Value *Shift) { this->Shift = Shift; }
  template <typename T>
  llvm::Value *load(T &Builder, CodeGenModule &CGM,
                    llvm::StringRef Name = "") const {
    switch (Kind) {
    case ValueKind::VK_Unknown:
      assert(false && "Kind == Unknown");
      __builtin_unreachable();
    case ValueKind::VK_RValue:
      return V;
    case ValueKind::VK_LValue:
      return Builder.CreateLoad(V, Name);
    case ValueKind::VK_SValue:
      llvm::Value *Address = CGM.getEndianlessValue(Builder.CreateLoad(V));
      llvm::Value *Val = CGM.emitStorageLoad(Address);
      switch (Ty->getCategory()) {
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::FixedBytes:
      case Type::Category::Integer:
      case Type::Category::RationalNumber: {
        llvm::Type *ValueTy = Builder.getIntNTy(Ty->getBitNum());
        Val = Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Val), ValueTy);
        if (Shift != nullptr) {
          Val = Builder.CreateLShr(Val, Shift);
        }
        return Builder.CreateTruncOrBitCast(Val, CGM.getLLVMType(Ty), Name);
      }
      case Type::Category::String:
      case Type::Category::Bytes: {
        llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
        llvm::Function *StorageLoad = nullptr;
        if (CGM.isEVM()) {
          StorageLoad = CGM.getModule().getFunction("llvm.evm.sload");
        } else if (CGM.isEWASM()) {
          StorageLoad = CGM.getModule().getFunction("ethereum.storageLoad");
        }
        llvm::BasicBlock *InlineSlot =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "inline", ThisFunc);
        llvm::BasicBlock *ExtendSlot =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "extend", ThisFunc);
        llvm::BasicBlock *Loop =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "loop", ThisFunc);
        llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "extend.loop_end", ThisFunc);
        llvm::BasicBlock *Last = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "extend.last_word", ThisFunc);
        llvm::BasicBlock *Done =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "done", ThisFunc);

        llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
        Builder.CreateStore(Val, ValPtr);
        llvm::Value *LastBit = Builder.CreateAnd(
            Val, Builder.CreateLShr(
                     Builder.getInt(llvm::APInt::getHighBitsSet(256, 1)), 7));
        llvm::Value *Condition =
            Builder.CreateICmpEQ(LastBit, Builder.getIntN(256, 0));
        Builder.CreateCondBr(Condition, InlineSlot, ExtendSlot);

        // InlineSlot case
        // +----------+----------+-----------------+
        // |          | 248 bits |      8 bits     |
        // +----------+----------+---------+-------+
        // |          |          |  7 bits | 1 bit |
        // +----------+----------+---------+-------+
        // | position |   data   |  length |   0   |
        // +----------+----------+---------+-------+

        Builder.SetInsertPoint(InlineSlot);
        llvm::Value *InlineLength =
            Builder.CreateLShr(Builder.CreateAnd(CGM.getEndianlessValue(Val),
                                                 Builder.getIntN(256, 0xFF)),
                               1);
        llvm::Value *InlinePtr =
            Builder.CreateAlloca(CGM.Int8Ty, Builder.getInt16(32));
        CGM.emitMemcpy(Builder.CreateBitCast(InlinePtr, CGM.BytesElemPtrTy),
                       Builder.CreateBitCast(ValPtr, CGM.BytesElemPtrTy),
                       Builder.CreateZExtOrTrunc(InlineLength, CGM.Int32Ty));
        Builder.CreateBr(Done);

        // ExtendSlot case
        // +-----------------------+----------+-------+
        // |                       | 255 bits | 1 bit |
        // +-----------------------+----------+-------+
        // |        position       |  length  |   1   |
        // +-----------------------+----------+-------+
        // |                       |    data length   |
        // +-----------------------+------------------+
        // |  keccak256(position)  |       data       |
        // +-----------------------+------------------+
        // | keccak256(position)+1 |         :        |
        // +-----------------------+------------------+

        Builder.SetInsertPoint(ExtendSlot);
        llvm::Value *ExtendLength =
            Builder.CreateLShr(CGM.getEndianlessValue(Val), 1);
        llvm::Value *Bytes = CGM.emitConcatBytes({Address});
        llvm::Value *Address = CGM.emitKeccak256(Bytes);
        llvm::Value *AddressPtr = Builder.CreateAlloca(CGM.Int256Ty);
        llvm::Value *ExtendPtr = Builder.CreateAlloca(CGM.Int8Ty, ExtendLength);
        Condition =
            Builder.CreateICmpSGE(ExtendLength, Builder.getIntN(256, 32));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);

        Builder.SetInsertPoint(Loop);
        llvm::PHINode *PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIPtr = Builder.CreatePHI(CGM.Int8PtrTy, 2);
        llvm::PHINode *PHIAddress = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIAddressPtr = Builder.CreatePHI(CGM.Int256PtrTy, 2);
        Builder.CreateStore(CGM.getEndianlessValue(PHIAddress), PHIAddressPtr);
        if (CGM.isEVM()) {
          llvm::Value *LoadV = Builder.CreateCall(
              StorageLoad,
              {CGM.getEndianlessValue(Builder.CreateLoad(PHIAddressPtr))});
          Builder.CreateStore(CGM.getEndianlessValue(LoadV),
                              Builder.CreateBitCast(PHIPtr, CGM.Int256PtrTy));
        } else if (CGM.isEWASM()) {
          Builder.CreateCall(
              StorageLoad,
              {PHIAddressPtr, Builder.CreateBitCast(PHIPtr, CGM.Int256PtrTy)});
        }
        llvm::Value *NextRemain =
            Builder.CreateSub(PHIRemain, Builder.getIntN(256, 32));
        llvm::Value *NextAddress =
            Builder.CreateAdd(PHIAddress, Builder.getIntN(256, 1));
        llvm::Value *NextendPtr =
            Builder.CreateInBoundsGEP(PHIPtr, {Builder.getInt32(32)});
        Condition = Builder.CreateICmpSGE(NextRemain, Builder.getIntN(256, 32));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);
        PHIRemain->addIncoming(ExtendLength, ExtendSlot);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(ExtendPtr, ExtendSlot);
        PHIPtr->addIncoming(NextendPtr, Loop);
        PHIAddress->addIncoming(Address, ExtendSlot);
        PHIAddress->addIncoming(NextAddress, Loop);
        PHIAddressPtr->addIncoming(AddressPtr, ExtendSlot);
        PHIAddressPtr->addIncoming(PHIAddressPtr, Loop);

        Builder.SetInsertPoint(LoopEnd);
        PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        PHIPtr = Builder.CreatePHI(CGM.Int8PtrTy, 2);
        PHIAddress = Builder.CreatePHI(CGM.Int256Ty, 2);
        PHIRemain->addIncoming(ExtendLength, ExtendSlot);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(ExtendPtr, ExtendSlot);
        PHIPtr->addIncoming(NextendPtr, Loop);
        PHIAddress->addIncoming(Address, ExtendSlot);
        PHIAddress->addIncoming(NextAddress, Loop);
        Condition = Builder.CreateICmpSGT(PHIRemain, Builder.getIntN(256, 0));
        Builder.CreateCondBr(Condition, Last, Done);

        Builder.SetInsertPoint(Last);
        AddressPtr = Builder.CreateAlloca(CGM.Int256Ty);
        Builder.CreateStore(CGM.getEndianlessValue(PHIAddress), AddressPtr);
        ValPtr = Builder.CreateAlloca(CGM.Int256Ty);
        if (CGM.isEVM()) {
          llvm::Value *LoadV = Builder.CreateCall(
              StorageLoad,
              {CGM.getEndianlessValue(Builder.CreateLoad(AddressPtr))});
          Builder.CreateStore(CGM.getEndianlessValue(LoadV), ValPtr);
        } else if (CGM.isEWASM()) {
          Builder.CreateCall(StorageLoad, {AddressPtr, ValPtr});
        }
        CGM.emitMemcpy(Builder.CreateBitCast(PHIPtr, CGM.BytesElemPtrTy),
                       Builder.CreateBitCast(ValPtr, CGM.BytesElemPtrTy),
                       Builder.CreateZExtOrTrunc(PHIRemain, CGM.Int32Ty));
        Builder.CreateBr(Done);

        Builder.SetInsertPoint(Done);
        llvm::PHINode *PHILength = Builder.CreatePHI(CGM.Int256Ty, 3);
        PHIPtr = Builder.CreatePHI(CGM.Int8PtrTy, 3);
        PHILength->addIncoming(InlineLength, InlineSlot);
        PHILength->addIncoming(ExtendLength, LoopEnd);
        PHILength->addIncoming(ExtendLength, Last);
        PHIPtr->addIncoming(InlinePtr, InlineSlot);
        PHIPtr->addIncoming(ExtendPtr, LoopEnd);
        PHIPtr->addIncoming(ExtendPtr, Last);
        Bytes = llvm::ConstantAggregateZero::get(CGM.getLLVMType(Ty));
        Bytes = Builder.CreateInsertValue(Bytes, PHILength, {0});
        Bytes = Builder.CreateInsertValue(Bytes, PHIPtr, {1});

        return Bytes;
      }
      case Type::Category::Array:
        return V; // for abi encoding array
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
    case ValueKind::VK_Unknown:
      assert(false && "Kind == Unknown");
      __builtin_unreachable();
    case ValueKind::VK_RValue:
      assert(false && "store on RValue");
      __builtin_unreachable();
    case ValueKind::VK_LValue:
      Builder.CreateStore(Value, V);
      return;
    case ValueKind::VK_SValue:
      llvm::Value *Address = CGM.getEndianlessValue(Builder.CreateLoad(V));
      switch (Ty->getCategory()) {
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::FixedBytes:
      case Type::Category::Integer:
      case Type::Category::RationalNumber: {
        if (Shift != nullptr) {
          llvm::ConstantInt *Mask = Builder.getInt(
              llvm::APInt::getHighBitsSet(256, 256 - Ty->getBitNum()));
          llvm::Value *Mask1 = Builder.CreateShl(Mask, Shift);
          llvm::Value *Mask2 =
              Builder.CreateShl(Builder.CreateZExt(Value, CGM.Int256Ty), Shift);

          llvm::Value *Val =
              CGM.getEndianlessValue(CGM.emitStorageLoad(Address));
          Value = Builder.CreateOr(Builder.CreateAnd(Val, Mask1), Mask2);
        }
        CGM.emitStorageStore(
            Address, CGM.getEndianlessValue(
                         Builder.CreateZExtOrTrunc(Value, CGM.Int256Ty)));
        return;
      }
      case Type::Category::String:
      case Type::Category::Bytes: {
        llvm::Type *Array32Int8Ptr =
            llvm::PointerType::getUnqual(llvm::ArrayType::get(CGM.Int8Ty, 32));
        llvm::Function *ThisFunc = Builder.GetInsertBlock()->getParent();
        llvm::BasicBlock *InlineSlot =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "inline", ThisFunc);
        llvm::BasicBlock *ExtendSlot =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "extend", ThisFunc);
        llvm::BasicBlock *Loop = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "extend.loop", ThisFunc);
        llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "extend.loop_end", ThisFunc);
        llvm::BasicBlock *Last = llvm::BasicBlock::Create(
            CGM.getLLVMContext(), "extend.last_word", ThisFunc);
        llvm::BasicBlock *Done =
            llvm::BasicBlock::Create(CGM.getLLVMContext(), "done", ThisFunc);

        llvm::Value *Length = Builder.CreateExtractValue(Value, {0});
        llvm::Value *LengthEncode = Builder.CreateShl(Length, 1);
        llvm::Value *Ptr = Builder.CreateExtractValue(Value, {1});
        Ptr = Builder.CreateBitCast(Ptr, Array32Int8Ptr);
        llvm::Value *AddressPtr = Builder.CreateAlloca(CGM.Int256Ty);
        Builder.CreateStore(Address, AddressPtr);
        llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty);

        llvm::Value *Condition =
            Builder.CreateICmpSGE(Length, Builder.getIntN(256, 32));
        Builder.CreateCondBr(Condition, ExtendSlot, InlineSlot);

        // InlineSlot case
        // +----------+----------+-----------------+
        // |          | 248 bits |      8 bits     |
        // +----------+----------+---------+-------+
        // |          |          |  7 bits | 1 bit |
        // +----------+----------+---------+-------+
        // | position |   data   |  length |   0   |
        // +----------+----------+---------+-------+

        Builder.SetInsertPoint(InlineSlot);
        CGM.emitMemcpy(Builder.CreateBitCast(ValPtr, CGM.BytesElemPtrTy),
                       Builder.CreateBitCast(Ptr, CGM.BytesElemPtrTy),
                       Builder.CreateZExtOrTrunc(Length, CGM.Int32Ty));
        llvm::Value *Val = Builder.CreateLoad(ValPtr);
        Val = Builder.CreateOr(Val, CGM.getEndianlessValue(LengthEncode));
        Builder.CreateStore(Val, ValPtr);
        CGM.emitStorageStore(Builder.CreateLoad(AddressPtr),
                             Builder.CreateLoad(ValPtr));
        Builder.CreateBr(Done);

        // ExtendSlot case
        // +-----------------------+----------+-------+
        // |                       | 255 bits | 1 bit |
        // +-----------------------+----------+-------+
        // |        position       |  length  |   1   |
        // +-----------------------+----------+-------+
        // |                       |    data length   |
        // +-----------------------+------------------+
        // |  keccak256(position)  |       data       |
        // +-----------------------+------------------+
        // | keccak256(position)+1 |         :        |
        // +-----------------------+------------------+

        Builder.SetInsertPoint(ExtendSlot);
        LengthEncode = CGM.getEndianlessValue(
            Builder.CreateOr(LengthEncode, Builder.getIntN(256, 1)));
        Builder.CreateStore(LengthEncode, ValPtr);
        CGM.emitStorageStore(Builder.CreateLoad(AddressPtr),
                             Builder.CreateLoad(ValPtr));
        llvm::Value *Bytes = CGM.emitConcatBytes({Address});
        Address = CGM.emitKeccak256(Bytes);
        Condition = Builder.CreateICmpSGE(Length, Builder.getIntN(256, 32));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);

        Builder.SetInsertPoint(Loop);
        llvm::PHINode *PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIPtr = Builder.CreatePHI(Array32Int8Ptr, 2);
        llvm::PHINode *PHIAddress = Builder.CreatePHI(CGM.Int256Ty, 2);
        llvm::PHINode *PHIAddressPtr = Builder.CreatePHI(CGM.Int256PtrTy, 2);
        Builder.CreateStore(CGM.getEndianlessValue(PHIAddress), PHIAddressPtr);
        CGM.emitStorageStore(
            Builder.CreateLoad(PHIAddressPtr),
            Builder.CreateLoad(Builder.CreateBitCast(PHIPtr, CGM.Int256PtrTy)));
        llvm::Value *NextRemain =
            Builder.CreateSub(PHIRemain, Builder.getIntN(256, 32));
        llvm::Value *NextAddress =
            Builder.CreateAdd(PHIAddress, Builder.getIntN(256, 1));
        llvm::Value *NextendPtr = Builder.CreateInBoundsGEP(
            PHIPtr, {Builder.getInt32(0), Builder.getInt32(32)});
        NextendPtr = Builder.CreateBitCast(NextendPtr, Array32Int8Ptr);
        Condition = Builder.CreateICmpSGE(NextRemain, Builder.getIntN(256, 32));
        Builder.CreateCondBr(Condition, Loop, LoopEnd);
        PHIRemain->addIncoming(Length, ExtendSlot);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(Ptr, ExtendSlot);
        PHIPtr->addIncoming(NextendPtr, Loop);
        PHIAddress->addIncoming(Address, ExtendSlot);
        PHIAddress->addIncoming(NextAddress, Loop);
        PHIAddressPtr->addIncoming(AddressPtr, ExtendSlot);
        PHIAddressPtr->addIncoming(PHIAddressPtr, Loop);

        Builder.SetInsertPoint(LoopEnd);
        PHIRemain = Builder.CreatePHI(CGM.Int256Ty, 2);
        PHIPtr = Builder.CreatePHI(Array32Int8Ptr, 2);
        PHIAddress = Builder.CreatePHI(CGM.Int256Ty, 2);
        PHIRemain->addIncoming(Length, ExtendSlot);
        PHIRemain->addIncoming(NextRemain, Loop);
        PHIPtr->addIncoming(Ptr, ExtendSlot);
        PHIPtr->addIncoming(NextendPtr, Loop);
        PHIAddress->addIncoming(Address, ExtendSlot);
        PHIAddress->addIncoming(NextAddress, Loop);
        Condition = Builder.CreateICmpSGT(PHIRemain, Builder.getIntN(256, 0));
        Builder.CreateCondBr(Condition, Last, Done);

        Builder.SetInsertPoint(Last);
        AddressPtr = Builder.CreateAlloca(CGM.Int256Ty);
        Builder.CreateStore(CGM.getEndianlessValue(PHIAddress), AddressPtr);
        ValPtr = Builder.CreateAlloca(CGM.Int256Ty);
        CGM.emitMemcpy(Builder.CreateBitCast(ValPtr, CGM.BytesElemPtrTy),
                       Builder.CreateBitCast(PHIPtr, CGM.BytesElemPtrTy),
                       Builder.CreateZExtOrTrunc(PHIRemain, CGM.Int32Ty));
        CGM.emitStorageStore(Builder.CreateLoad(AddressPtr),
                             Builder.CreateLoad(ValPtr));
        Builder.CreateBr(Done);

        Builder.SetInsertPoint(Done);
        return;
      }
      default:
        assert(false);
      }
    }
    assert(false && "unknown kind!");
    __builtin_unreachable();
  }
};

} // namespace soll::CodeGen
