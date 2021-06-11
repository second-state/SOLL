// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "ExprEmitter.h"

namespace soll::CodeGen {
class AbiEmitter {
public:
  CodeGenFunction &CGF;
  CodeGenModule &CGM;
  llvm::IRBuilder<llvm::ConstantFolder> &Builder;
  llvm::LLVMContext &VMContext;
  AbiEmitter(CodeGenFunction &CGF)
      : CGF(CGF), CGM(CGF.getCodeGenModule()), Builder(CGF.getBuilder()),
        VMContext(CGF.getLLVMContext()) {}
  llvm::Value *getEncodePackedTupleSize(
      const std::vector<std::pair<ExprValuePtr, bool>> &Values);
  llvm::Value *
  getEncodeTupleSize(const std::vector<std::pair<ExprValuePtr, bool>> &Values);
  llvm::Value *emitEncodePackedTuple(
      llvm::Value *Int8Ptr,
      const std::vector<std::pair<ExprValuePtr, bool>> &Values);
  llvm::Value *
  emitEncodeTuple(llvm::Value *Int8Ptr,
                  const std::vector<std::pair<ExprValuePtr, bool>> &Values);

  std::pair<ExprValuePtr, llvm::Value *> getDecode(llvm::Value *Int8Ptr,
                                                   const Type *Ty);

private:
  std::pair<std::vector<llvm::Value *>, llvm::Value *>
  getDecodeTuple(llvm::Value *Int8Ptr, const TupleType *Ty);
  llvm::Value *getArrayLength(const ExprValuePtr &Base, const ArrayType *ArrTy);
  unsigned getElementPerSlot(bool isStateVariable, const ArrayType *ArrTy);
  llvm::Value *getEncodePackedSize(const ExprValuePtr &Value,
                                   bool IsStateVariable);
  llvm::Value *getEncodeSize(const ExprValuePtr &Value, bool IsStateVariable);
  llvm::Value *emitEncodePacked(llvm::Value *Int8Ptr, const ExprValuePtr &Value,
                                bool IsStateVariable,
                                bool IsArrayElement = false);
  llvm::Value *emitEncode(llvm::Value *Int8Ptr, const ExprValuePtr &Value,
                          bool IsStateVariable);
  llvm::Value *copyToInt8Ptr(llvm::Value *Int8Ptr, llvm::Value *Value,
                             bool IncreasePtr);
};
} // namespace soll::CodeGen