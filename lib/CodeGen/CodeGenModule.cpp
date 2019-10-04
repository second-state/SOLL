// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"
#include "CodeGenFunction.h"
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Intrinsics.h>

/*
// for testing purpose
namespace llvm::Intrinsic {
enum {
  evm_calldatacopy,
  evm_calldatasize,
  evm_staticcall,
  evm_sstore,
  evm_sload,
  evm_caller,
  evm_callvalue,
  evm_log3,
  evm_gas,
  evm_return,
  evm_revert,
  evm_returndatacopy,
  evm_gasprice,
  evm_origin,
  evm_coinbase,
  evm_difficulty,
  evm_gaslimit,
  evm_number,
  evm_timestamp
};
}
*/

namespace soll::CodeGen {

CodeGenModule::CodeGenModule(ASTContext &C, llvm::Module &M,
                             DiagnosticsEngine &Diags,
                             const TargetOptions &TargetOpts)
    : Context(C), TheModule(M), Diags(Diags), TargetOpts(TargetOpts),
      VMContext(M.getContext()), Builder(VMContext) {
  initTypes();
  if (isEVM()) {
    initEVMOpcodeDeclaration();
  } else {
    initEEIDeclaration();
  }
  initHelperDeclaration();
  initPrebuiltContract();
}

void CodeGenModule::initTypes() {
  VoidTy = llvm::Type::getVoidTy(VMContext);

  BoolTy = llvm::Type::getInt1Ty(VMContext);
  Int8Ty = llvm::Type::getInt8Ty(VMContext);
  Int32Ty = llvm::Type::getInt32Ty(VMContext);
  Int64Ty = llvm::Type::getInt64Ty(VMContext);
  Int128Ty = llvm::Type::getInt128Ty(VMContext);
  AddressTy = llvm::Type::getIntNTy(VMContext, 160);
  Int256Ty = llvm::Type::getIntNTy(VMContext, 256);

  Int8PtrTy = llvm::Type::getInt8PtrTy(VMContext);
  Int32PtrTy = llvm::Type::getInt32PtrTy(VMContext);
  Int64PtrTy = llvm::Type::getInt64PtrTy(VMContext);
  Int128PtrTy = llvm::Type::getIntNPtrTy(VMContext, 128);
  AddressPtrTy = llvm::Type::getIntNPtrTy(VMContext, 160);
  Int256PtrTy = llvm::Type::getIntNPtrTy(VMContext, 256);

  BytesTy = llvm::StructType::create(VMContext, {Int256Ty, Int8PtrTy}, "bytes");
  StringTy = llvm::StructType::create(VMContext, {Int256Ty, Int8PtrTy},
                                      "string", false);

  EVMIntTy = Int256Ty;
  EVMIntPtrTy = Int256PtrTy;
}

llvm::Function *CodeGenModule::getIntrinsic(unsigned IID,
                                            llvm::ArrayRef<llvm::Type *> Typs) {
  return llvm::Intrinsic::getDeclaration(
      &TheModule, static_cast<llvm::Intrinsic::ID>(IID), Typs);
}

void CodeGenModule::initEVMOpcodeDeclaration() {
#if !defined(ENABLE_EVM_BACKEND)
  llvm::errs() << "Your LLVM backend targets doesn't support EVM!\n";
  exit(1);
#else
  llvm::FunctionType *FT = nullptr;

  // evm_calldatacopy
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_callDataCopy = getIntrinsic(llvm::Intrinsic::evm_calldatacopy, FT);

  // evm_calldatasize
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getCallDataSize = getIntrinsic(llvm::Intrinsic::evm_calldatasize, FT);

  // evm_staticcall
  FT = llvm::FunctionType::get(
      EVMIntTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
      false);
  Func_callStatic = getIntrinsic(llvm::Intrinsic::evm_staticcall, FT);

  // evm_sstore
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_storageStore = getIntrinsic(llvm::Intrinsic::evm_sstore, FT);

  // evm_sload
  FT = llvm::FunctionType::get(EVMIntTy, {EVMIntTy}, false);
  Func_storageLoad = getIntrinsic(llvm::Intrinsic::evm_sload, FT);

  // evm_caller
  FT = llvm::FunctionType::get(AddressTy, {}, false);
  Func_getCaller = getIntrinsic(llvm::Intrinsic::evm_caller, FT);

  // evm_callvalue
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getCallValue = getIntrinsic(llvm::Intrinsic::evm_callvalue, FT);

  // evm_log3
  FT = llvm::FunctionType::get(
      VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_log3 = getIntrinsic(llvm::Intrinsic::evm_log3, FT);

  // evm_gas
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getGasLeft = getIntrinsic(llvm::Intrinsic::evm_gas, FT);

  // finish
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_finish = getIntrinsic(llvm::Intrinsic::evm_return, FT);

  // revert
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_revert = getIntrinsic(llvm::Intrinsic::evm_revert, FT);

  // returnDataCopy
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_returnDataCopy = getIntrinsic(llvm::Intrinsic::evm_returndatacopy, FT);

  // getTxGasPrice
  FT = llvm::FunctionType::get(VoidTy, {Int128PtrTy}, false);
  Func_getTxGasPrice = getIntrinsic(llvm::Intrinsic::evm_gasprice, FT);

  // getTxOrigin
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getTxOrigin = getIntrinsic(llvm::Intrinsic::evm_origin, FT);

  // getBlockCoinbase
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getBlockCoinbase =
      getIntrinsic(llvm::Intrinsic::evm_coinbase, FT);

  // getBlockDifficulty
  FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy}, false);
  Func_getBlockDifficulty =
      getIntrinsic(llvm::Intrinsic::evm_difficulty, FT);

