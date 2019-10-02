// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace soll::CodeGen {

struct CodeGenTypeCache {
  /// void
  llvm::Type *VoidTy;

  /// i8, i16, i32, i64, i128, i256
  llvm::IntegerType *Int8Ty, *Int16Ty, *Int32Ty, *Int64Ty, *Int128Ty, *Int256Ty;
  /// i1
  llvm::IntegerType *BoolTy;
  /// i160
  llvm::IntegerType *AddressTy;

  /// i8*, i32*, i64*, i128*, i256*
  llvm::PointerType *Int8PtrTy, *Int32PtrTy, *Int64PtrTy, *Int128PtrTy,
      *Int256PtrTy;
  /// i160*
  llvm::PointerType *AddressPtrTy;

  /// bytes
  llvm::StructType *BytesTy;
  /// String
  llvm::StructType *StringTy;
};

} // namespace soll::CodeGen
