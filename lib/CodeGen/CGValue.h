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
  llvm::Value *load(T &Builder, CodeGenModule &CGM, llvm::StringRef Name = "") const {
    switch (Kind) {
    case ValueKind::VK_RValue:
      return V;
    case ValueKind::VK_LValue:
      return Builder.CreateLoad(V, Name);
    case ValueKind::VK_SValue:
      llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
      Builder.CreateCall(Builder.GetInsertBlock()->getModule()->getFunction(
                             "ethereum.storageLoad"),
                         {V, ValPtr});
      llvm::Value *Val = CGM.emitEndianConvert(Builder.CreateLoad(ValPtr));
      if (Shift != nullptr) {
        Val = Builder.CreateLShr(Val, Shift);
      }
      return Builder.CreateTruncOrBitCast(Val, CGM.getLLVMType(Ty), Name);
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
      llvm::Value *ValPtr = Builder.CreateAlloca(CGM.Int256Ty, nullptr);
      if (Shift != nullptr) {
        llvm::ConstantInt *Mask = Builder.getInt(
            llvm::APInt::getHighBitsSet(256, 256 - Ty->getBitNum()));
        llvm::Value *Mask1 = Builder.CreateShl(Mask, Shift);
        llvm::Value *Mask2 =
            Builder.CreateShl(Builder.CreateZExt(Value, CGM.Int256Ty), Shift);

        Builder.CreateCall(Builder.GetInsertBlock()->getModule()->getFunction(
                               "ethereum.storageLoad"),
                           {V, ValPtr});
        llvm::Value *Val = CGM.emitEndianConvert(Builder.CreateLoad(ValPtr));
        Value = Builder.CreateOr(Builder.CreateAnd(Val, Mask1), Mask2);
      }
      Builder.CreateStore(CGM.emitEndianConvert(Value), ValPtr);
      Builder.CreateCall(Builder.GetInsertBlock()->getModule()->getFunction(
                             "ethereum.storageStore"),
                         {V, ValPtr});
      return;
    }
    assert(false && "unknown kind!");
    __builtin_unreachable();
  }
};

} // namespace soll::CodeGen