  // getBlockGasLimit
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockGasLimit =
      getIntrinsic(llvm::Intrinsic::evm_gaslimit, FT);

  // getBlockNumber
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockNumber = getIntrinsic(llvm::Intrinsic::evm_number, FT);

  // getBlockTimestamp
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockTimestamp =
      getIntrinsic(llvm::Intrinsic::evm_timestamp, FT);

#endif
}

void CodeGenModule::initEEIDeclaration() {
  llvm::FunctionType *FT = nullptr;
  llvm::Attribute Ethereum =
      llvm::Attribute::get(VMContext, "wasm-import-module", "ethereum");
  llvm::Attribute Debug =
      llvm::Attribute::get(VMContext, "wasm-import-module", "debug");

  // callDataCopy
  FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
  Func_callDataCopy = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.callDataCopy", TheModule);
  Func_callDataCopy->addFnAttr(Ethereum);
  Func_callDataCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "callDataCopy"));
  Func_callDataCopy->addFnAttr(llvm::Attribute::WriteOnly);
  Func_callDataCopy->addFnAttr(llvm::Attribute::NoUnwind);
  Func_callDataCopy->addParamAttr(0, llvm::Attribute::WriteOnly);

  // callStatic
  FT = llvm::FunctionType::get(
      Int32Ty, {Int64Ty, AddressPtrTy, Int8PtrTy, Int32Ty}, false);
  Func_callStatic = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "ethereum.callStatic", TheModule);
  Func_callStatic->addFnAttr(Ethereum);
  Func_callStatic->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "callStatic"));
  Func_callStatic->addFnAttr(llvm::Attribute::NoUnwind);
  Func_callStatic->addParamAttr(1, llvm::Attribute::ReadOnly);
  Func_callStatic->addParamAttr(2, llvm::Attribute::ReadOnly);

  // finish
  FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
  Func_finish = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                       "ethereum.finish", TheModule);
  Func_finish->addFnAttr(Ethereum);
  Func_finish->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "finish"));
  Func_finish->addFnAttr(llvm::Attribute::WriteOnly);
  Func_finish->addFnAttr(llvm::Attribute::NoUnwind);
  Func_finish->addParamAttr(0, llvm::Attribute::ReadOnly);

  // getCallDataSize
  FT = llvm::FunctionType::get(Int32Ty, {}, false);
  Func_getCallDataSize =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getCallDataSize", TheModule);
  Func_getCallDataSize->addFnAttr(Ethereum);
  Func_getCallDataSize->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getCallDataSize"));
  Func_getCallDataSize->addFnAttr(llvm::Attribute::ReadOnly);
  Func_getCallDataSize->addFnAttr(llvm::Attribute::NoUnwind);

  // getCallValue
  FT = llvm::FunctionType::get(VoidTy, {Int128PtrTy}, false);
  Func_getCallValue = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.getCallValue", TheModule);
  Func_getCallValue->addFnAttr(Ethereum);
  Func_getCallValue->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getCallValue"));

  // getCaller
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getCaller = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                          "ethereum.getCaller", TheModule);
  Func_getCaller->addFnAttr(Ethereum);
  Func_getCaller->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getCaller"));
  Func_getCaller->addFnAttr(llvm::Attribute::ArgMemOnly);
  Func_getCaller->addFnAttr(llvm::Attribute::NoUnwind);
  Func_getCaller->addParamAttr(0, llvm::Attribute::WriteOnly);

  // getCaller
  FT = llvm::FunctionType::get(Int64Ty, {}, false);
  Func_getGasLeft = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "ethereum.getGasLeft", TheModule);
  Func_getGasLeft->addFnAttr(Ethereum);
  Func_getGasLeft->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getGasLeft"));
  Func_getGasLeft->addFnAttr(llvm::Attribute::NoUnwind);

  // log
  FT =
      llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty, Int256PtrTy,
                                       Int256PtrTy, Int256PtrTy, Int256PtrTy},
                              false);
  Func_log = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                    "ethereum.log", TheModule);
  Func_log->addFnAttr(Ethereum);
  Func_log->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "log"));
  Func_log->addFnAttr(llvm::Attribute::ReadOnly);
  Func_log->addFnAttr(llvm::Attribute::NoUnwind);
  Func_log->addParamAttr(0, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(3, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(4, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(5, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(6, llvm::Attribute::ReadOnly);

  // returnDataCopy
  FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
  Func_returnDataCopy =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.returnDataCopy", TheModule);
  Func_returnDataCopy->addFnAttr(Ethereum);
  Func_returnDataCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "returnDataCopy"));
  Func_returnDataCopy->addFnAttr(llvm::Attribute::NoUnwind);
  Func_returnDataCopy->addParamAttr(0, llvm::Attribute::WriteOnly);

  // revert
  FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
  Func_revert = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                       "ethereum.revert", TheModule);
  Func_revert->addFnAttr(Ethereum);
  Func_revert->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "revert"));
  Func_revert->addFnAttr(llvm::Attribute::WriteOnly);
  Func_revert->addFnAttr(llvm::Attribute::NoUnwind);
  Func_revert->addParamAttr(0, llvm::Attribute::ReadOnly);

  // storageLoad
  FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy, Int256PtrTy}, false);
  Func_storageLoad = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "ethereum.storageLoad", TheModule);
  Func_storageLoad->addFnAttr(Ethereum);
  Func_storageLoad->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "storageLoad"));
  Func_storageLoad->addFnAttr(llvm::Attribute::NoUnwind);
  Func_storageLoad->addParamAttr(0, llvm::Attribute::ReadOnly);
  Func_storageLoad->addParamAttr(1, llvm::Attribute::WriteOnly);

  // storageStore
  FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy, Int256PtrTy}, false);
  Func_storageStore = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.storageStore", TheModule);
  Func_storageStore->addFnAttr(Ethereum);
  Func_storageStore->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "storageStore"));
  Func_storageStore->addFnAttr(llvm::Attribute::NoUnwind);
  Func_storageStore->addParamAttr(0, llvm::Attribute::ReadOnly);
  Func_storageStore->addParamAttr(1, llvm::Attribute::ReadOnly);

  // getTxGasPrice
  FT = llvm::FunctionType::get(VoidTy, {Int128PtrTy}, false);
  Func_getTxGasPrice = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.getTxGasPrice", TheModule);
  Func_getTxGasPrice->addFnAttr(Ethereum);
  Func_getTxGasPrice->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getTxGasPrice"));

  // getTxOrigin
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getTxOrigin = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "ethereum.getTxOrigin", TheModule);
  Func_getTxOrigin->addFnAttr(Ethereum);
  Func_getTxOrigin->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getTxOrigin"));

  // getBlockCoinbase
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getBlockCoinbase =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getBlockCoinbase", TheModule);
  Func_getBlockCoinbase->addFnAttr(Ethereum);
  Func_getBlockCoinbase->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getBlockCoinbase"));

  // getBlockDifficulty
  FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy}, false);
  Func_getBlockDifficulty =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getBlockDifficulty", TheModule);
  Func_getBlockDifficulty->addFnAttr(Ethereum);
  Func_getBlockDifficulty->addFnAttr(llvm::Attribute::get(
      VMContext, "wasm-import-name", "getBlockDifficulty"));

  // getBlockGasLimit
  FT = llvm::FunctionType::get(Int64Ty, {}, false);
  Func_getBlockGasLimit =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getBlockGasLimit", TheModule);
  Func_getBlockGasLimit->addFnAttr(Ethereum);
  Func_getBlockGasLimit->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getBlockGasLimit"));

  // getBlockNumber
  FT = llvm::FunctionType::get(Int64Ty, {}, false);
  Func_getBlockNumber =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getBlockNumber", TheModule);
  Func_getBlockNumber->addFnAttr(Ethereum);
  Func_getBlockNumber->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getBlockNumber"));

  // getBlockTimestamp
  FT = llvm::FunctionType::get(Int64Ty, {}, false);
  Func_getBlockTimestamp =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getBlockTimestamp", TheModule);
  Func_getBlockTimestamp->addFnAttr(Ethereum);
  Func_getBlockTimestamp->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getBlockTimestamp"));

  // debug.print32
  FT = llvm::FunctionType::get(VoidTy, {Int32Ty}, false);
  Func_print32 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                        "ethereum.print32", TheModule);
  Func_print32->addFnAttr(Debug);
  Func_print32->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "print32"));
  Func_print32->addFnAttr(llvm::Attribute::ReadOnly);
  Func_print32->addFnAttr(llvm::Attribute::NoUnwind);
}

void CodeGenModule::initHelperDeclaration() {
  if (isEWASM()) {
    Func_bswap256 = llvm::Function::Create(
        llvm::FunctionType::get(Int256Ty, {Int256Ty}, false),
        llvm::Function::InternalLinkage, "solidity.bswapi256", TheModule);
    Func_bswap256->addFnAttr(llvm::Attribute::NoUnwind);
    initBswapI256();
  } else {
    Func_bswap256 = nullptr;
  }

  Func_memcpy = llvm::Function::Create(
      llvm::FunctionType::get(Int8PtrTy, {Int8PtrTy, Int8PtrTy, Int32Ty},
                              false),
      llvm::Function::InternalLinkage, "solidity.memcpy", TheModule);
  Func_memcpy->addFnAttr(llvm::Attribute::NoUnwind);
  initMemcpy();
}

void CodeGenModule::initBswapI256() {
  auto *const Arg = Func_bswap256->arg_begin();

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_bswap256);
  Builder.SetInsertPoint(Entry);

  llvm::Value *data[32];
  for (size_t i = 0; i < 32; ++i) {
    if (i < 16) {
      data[i] = Builder.CreateShl(Arg, 248 - i * 16);
    } else {
      data[i] = Builder.CreateLShr(Arg, i * 16 - 248);
    }
    if (i != 0 && i != 31) {
      data[i] = Builder.CreateAnd(data[i], llvm::APInt(256, 0xFF, false)
                                               << ((31 - i) * 8));
    }
  }
  llvm::Value *result = Builder.CreateOr(data[0], data[1]);
  for (size_t i = 2; i < 32; ++i) {
    result = Builder.CreateOr(result, data[i]);
  }

  Builder.CreateRet(result);
}

void CodeGenModule::initMemcpy() {
  llvm::Argument *const Dst = Func_memcpy->arg_begin();
  llvm::Argument *const Src = Dst + 1;
  llvm::Argument *const Length = Src + 1;
  llvm::ConstantInt *const One = Builder.getInt32(1);
  Dst->setName("dst");
  Src->setName("src");
  Length->setName("length");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_memcpy);
  llvm::BasicBlock *Loop =
      llvm::BasicBlock::Create(VMContext, "loop", Func_memcpy);
  llvm::BasicBlock *Return =
      llvm::BasicBlock::Create(VMContext, "return", Func_memcpy);

  Builder.SetInsertPoint(Entry);
  llvm::Value *Cmp = Builder.CreateICmpNE(Length, Builder.getInt32(0));
  Builder.CreateCondBr(Cmp, Loop, Return);

  Builder.SetInsertPoint(Loop);
  llvm::PHINode *SrcPHI = Builder.CreatePHI(Int8PtrTy, 2);
  llvm::PHINode *DstPHI = Builder.CreatePHI(Int8PtrTy, 2);
  llvm::PHINode *LengthPHI = Builder.CreatePHI(Int32Ty, 2);

  llvm::Value *value = Builder.CreateLoad(SrcPHI);
  Builder.CreateStore(value, DstPHI);
  llvm::Value *Src2 = Builder.CreateInBoundsGEP(SrcPHI, {One});
  llvm::Value *Dst2 = Builder.CreateInBoundsGEP(DstPHI, {One});
  llvm::Value *Length2 = Builder.CreateSub(LengthPHI, One);
  llvm::Value *Cmp2 = Builder.CreateICmpNE(Length2, Builder.getInt32(0));
  Builder.CreateCondBr(Cmp2, Loop, Return);

  Builder.SetInsertPoint(Return);
  Builder.CreateRet(Dst);

  SrcPHI->addIncoming(Src, Entry);
  SrcPHI->addIncoming(Src2, Loop);
  DstPHI->addIncoming(Dst, Entry);
  DstPHI->addIncoming(Dst2, Loop);
  LengthPHI->addIncoming(Length, Entry);
  LengthPHI->addIncoming(Length2, Loop);
}

void CodeGenModule::initPrebuiltContract() {
  llvm::FunctionType *FT = nullptr;

  // keccak256
  FT = llvm::FunctionType::get(Int256Ty, {BytesTy}, false);
  Func_keccak256 = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                          "solidity.keccak256", TheModule);
  Func_keccak256->addFnAttr(llvm::Attribute::NoUnwind);

  // sha256
  Func_sha256 = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                       "solidity.sha256", TheModule);
  Func_sha256->addFnAttr(llvm::Attribute::NoUnwind);

  initKeccak256();
  initSha256();
}

void CodeGenModule::initKeccak256() {
  llvm::Argument *Memory = Func_keccak256->arg_begin();
  Memory->setName("memory");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_keccak256);
  Builder.SetInsertPoint(Entry);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(Memory, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(Memory, {1}, "ptr");

  if (isEVM()) {
    llvm::Value *ResultPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, "result.ptr");
    llvm::Value *Fee = emitGetGasLeft();
    Builder.CreateCall(Func_callStatic,
                       {Fee, Builder.getIntN(256, 9),
                        Builder.CreatePtrToInt(Ptr, EVMIntTy),
                        Builder.CreateZExtOrTrunc(Length, EVMIntTy),
                        Builder.CreatePtrToInt(ResultPtr, EVMIntTy),
                        Builder.getIntN(256, 32)});
    llvm::Value *Result = Builder.CreateLoad(ResultPtr);

    Builder.CreateRet(Result);
  } else if (isEWASM()) {
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 9).byteSwap();
    Builder.CreateStore(Builder.getInt(Address), AddressPtr);

    llvm::Value *Fee = emitGetGasLeft();
    Builder.CreateCall(Func_callStatic, {Fee, AddressPtr, Ptr, Length});
    llvm::Value *ResultPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, "result.ptr");
    llvm::Value *ResultVPtr =
        Builder.CreateBitCast(ResultPtr, Int8PtrTy, "result.vptr");
    Builder.CreateCall(Func_returnDataCopy,
                       {ResultVPtr, Builder.getInt32(0), Builder.getInt32(32)});
    llvm::Value *Result = Builder.CreateLoad(ResultPtr);

    Builder.CreateRet(Result);
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::initSha256() {
  llvm::Argument *Memory = Func_sha256->arg_begin();
  Memory->setName("memory");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_sha256);
  Builder.SetInsertPoint(Entry);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(Memory, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(Memory, {1}, "ptr");

  if (isEVM()) {
    llvm::Value *ResultPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, "result.ptr");
    llvm::Value *Fee = emitGetGasLeft();
    Builder.CreateCall(Func_callStatic,
                       {Fee, Builder.getIntN(256, 2),
                        Builder.CreatePtrToInt(Ptr, EVMIntTy),
                        Builder.CreateZExtOrTrunc(Length, EVMIntTy),
                        Builder.CreatePtrToInt(ResultPtr, EVMIntTy),
                        Builder.getIntN(256, 32)});
    llvm::Value *Result = Builder.CreateLoad(ResultPtr);

    Builder.CreateRet(Result);
  } else if (isEWASM()) {
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 2).byteSwap();
    Builder.CreateStore(Builder.getInt(Address), AddressPtr);

    llvm::Value *Fee = emitGetGasLeft();
    Builder.CreateCall(Func_callStatic, {Fee, AddressPtr, Ptr, Length});
    llvm::Value *ResultPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, "result.ptr");
    llvm::Value *ResultVPtr =
        Builder.CreateBitCast(ResultPtr, Int8PtrTy, "result.vptr");
    Builder.CreateCall(Func_returnDataCopy,
                       {ResultVPtr, Builder.getInt32(0), Builder.getInt32(32)});
    llvm::Value *Result = Builder.CreateLoad(ResultPtr);

    Builder.CreateRet(Result);
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitContractDecl(const ContractDecl *CD) {
  for (const auto *D : CD->getSubNodes()) {
    if (const auto *ED = dynamic_cast<const EventDecl *>(D)) {
      emitEventDecl(ED);
    } else if (const auto *FD = dynamic_cast<const FunctionDecl *>(D)) {
      emitFunctionDecl(FD);
    } else if (const auto *VD = dynamic_cast<const VarDecl *>(D)) {
      emitVarDecl(VD);
    } else {
      assert(false && "unknown subnode type!");
    }
  }
  if (const auto *Constructor = CD->getConstructor()) {
    emitFunctionDecl(Constructor);
  }
  if (const auto *Fallback = CD->getFallback()) {
    emitFunctionDecl(Fallback);
  }

  emitContractConstructorDecl(CD);
  emitContractDispatcherDecl(CD);
}

void CodeGenModule::emitContractConstructorDecl(const ContractDecl *CD) {
  llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);
  llvm::GlobalVariable *deploy_size = new llvm::GlobalVariable(
      TheModule, Int32Ty, true, llvm::GlobalVariable::ExternalLinkage, nullptr,
      "deploy.size");
  deploy_size->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
  deploy_size->setAlignment(8);
  deploy_size->setVisibility(llvm::GlobalValue::HiddenVisibility);
  llvm::GlobalVariable *deploy_data = new llvm::GlobalVariable(
      TheModule, Int8Ty, true, llvm::GlobalVariable::ExternalLinkage, nullptr,
      "deploy.data");
  deploy_data->setAlignment(1);
  deploy_data->setVisibility(llvm::GlobalValue::HiddenVisibility);

  llvm::Function *Ctor = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "solidity.ctor", TheModule);
  Ctor->setVisibility(llvm::Function::VisibilityTypes::HiddenVisibility);
  Ctor->addFnAttr(
      llvm::Attribute::get(VMContext, llvm::Attribute::AlwaysInline));

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(VMContext, "entry", Ctor);
  Builder.SetInsertPoint(Entry);

  if (const auto *Constructor = CD->getConstructor()) {
    llvm::Function *Func = TheModule.getFunction(getMangledName(Constructor));
    Builder.CreateCall(Func, {});
  }

  emitFinish(deploy_data, Builder.CreateLoad(deploy_size));
  Builder.CreateRetVoid();
}

void CodeGenModule::emitContractDispatcherDecl(const ContractDecl *CD) {
  llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);
  llvm::Function *Main = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "solidity.main", TheModule);
  Main->setVisibility(llvm::Function::VisibilityTypes::HiddenVisibility);
  Main->addFnAttr(
      llvm::Attribute::get(VMContext, llvm::Attribute::AlwaysInline));

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(VMContext, "entry", Main);
  llvm::BasicBlock *Error = llvm::BasicBlock::Create(VMContext, "error", Main);

  // two phase codegen
  Builder.SetInsertPoint(Error);
  if (const FunctionDecl *Fallback = CD->getFallback()) {
    assert(Fallback->getParams()->getParams().empty() &&
           "not parameters in fallback functions!");
    assert(Fallback->getReturnParams()->getParams().empty() &&
           "no returns in fallback functions!");
    // fallback

    llvm::Function *Func = TheModule.getFunction(getMangledName(Fallback));

    Builder.CreateCall(Func, {});
    emitFinish(llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0));
    Builder.CreateRetVoid();
  } else {
    Builder.CreateCall(Func_revert, {llvm::ConstantPointerNull::get(Int8PtrTy),
                                     Builder.getInt32(0)});
    Builder.CreateUnreachable();
  }

  std::vector<const FunctionDecl *> PublicFDs;
  for (const FunctionDecl *FD : CD->getFuncs()) {
    switch (FD->getVisibility()) {
    case Decl::Visibility::Default:
    case Decl::Visibility::External:
    case Decl::Visibility::Public:
      PublicFDs.emplace_back(FD);
      break;
    default:
      break;
    }
  }
  if (!PublicFDs.empty()) {
    llvm::BasicBlock *Switch =
        llvm::BasicBlock::Create(VMContext, "switch", Main);

    Builder.SetInsertPoint(Entry);
    llvm::Value *callDataSize =
        Builder.CreateCall(Func_getCallDataSize, {}, "size");
    llvm::Value *cmp =
        Builder.CreateICmpUGE(callDataSize, Builder.getInt32(4), "cmp");
    Builder.CreateCondBr(cmp, Switch, Error);

    Builder.SetInsertPoint(Switch);
    llvm::Value *HashVPtr =
        Builder.CreateAlloca(Int8Ty, Builder.getInt32(4), "hash.vptr");
    emitCallDataCopy(HashVPtr, Builder.getInt32(0), Builder.getInt32(4));
    llvm::Value *HashPtr =
        Builder.CreateBitCast(HashVPtr, Int32PtrTy, "hash.ptr");
    llvm::Value *Hash = Builder.CreateLoad(Int32Ty, HashPtr, "hash");

    llvm::SwitchInst *SI = Builder.CreateSwitch(Hash, Error, PublicFDs.size());

    for (const FunctionDecl *FD : PublicFDs) {
      const std::string &MangledName = getMangledName(FD);
      llvm::BasicBlock *CondBB =
          llvm::BasicBlock::Create(VMContext, MangledName, Main);
      emitABILoad(FD, CondBB, Error, callDataSize);
      SI->addCase(Builder.getInt32(FD->getSignatureHashUInt32()), CondBB);
    }
  } else {
    Builder.SetInsertPoint(Entry);
    Builder.CreateBr(Error);
  }
}

llvm::Value *CodeGenModule::emitABILoadParamStatic(const Type *Ty,
                                                   llvm::StringRef Name,
                                                   llvm::Value *Buffer,
                                                   std::uint32_t Offset) {
  if (const auto *ArrayTy = dynamic_cast<const ArrayType *>(Ty)) {
    if (!ArrayTy->isDynamicSized()) {
      const Type *ElemTy = ArrayTy->getElementType().get();
      const std::uint32_t Size = ElemTy->getABIStaticSize();
      llvm::Value *Result = llvm::UndefValue::get(getStaticLLVMType(ArrayTy));
      for (std::uint32_t I = 0; I < ArrayTy->getLength(); ++I) {
        const std::string &SubName = (Name + "." + std::to_string(I)).str();
        Builder.CreateInsertValue(
            Result, emitABILoadParamStatic(ElemTy, SubName, Buffer, Offset), I);
        Offset += Size;
      }
      return Result;
    }
  }
  llvm::Value *CPtr = Builder.CreateInBoundsGEP(
      Buffer, {Builder.getInt32(Offset)}, Name + ".cptr");
  llvm::Value *Ptr = Builder.CreateBitCast(CPtr, Int256PtrTy, Name + ".ptr");
  llvm::Value *ValB = Builder.CreateLoad(Int256Ty, Ptr, Name + ".b");
  llvm::Value *Val = Builder.CreateCall(Func_bswap256, {ValB}, Name + ".e");
  return Builder.CreateZExtOrTrunc(Val, getLLVMType(Ty), Name);
}

std::pair<llvm::Value *, llvm::Value *> CodeGenModule::emitABILoadParamDynamic(
    const Type *Ty, llvm::Value *Size, llvm::StringRef Name,
    llvm::Value *Buffer, llvm::Value *Offset) {
  if (const auto *ArrayTy = dynamic_cast<const ArrayType *>(Ty)) {
    if (!ArrayTy->isDynamicSized()) {
      const Type *ElemTy = ArrayTy->getElementType().get();
      llvm::Value *Result = llvm::UndefValue::get(getLLVMType(ArrayTy));
      for (std::uint32_t I = 0; I < ArrayTy->getLength(); ++I) {
        const std::string &SubName = (Name + "." + std::to_string(I)).str();
        llvm::Value *SubSize = Builder.CreateExtractValue(Size, I);
        llvm::Value *Arg;
        std::tie(Arg, Offset) =
            emitABILoadParamDynamic(ElemTy, SubSize, SubName, Buffer, Offset);
        Builder.CreateInsertValue(Result, Arg, I);
      }
      return {Result, Offset};
    } else {
      // XXX: implement array ABILoad
      assert(false && "Dynamic array argument not supported yet!");
      __builtin_unreachable();
    }
  } else if (dynamic_cast<const StringType *>(Ty)) {
    llvm::Value *CPtr =
        Builder.CreateInBoundsGEP(Buffer, {Offset}, Name + ".cptr");
    llvm::Value *String = llvm::UndefValue::get(StringTy);
    String = Builder.CreateInsertValue(String, Size, {0});
    String = Builder.CreateInsertValue(String, CPtr, {1});
    return {String, Builder.CreateAdd(Offset, Size)};
  } else if (dynamic_cast<const BytesType *>(Ty)) {
    llvm::Value *CPtr =
        Builder.CreateInBoundsGEP(Buffer, {Offset}, Name + ".cptr");
    llvm::Value *Bytes = llvm::UndefValue::get(BytesTy);
    Bytes = Builder.CreateInsertValue(Bytes, Size, {0});
    Bytes = Builder.CreateInsertValue(Bytes, CPtr, {1});
    ;
    return {Bytes, Builder.CreateAdd(Offset, Size)};
  } else {
    assert(false && "unknown ABILoad dynamic type!");
    __builtin_unreachable();
  }
}

void CodeGenModule::emitABIStore(const Type *Ty, llvm::StringRef Name,
                                 llvm::Value *Result) {
  switch (Ty->getCategory()) {
  case Type::Category::Address:
  case Type::Category::Bool:
  case Type::Category::FixedBytes:
  case Type::Category::Integer: {
    // XXX: check signed
    llvm::Value *RetB = Builder.CreateCall(
        Func_bswap256, {Builder.CreateZExtOrTrunc(Result, Int256Ty)},
        Name + ".ret.b");

    // put return value to returndata
    llvm::Value *RetPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, Name + ".ret.ptr");
    Builder.CreateStore(RetB, RetPtr);
    llvm::Value *RetVPtr =
        Builder.CreateBitCast(RetPtr, Int8PtrTy, Name + ".ret.vptr");
    Builder.CreateCall(Func_finish, {RetVPtr, Builder.getInt32(32)});
    break;
  }
  case Type::Category::String: {
    llvm::Value *RetLen =
        Builder.CreateExtractValue(Result, {0}, Name + ".ret.len");
    llvm::Value *RetLenTrunc =
        Builder.CreateTrunc(RetLen, Int32Ty, Name + ".ret.len.trunc");
    llvm::Value *RetLenB =
        Builder.CreateCall(Func_bswap256, {RetLen}, Name + ".ret.len.b");
    llvm::Value *RetData =
        Builder.CreateExtractValue(Result, {1}, Name + ".ret.data");

    // put return value to returndata
    llvm::Value *RetSize = Builder.CreateAdd(RetLenTrunc, Builder.getInt32(32),
                                             Name + ".ret.size");
    llvm::Value *RetPtr =
        Builder.CreateAlloca(Int8Ty, RetSize, Name + ".ret.ptr");
    llvm::Value *RetLenVPtr =
        Builder.CreateInBoundsGEP(RetPtr, {Builder.getInt32(0)});
    llvm::Value *RetLenPtr =
        Builder.CreateBitCast(RetLenVPtr, Int256PtrTy, Name + ".ret.len.ptr");
    Builder.CreateStore(RetLenB, RetLenPtr);
    llvm::Value *RetDataPtr = Builder.CreateInBoundsGEP(
        RetPtr, Builder.getInt32(32), Name + ".ret.data.ptr");

    Builder.CreateStore(RetLenB, RetLenPtr);

    Builder.CreateCall(Func_memcpy, {RetDataPtr, RetData, RetLenTrunc});

    llvm::Value *RetVPtr =
        Builder.CreateBitCast(RetPtr, Int8PtrTy, Name + ".ret.vptr");
    Builder.CreateCall(Func_finish,
                       {RetVPtr, Builder.CreateTrunc(RetSize, Int32Ty)});
    break;
  }
  default:
    assert(false && "unsupported type!");
  }
}

void CodeGenModule::emitABILoad(const FunctionDecl *FD,
                                llvm::BasicBlock *Loader,
                                llvm::BasicBlock *Error,
                                llvm::Value *callDataSize) {
  Builder.SetInsertPoint(Loader);
  const std::string &MangledName = getMangledName(FD);

  std::vector<llvm::Value *> ArgsVal;
  // get arguments from calldata
  const auto ABIStaticSize = FD->getParams()->getABIStaticSize();
  if (ABIStaticSize > 0) {
    auto *ArgsBuf = Builder.CreateAlloca(
        Int8Ty, Builder.getInt32(ABIStaticSize), MangledName + ".args.buf");
    auto *ArgsPtr =
        Builder.CreateBitCast(ArgsBuf, Int8PtrTy, MangledName + ".args.ptr");
    emitCallDataCopy(ArgsPtr, Builder.getInt32(4),
                     Builder.getInt32(ABIStaticSize));

    const auto &Fparams = FD->getParams()->getParams();
    std::uint32_t Offset = 0;
    std::vector<size_t> ArgsDynamic;
    for (std::size_t i = 0; i < Fparams.size(); i++) {
      const Type *Ty = Fparams[i]->GetType().get();
      if (Ty->isDynamic()) {
        ArgsDynamic.push_back(i);
      }
      const std::uint32_t Size = Ty->getABIStaticSize();
      const std::string &ParamName =
          (MangledName + "." + Fparams[i]->getName()).str();
      ArgsVal.push_back(emitABILoadParamStatic(Ty, ParamName, ArgsBuf, Offset));
      Offset += Size;
    }

    if (!ArgsDynamic.empty()) {
      llvm::BasicBlock *DynamicLoader = llvm::BasicBlock::Create(
          VMContext, MangledName + ".dynamic", Loader->getParent());

      llvm::Value *SizeCheck = Builder.getInt1(true);
      llvm::Value *DynamicSize = Builder.getInt32(0);
      for (size_t i : ArgsDynamic) {
        SizeCheck = Builder.CreateAnd(
            SizeCheck, Builder.CreateICmpULE(
                           ArgsVal[i], Builder.getIntN(256, 0xFFFFFFFFU)));
        DynamicSize = Builder.CreateAdd(
            DynamicSize, Builder.CreateTrunc(ArgsVal[i], Int32Ty));
      }
      llvm::Value *ExceptedCallDataSize =
          Builder.CreateAdd(DynamicSize, Builder.getInt32(Offset + 4));

      SizeCheck = Builder.CreateAnd(
          SizeCheck, Builder.CreateICmpEQ(ExceptedCallDataSize, callDataSize));
      Builder.CreateCondBr(SizeCheck, DynamicLoader, Error);

      Builder.SetInsertPoint(DynamicLoader);
      llvm::Value *ArgDynPtr =
          Builder.CreateAlloca(Int8Ty, DynamicSize, MangledName + ".dyn.ptr");
      emitCallDataCopy(ArgDynPtr, Builder.getInt32(Offset + 4), DynamicSize);

      llvm::Value *Offset = Builder.getInt32(0);
      for (size_t i : ArgsDynamic) {
        llvm::StringRef Name = ArgsVal[i]->getName();
        ArgsVal[i]->setName(Name + ".static");
        llvm::Value *Arg;
        std::tie(Arg, Offset) = emitABILoadParamDynamic(
            Fparams[i]->GetType().get(), ArgsVal[i], Name, ArgDynPtr, Offset);
        ArgsVal[i] = Arg;
      }
    }
  }

  // Call this function
  llvm::Function *F = TheModule.getFunction(MangledName);
  const auto &Returns = FD->getReturnParams()->getParams();
  if (Returns.empty()) {
    Builder.CreateCall(F, ArgsVal);
    Builder.CreateCall(Func_finish, {llvm::ConstantPointerNull::get(Int8PtrTy),
                                     Builder.getInt32(0)});
  } else if (Returns.size() == 1) {
    llvm::Value *Result = Builder.CreateCall(F, ArgsVal, MangledName + ".ret");
    emitABIStore(Returns.front()->GetType().get(), MangledName, Result);
  } else {
    assert(false && "unsupported tuple return!");
  }

  Builder.CreateRetVoid();
}

void CodeGenModule::emitEventDecl(const EventDecl *ED) {
  auto Signature = ED->getSignatureHashUInt32();

  llvm::GlobalVariable *EventSignature = new llvm::GlobalVariable(
      TheModule, Int32Ty, true, llvm::GlobalVariable::InternalLinkage,
      Builder.getInt32(Signature), "solidity.event." + getMangledName(ED));
  EventSignature->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
  EventSignature->setAlignment(1);
}

void CodeGenModule::emitFunctionDecl(const FunctionDecl *FD) {
  if (FD->isConstructor()) {
    assert(FD->getParams()->getParams().empty() &&
           "parameters in constructor not supported!");
    assert(FD->getReturnParams()->getParams().empty() &&
           "no returns in constructor!");
  }
  if (FD->isFallback()) {
    assert(FD->getParams()->getParams().empty() &&
           "no parameters in fallback function!");
    assert(FD->getReturnParams()->getParams().empty() &&
           "no returns in fallback function!");
  }

  llvm::Function *F = createLLVMFunction(FD);
  CodeGenFunction(*this).generateCode(FD, F);
}

void CodeGenModule::emitVarDecl(const VarDecl *VD) {
  const std::size_t Index = StateVarAddrCursor++;
  llvm::GlobalVariable *StateVarAddr = new llvm::GlobalVariable(
      TheModule, Int256Ty, true, llvm::GlobalVariable::InternalLinkage,
      Builder.getIntN(256, Index), VD->getName());
  StateVarAddr->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
  StateVarAddr->setAlignment(256);
  StateVarDeclMap.try_emplace(VD, StateVarAddr);
}

std::string CodeGenModule::getMangledName(const CallableVarDecl *CVD) {
  // XXX: Implement mangling
  std::string Name = CVD->getName();
  for (auto Param : CVD->getParams()->getParams()) {
    Name += '.';
    Name += Param->GetType()->getName();
  }
  return Name;
}

llvm::Type *CodeGenModule::getLLVMType(const Type *Ty) {
  switch (Ty->getCategory()) {
  case Type::Category::Integer:
    return Builder.getIntNTy(
        dynamic_cast<const IntegerType *>(Ty)->getBitNum());
  case Type::Category::FixedBytes:
    return Builder.getIntNTy(
        dynamic_cast<const FixedBytesType *>(Ty)->getBitNum());
  case Type::Category::Bool:
    return BoolTy;
  case Type::Category::Address:
    return AddressTy;
  case Type::Category::String:
    return StringTy;
  case Type::Category::Bytes:
    return BytesTy;
  case Type::Category::Array: {
    auto ArrayTy = dynamic_cast<const ArrayType *>(Ty);
    if (ArrayTy->isDynamicSized()) {
      return llvm::PointerType::getUnqual(
          getLLVMType(ArrayTy->getElementType().get()));
    } else {
      return llvm::ArrayType::get(getLLVMType(ArrayTy->getElementType().get()),
                                  ArrayTy->getLength());
    }
  }
  default:
    assert(false && "unsupported type!");
    __builtin_unreachable();
  }
}

llvm::Type *CodeGenModule::getStaticLLVMType(const Type *Ty) {
  switch (Ty->getCategory()) {
  case Type::Category::Integer:
    return Int256Ty;
  case Type::Category::Bool:
    return Int256Ty;
  case Type::Category::Address:
    return Int256Ty;
  case Type::Category::FixedBytes:
    return Int256Ty;
  case Type::Category::String:
    return Int256Ty;
  case Type::Category::Bytes:
    return Int256Ty;
  case Type::Category::Array: {
    auto ArrayTy = dynamic_cast<const ArrayType *>(Ty);
    if (ArrayTy->isDynamicSized()) {
      return Int256Ty;
    } else {
      return llvm::ArrayType::get(
          getStaticLLVMType(ArrayTy->getElementType().get()),
          ArrayTy->getLength());
    }
  }
  default:
    assert(false && "unsupported type!");
    __builtin_unreachable();
  }
}

llvm::FunctionType *CodeGenModule::getFunctionType(const CallableVarDecl *CVD) {
  llvm::SmallVector<llvm::Type *, 8> ArgTypes;
  for (const auto *Param : CVD->getParams()->getParams()) {
    const Type *Ty = Param->GetType().get();
    ArgTypes.push_back(getLLVMType(Ty));
  }

  llvm::Type *RetTypes;
  const auto &RetList = CVD->getReturnParams()->getParams();
  if (RetList.empty()) {
    RetTypes = VoidTy;
  } else if (RetList.size() == 1) {
    RetTypes = getLLVMType(RetList.front()->GetType().get());
  } else {
    assert(false && "unsupported tuple return!");
    __builtin_unreachable();
  }

  return llvm::FunctionType::get(RetTypes, ArgTypes, false);
}

llvm::Function *CodeGenModule::createLLVMFunction(const CallableVarDecl *CVD) {
  const std::string &MangledName = getMangledName(CVD);
  llvm::Function *F = llvm::Function::Create(getFunctionType(CVD),
                                             llvm::Function::InternalLinkage,
                                             MangledName, TheModule);
  assert(F->getName() == MangledName && "name was uniqued!");
  return F;
}

llvm::Value *CodeGenModule::emitEndianConvert(llvm::Value *Val) {
  // XXX: Assume type of Val is integer
  llvm::Type *Ty = Val->getType();
  llvm::Value *Ext = Builder.CreateZExtOrTrunc(Val, Int256Ty, "extend_256");
  if (const unsigned BitWidth = Ty->getIntegerBitWidth(); BitWidth != 256) {
    Ext = Builder.CreateShl(Ext, 256 - BitWidth, "shift_left");
  }
  llvm::Value *Reverse = Builder.CreateCall(
      getModule().getFunction("solidity.bswapi256"), {Ext}, "reverse");
  return Builder.CreateTrunc(Reverse, Ty, "trunc");
}

llvm::Value *CodeGenModule::emitGetGasLeft() {
  return Builder.CreateCall(Func_getGasLeft, {});
}

void CodeGenModule::emitFinish(llvm::Value *DataOffset, llvm::Value *Length) {
  if (isEVM()) {
    Builder.CreateCall(Func_finish,
                       {Builder.CreatePtrToInt(DataOffset, EVMIntTy),
                        Builder.CreateZExtOrTrunc(Length, EVMIntTy)});

  } else if (isEWASM()) {
    Builder.CreateCall(Func_finish, {DataOffset, Length});
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitCallDataCopy(llvm::Value *ResultOffset,
                                     llvm::Value *DataOffset,
                                     llvm::Value *Length) {
  if (isEVM()) {
    Builder.CreateCall(Func_callDataCopy,
                       {Builder.CreatePtrToInt(ResultOffset, EVMIntTy),
                        Builder.CreateZExtOrTrunc(DataOffset, EVMIntTy),
                        Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
  } else if (isEWASM()) {
    Builder.CreateCall(Func_callDataCopy, {ResultOffset, DataOffset, Length});
  } else {
    __builtin_unreachable();
  }
}

} // namespace soll::CodeGen
