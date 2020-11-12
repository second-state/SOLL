// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenModule.h"
#include "CodeGenFunction.h"
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Constants.h>

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
  evm_log0,
  evm_log1,
  evm_log2,
  evm_log3,
  evm_log4,
  evm_gas,
  evm_sha3,
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

CodeGenModule::CodeGenModule(
    ASTContext &C, llvm::Module &M, std::string &E,
    std::vector<std::pair<std::string, std::string>> &NE,
    DiagnosticsEngine &Diags, const CodeGenOptions &CodeGenOpts,
    const TargetOptions &TargetOpts)
    : Context(C), TheModule(M), Entry(E), NestedEntries(NE), Diags(Diags),
      CodeGenOpts(CodeGenOpts), TargetOpts(TargetOpts),
      VMContext(M.getContext()), Builder(VMContext), StateVarAddrCursor(0) {
  initTypes();
  if (isEVM()) {
    initEVMOpcodeDeclaration();
  } else {
    initMemorySection();
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
  Int160Ty = AddressTy;
  Int256Ty = llvm::Type::getIntNTy(VMContext, 256);

  Int8PtrTy = llvm::Type::getInt8PtrTy(VMContext);
  Int32PtrTy = llvm::Type::getInt32PtrTy(VMContext);
  Int64PtrTy = llvm::Type::getInt64PtrTy(VMContext);
  Int128PtrTy = llvm::Type::getIntNPtrTy(VMContext, 128);
  AddressPtrTy = llvm::Type::getIntNPtrTy(VMContext, 160);
  Int160PtrTy = AddressPtrTy;
  Int256PtrTy = llvm::Type::getIntNPtrTy(VMContext, 256);

  if (isEVM()) {
    ArgsElemTy = Int256Ty;
    ArgsElemPtrTy = Int256PtrTy;
    BytesElemTy = Int256Ty;
    BytesElemPtrTy = Int256PtrTy;
    ReturnElemTy = Int256Ty;
    ReturnElemPtrTy = Int256PtrTy;
  } else if (isEWASM()) {
    ArgsElemTy = Int8Ty;
    ArgsElemPtrTy = Int8PtrTy;
    BytesElemTy = Int8Ty;
    BytesElemPtrTy = Int8PtrTy;
    ReturnElemTy = Int8Ty;
    ReturnElemPtrTy = Int8PtrTy;
  } else {
    __builtin_unreachable();
  }
  BytesTy =
      llvm::StructType::create(VMContext, {Int256Ty, BytesElemPtrTy}, "bytes");
  StringTy = llvm::StructType::create(VMContext, {Int256Ty, BytesElemPtrTy},
                                      "string", false);

  EVMIntTy = Int256Ty;
  EVMIntPtrTy = Int256PtrTy;
}

void CodeGenModule::initMemorySection() {
  MemorySize = new llvm::GlobalVariable(TheModule, Int256Ty, false,
                                        llvm::GlobalVariable::PrivateLinkage,
                                        Builder.getIntN(256, 0), "memory.size");
  MemorySize->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Global);
  MemorySize->setAlignment(llvm::MaybeAlign(256));

  HeapBase = new llvm::GlobalVariable(TheModule, Int8Ty, false,
                                      llvm::GlobalVariable::ExternalLinkage,
                                      nullptr, "__heap_base");
  HeapBase->setAlignment(llvm::MaybeAlign(1));

  Func_updateMemorySize = llvm::Function::Create(
      llvm::FunctionType::get(VoidTy, {Int256Ty, Int256Ty}, false),
      llvm::Function::InternalLinkage, "solidity.updateMemorySize", TheModule);
  Func_updateMemorySize->addFnAttr(llvm::Attribute::NoUnwind);
  initUpdateMemorySize();
}

void CodeGenModule::initUpdateMemorySize() {
  llvm::Argument *Pos = Func_updateMemorySize->arg_begin();
  Pos->setName("memory.pos");
  llvm::Argument *Range = Pos + 1;
  Range->setName("memory.range");
  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_updateMemorySize);
  llvm::BasicBlock *Update =
      llvm::BasicBlock::Create(VMContext, "update", Func_updateMemorySize);
  llvm::BasicBlock *Done =
      llvm::BasicBlock::Create(VMContext, "done", Func_updateMemorySize);
  Builder.SetInsertPoint(Entry);
  llvm::Value *OrigSize = Builder.CreateLoad(MemorySize, "memory.size");
  llvm::Value *EndPos = Builder.CreateAdd(Pos, Range);
  llvm::Value *Condition = Builder.CreateICmpUGT(EndPos, OrigSize);
  Builder.CreateCondBr(Condition, Update, Done);
  Builder.SetInsertPoint(Update);
  llvm::ConstantInt *Mask =
      Builder.getInt(llvm::APInt::getHighBitsSet(256, 251));
  llvm::Value *Base = Builder.CreateAnd(EndPos, Mask);
  llvm::Value *NewSize =
      Builder.CreateAdd(Base, Builder.getIntN(256, 32), "memory.new_size");
  Builder.CreateStore(NewSize, MemorySize);
  Builder.CreateBr(Done);
  Builder.SetInsertPoint(Done);
  Builder.CreateRetVoid();
}

llvm::Function *CodeGenModule::getIntrinsic(unsigned IID,
                                            llvm::ArrayRef<llvm::Type *> Typs) {
  return llvm::Intrinsic::getDeclaration(
      &TheModule, static_cast<llvm::Intrinsic::ID>(IID), llvm::None);
}

void CodeGenModule::initEVMOpcodeDeclaration() {
  llvm::FunctionType *FT = nullptr;
  llvm::Attribute EVMAttr = llvm::Attribute::get(VMContext, "llvm", "evm");

  // evm_calldatacopy
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_callDataCopy = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.calldatacopy", TheModule);
  Func_callDataCopy->addFnAttr(EVMAttr);
  Func_callDataCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "calldatacopy"));
  Func_callDataCopy->addFnAttr(llvm::Attribute::WriteOnly);
  Func_callDataCopy->addFnAttr(llvm::Attribute::NoUnwind);
  Func_callDataCopy->addParamAttr(0, llvm::Attribute::WriteOnly);

  // evm_calldatasize
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getCallDataSize = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.calldatasize", TheModule);
  Func_getCallDataSize->addFnAttr(EVMAttr);
  Func_getCallDataSize->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "calldatasize"));
  Func_getCallDataSize->addFnAttr(llvm::Attribute::ReadOnly);
  Func_getCallDataSize->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_sha3
  FT = llvm::FunctionType::get(EVMIntTy, {EVMIntTy, EVMIntTy}, false);
  Func_sha3 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.sha3", TheModule);
  Func_sha3->addFnAttr(EVMAttr);
  Func_sha3->addFnAttr(llvm::Attribute::get(VMContext, "evm", "sha3"));

  // evm_call
  FT = llvm::FunctionType::get(
      EVMIntTy,
      {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
      false);
  Func_call = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.call", TheModule);
  Func_call->addFnAttr(EVMAttr);
  Func_call->addFnAttr(llvm::Attribute::get(VMContext, "evm", "call"));
  Func_call->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_staticcall
  FT = llvm::FunctionType::get(
      EVMIntTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
      false);
  Func_callStatic = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "llvm.evm.staticcall", TheModule);
  Func_callStatic->addFnAttr(EVMAttr);
  Func_callStatic->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "staticcall"));
  Func_callStatic->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_delegatecall
  FT = llvm::FunctionType::get(
      EVMIntTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
      false);
  Func_callDelegate = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.delegatecall", TheModule);
  Func_callDelegate->addFnAttr(EVMAttr);
  Func_callDelegate->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "delegatecall"));
  Func_callDelegate->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_sstore
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_storageStore = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.sstore", TheModule);
  Func_storageStore->addFnAttr(EVMAttr);
  Func_storageStore->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "sstore"));
  Func_storageStore->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_sload
  FT = llvm::FunctionType::get(EVMIntTy, {EVMIntTy}, false);
  Func_storageLoad = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "llvm.evm.sload", TheModule);
  Func_storageLoad->addFnAttr(EVMAttr);
  Func_storageLoad->addFnAttr(llvm::Attribute::get(VMContext, "evm", "sload"));
  Func_storageLoad->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_caller
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getCaller = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                          "llvm.evm.caller", TheModule);
  Func_getCaller->addFnAttr(EVMAttr);
  Func_getCaller->addFnAttr(llvm::Attribute::get(VMContext, "evm", "caller"));
  Func_getCaller->addFnAttr(llvm::Attribute::ArgMemOnly);
  Func_getCaller->addFnAttr(llvm::Attribute::NoUnwind);
  Func_getCaller->addParamAttr(0, llvm::Attribute::WriteOnly);

  // evm_callvalue
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getCallValue = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.callvalue", TheModule);
  Func_getCallValue->addFnAttr(EVMAttr);
  Func_getCallValue->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "callvalue"));

  // evm_callcopy
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_codeCopy = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "llvm.evm.codecopy", TheModule);
  Func_codeCopy->addFnAttr(EVMAttr);
  Func_codeCopy->addFnAttr(llvm::Attribute::get(VMContext, "evm", "codecopy"));
  Func_codeCopy->addFnAttr(llvm::Attribute::ArgMemOnly);
  Func_codeCopy->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_log0
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_log0 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.log0", TheModule);
  Func_log0->addFnAttr(EVMAttr);
  Func_log0->addFnAttr(llvm::Attribute::get(VMContext, "evm", "log0"));
  Func_log0->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_log1
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_log1 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.log1", TheModule);
  Func_log1->addFnAttr(EVMAttr);
  Func_log1->addFnAttr(llvm::Attribute::get(VMContext, "evm", "log1"));
  Func_log1->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_log2
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
                               false);
  Func_log2 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.log2", TheModule);
  Func_log2->addFnAttr(EVMAttr);
  Func_log2->addFnAttr(llvm::Attribute::get(VMContext, "evm", "log2"));
  Func_log2->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_log3
  FT = llvm::FunctionType::get(
      VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_log3 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.log3", TheModule);
  Func_log3->addFnAttr(EVMAttr);
  Func_log3->addFnAttr(llvm::Attribute::get(VMContext, "evm", "log3"));
  Func_log3->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_log4
  FT = llvm::FunctionType::get(
      VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy, EVMIntTy},
      false);
  Func_log4 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "llvm.evm.log4", TheModule);
  Func_log4->addFnAttr(EVMAttr);
  Func_log4->addFnAttr(llvm::Attribute::get(VMContext, "evm", "log4"));
  Func_log4->addFnAttr(llvm::Attribute::NoUnwind);

  // evm_gas
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getGasLeft = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "llvm.evm.gas", TheModule);
  Func_getGasLeft->addFnAttr(EVMAttr);
  Func_getGasLeft->addFnAttr(llvm::Attribute::get(VMContext, "evm", "gas"));
  Func_getGasLeft->addFnAttr(llvm::Attribute::NoUnwind);

  // finish
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_finish = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                       "llvm.evm.return", TheModule);
  Func_finish->addFnAttr(EVMAttr);
  Func_finish->addFnAttr(llvm::Attribute::get(VMContext, "evm", "return"));
  Func_finish->addFnAttr(llvm::Attribute::WriteOnly);
  Func_finish->addFnAttr(llvm::Attribute::NoUnwind);

  // revert
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy}, false);
  Func_revert = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                       "llvm.evm.revert", TheModule);
  Func_revert->addFnAttr(EVMAttr);
  Func_revert->addFnAttr(llvm::Attribute::get(VMContext, "evm", "revert"));
  Func_revert->addFnAttr(llvm::Attribute::WriteOnly);
  Func_revert->addFnAttr(llvm::Attribute::NoUnwind);

  // returnDataSize
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_returnDataSize =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "llvm.evm.returndatasize", TheModule);
  Func_returnDataSize->addFnAttr(EVMAttr);
  Func_returnDataSize->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "returndatasize"));

  // returnDataCopy
  FT = llvm::FunctionType::get(VoidTy, {EVMIntTy, EVMIntTy, EVMIntTy}, false);
  Func_returnDataCopy =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "llvm.evm.returndatacopy", TheModule);
  Func_returnDataCopy->addFnAttr(EVMAttr);
  Func_returnDataCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "returndatacopy"));
  Func_returnDataCopy->addFnAttr(llvm::Attribute::NoUnwind);

  // getTxGasPrice
  FT = llvm::FunctionType::get(VoidTy, {Int128PtrTy}, false);
  Func_getTxGasPrice = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.gasprice", TheModule);
  Func_getTxGasPrice->addFnAttr(EVMAttr);
  Func_getTxGasPrice->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "gasprice"));

  // getTxOrigin
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getTxOrigin = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "llvm.evm.origin", TheModule);
  Func_getTxOrigin->addFnAttr(EVMAttr);
  Func_getTxOrigin->addFnAttr(llvm::Attribute::get(VMContext, "evm", "origin"));

  // getBlockCoinbase
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getBlockCoinbase = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.coinbase", TheModule);
  Func_getBlockCoinbase->addFnAttr(EVMAttr);
  Func_getBlockCoinbase->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "coinbase"));

  // getBlockDifficulty
  FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy}, false);
  Func_getBlockDifficulty = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.difficulty", TheModule);
  Func_getBlockDifficulty->addFnAttr(EVMAttr);
  Func_getBlockDifficulty->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "difficulty"));

  // getBlockGasLimit
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockGasLimit = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.gaslimit", TheModule);
  Func_getBlockGasLimit->addFnAttr(EVMAttr);
  Func_getBlockGasLimit->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "gaslimit"));

  // getBlockNumber
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockNumber = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.number", TheModule);
  Func_getBlockNumber->addFnAttr(EVMAttr);
  Func_getBlockNumber->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "number"));

  // getBlockTimestamp
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getBlockTimestamp = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.timestamp", TheModule);
  Func_getBlockTimestamp->addFnAttr(EVMAttr);
  Func_getBlockTimestamp->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "timestamp"));

  // getExternalBalance
  FT = llvm::FunctionType::get(Int64PtrTy, {}, false);
  Func_getExternalBalance = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "llvm.evm.balance", TheModule);
  Func_getExternalBalance->addFnAttr(EVMAttr);
  Func_getExternalBalance->addFnAttr(
      llvm::Attribute::get(VMContext, "evm", "balance"));

  // getAddress
  FT = llvm::FunctionType::get(EVMIntTy, {}, false);
  Func_getAddress = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "llvm.evm.address", TheModule);
  Func_getAddress->addFnAttr(EVMAttr);
  Func_getAddress->addFnAttr(llvm::Attribute::get(VMContext, "evm", "address"));
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

  // call
  FT = llvm::FunctionType::get(
      Int32Ty, {Int64Ty, AddressPtrTy, Int128PtrTy, Int8PtrTy, Int32Ty}, false);
  Func_call = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                     "ethereum.call", TheModule);
  Func_call->addFnAttr(Ethereum);
  Func_call->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "call"));
  Func_call->addFnAttr(llvm::Attribute::NoUnwind);
  Func_call->addParamAttr(1, llvm::Attribute::ReadOnly);
  Func_call->addParamAttr(2, llvm::Attribute::ReadOnly);
  Func_call->addParamAttr(3, llvm::Attribute::ReadOnly);

  // callCode
  FT = llvm::FunctionType::get(
      Int32Ty, {Int64Ty, AddressPtrTy, Int128PtrTy, Int8PtrTy, Int32Ty}, false);
  Func_callCode = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "ethereum.callCode", TheModule);
  Func_callCode->addFnAttr(Ethereum);
  Func_callCode->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "callCode"));
  Func_callCode->addFnAttr(llvm::Attribute::NoUnwind);
  Func_callCode->addParamAttr(1, llvm::Attribute::ReadOnly);
  Func_callCode->addParamAttr(2, llvm::Attribute::ReadOnly);
  Func_callCode->addParamAttr(3, llvm::Attribute::ReadOnly);

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

  // callDelegate
  FT = llvm::FunctionType::get(
      Int32Ty, {Int64Ty, AddressPtrTy, Int8PtrTy, Int32Ty}, false);
  Func_callDelegate = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.callDelegate", TheModule);
  Func_callDelegate->addFnAttr(Ethereum);
  Func_callDelegate->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "callDelegate"));
  Func_callDelegate->addFnAttr(llvm::Attribute::NoUnwind);
  Func_callDelegate->addParamAttr(1, llvm::Attribute::ReadOnly);
  Func_callDelegate->addParamAttr(2, llvm::Attribute::ReadOnly);

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

  // codeCopy
  FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
  Func_codeCopy = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "ethereum.codeCopy", TheModule);
  Func_codeCopy->addFnAttr(Ethereum);
  Func_codeCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "codeCopy"));
  Func_codeCopy->addFnAttr(llvm::Attribute::ArgMemOnly);
  Func_codeCopy->addFnAttr(llvm::Attribute::NoUnwind);
  Func_codeCopy->addParamAttr(0, llvm::Attribute::WriteOnly);

  // externalCodeCopy
  FT = llvm::FunctionType::get(
      VoidTy, {Int32PtrTy, Int32PtrTy, Int32Ty, Int32Ty}, false);
  Func_externalCodeCopy =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.externalCodeCopy", TheModule);
  Func_externalCodeCopy->addFnAttr(Ethereum);
  Func_externalCodeCopy->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "externalCodeCopy"));
  Func_externalCodeCopy->addFnAttr(llvm::Attribute::ArgMemOnly);
  Func_externalCodeCopy->addFnAttr(llvm::Attribute::NoUnwind);

  // getGasLeft
  FT = llvm::FunctionType::get(Int64Ty, {}, false);
  Func_getGasLeft = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "ethereum.getGasLeft", TheModule);
  Func_getGasLeft->addFnAttr(Ethereum);
  Func_getGasLeft->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getGasLeft"));
  Func_getGasLeft->addFnAttr(llvm::Attribute::NoUnwind);

  // log
  FT = llvm::FunctionType::get(VoidTy,
                               {Int8PtrTy, Int32Ty, Int32Ty, Int256PtrTy,
                                Int256PtrTy, Int256PtrTy, Int256PtrTy},
                               false);
  Func_log = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                    "ethereum.log", TheModule);
  Func_log->addFnAttr(Ethereum);
  Func_log->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "log"));
  Func_log->addFnAttr(llvm::Attribute::NoUnwind);
  Func_log->addParamAttr(0, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(3, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(4, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(5, llvm::Attribute::ReadOnly);
  Func_log->addParamAttr(6, llvm::Attribute::ReadOnly);

  // returnDataSize
  FT = llvm::FunctionType::get(Int32Ty, {}, false);
  Func_returnDataSize =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.returnDataSize", TheModule);
  Func_returnDataSize->addFnAttr(Ethereum);
  Func_returnDataSize->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "returnDataSize"));

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

  // getBlockHash
  FT = llvm::FunctionType::get(Int32Ty, {Int64Ty, Int256PtrTy}, false);
  Func_getBlockHash = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, "ethereum.getBlockHash", TheModule);
  Func_getBlockHash->addFnAttr(Ethereum);
  Func_getBlockHash->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getBlockHash"));

  // getExternalBalance
  FT = llvm::FunctionType::get(VoidTy, {Int32PtrTy, Int32PtrTy}, false);
  Func_getExternalBalance =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getExternalBalance", TheModule);
  Func_getExternalBalance->addFnAttr(Ethereum);
  Func_getExternalBalance->addFnAttr(llvm::Attribute::get(
      VMContext, "wasm-import-name", "getExternalBalance"));

  // debug.print32
  FT = llvm::FunctionType::get(VoidTy, {Int32Ty}, false);
  Func_print32 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                        "debug.print32", TheModule);
  Func_print32->addFnAttr(Debug);
  Func_print32->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "print32"));
  Func_print32->addFnAttr(llvm::Attribute::NoUnwind);

  // getAddress
  FT = llvm::FunctionType::get(VoidTy, {AddressPtrTy}, false);
  Func_getAddress = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                           "ethereum.getAddress", TheModule);
  Func_getAddress->addFnAttr(Ethereum);
  Func_getAddress->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getAddress"));

  // getCodeSize
  FT = llvm::FunctionType::get(Int32Ty, {}, false);
  Func_getCodeSize = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "ethereum.getCodeSize", TheModule);
  Func_getCodeSize->addFnAttr(Ethereum);
  Func_getCodeSize->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getCodeSize"));

  // getExternalCodeSize
  FT = llvm::FunctionType::get(Int32Ty, {Int32PtrTy}, false);
  Func_getExternalCodeSize =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getExternalCodeSize", TheModule);
  Func_getExternalCodeSize->addFnAttr(Ethereum);
  Func_getExternalCodeSize->addFnAttr(llvm::Attribute::get(
      VMContext, "wasm-import-name", "getExternalCodeSize"));

  // getReturnDataSize
  FT = llvm::FunctionType::get(Int32Ty, {}, false);
  Func_getReturnDataSize =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             "ethereum.getReturnDataSize", TheModule);
  Func_getReturnDataSize->addFnAttr(Ethereum);
  Func_getReturnDataSize->addFnAttr(
      llvm::Attribute::get(VMContext, "wasm-import-name", "getReturnDataSize"));
}

void CodeGenModule::initHelperDeclaration() {
  if (isEWASM()) {
    Func_exp256 = llvm::Function::Create(
        llvm::FunctionType::get(Int256Ty, {Int256Ty, Int256Ty}, false),
        llvm::Function::InternalLinkage, "solidity.expi256", TheModule);
    Func_exp256->addFnAttr(llvm::Attribute::NoUnwind);
    Func_exp256->addFnAttr(llvm::Attribute::ReadNone);
    initExpI256();

    Func_bswap256 = llvm::Function::Create(
        llvm::FunctionType::get(Int256Ty, {Int256Ty}, false),
        llvm::Function::InternalLinkage, "solidity.bswapi256", TheModule);
    Func_bswap256->addFnAttr(llvm::Attribute::NoUnwind);
    Func_bswap256->addFnAttr(llvm::Attribute::ReadNone);
    initBswapI256();
  } else {
    Func_exp256 = nullptr;
    Func_bswap256 = nullptr;
  }

  Func_memcpy = llvm::Function::Create(
      llvm::FunctionType::get(VoidTy, {BytesElemPtrTy, BytesElemPtrTy, Int32Ty},
                              false),
      llvm::Function::InternalLinkage, "solidity.memcpy", TheModule);
  Func_memcpy->addFnAttr(llvm::Attribute::NoUnwind);
  initMemcpy();
}

void CodeGenModule::initExpI256() {
  llvm::Argument *const Base = Func_exp256->arg_begin();
  llvm::Argument *const Exp = Base + 1;
  Base->setName("base");
  Exp->setName("exp");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_exp256);
  llvm::BasicBlock *Loop =
      llvm::BasicBlock::Create(VMContext, "loop", Func_exp256);
  llvm::BasicBlock *Return =
      llvm::BasicBlock::Create(VMContext, "return", Func_exp256);

  llvm::ConstantInt *Zero = Builder.getIntN(256, 0);
  llvm::ConstantInt *One = Builder.getIntN(256, 1);
  {
    Builder.SetInsertPoint(Entry);
    llvm::Value *IsZero = Builder.CreateICmpEQ(Exp, Zero);
    Builder.CreateCondBr(IsZero, Return, Loop);
  }

  llvm::Value *NewResult;
  {
    Builder.SetInsertPoint(Loop);
    llvm::PHINode *ResultPHI = Builder.CreatePHI(Int256Ty, 2);
    llvm::PHINode *PartPHI = Builder.CreatePHI(Int256Ty, 2);
    llvm::PHINode *ExpPHI = Builder.CreatePHI(Int256Ty, 2);

    llvm::Value *BitSet = Builder.CreateTrunc(ExpPHI, Builder.getInt1Ty());
    llvm::Value *BIsZero = Builder.CreateICmpEQ(BitSet, Builder.getInt1(false));
    llvm::Value *Mul = Builder.CreateSelect(BIsZero, One, PartPHI);
    NewResult = Builder.CreateMul(ResultPHI, Mul);
    llvm::Value *NewPart = Builder.CreateMul(PartPHI, PartPHI);
    llvm::Value *NewExp = Builder.CreateLShr(ExpPHI, One);

    ResultPHI->addIncoming(One, Entry);
    ResultPHI->addIncoming(NewResult, Loop);
    PartPHI->addIncoming(Base, Entry);
    PartPHI->addIncoming(NewPart, Loop);
    ExpPHI->addIncoming(Exp, Entry);
    ExpPHI->addIncoming(NewExp, Loop);

    llvm::Value *IsZero = Builder.CreateICmpEQ(NewExp, Zero);
    Builder.CreateCondBr(IsZero, Return, Loop);
  }

  {
    Builder.SetInsertPoint(Return);
    llvm::PHINode *ResultPHI = Builder.CreatePHI(Int256Ty, 2);
    ResultPHI->addIncoming(One, Entry);
    ResultPHI->addIncoming(NewResult, Loop);
    Builder.CreateRet(ResultPHI);
  }
}

void CodeGenModule::initBswapI256() {
  auto *const Arg = Func_bswap256->arg_begin();
  Arg->setName("data");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_bswap256);
  Builder.SetInsertPoint(Entry);

  llvm::Value *Data[32];
  for (size_t I = 0; I < 32; ++I) {
    if (I < 16) {
      Data[I] = Builder.CreateShl(Arg, 248 - I * 16);
    } else {
      Data[I] = Builder.CreateLShr(Arg, I * 16 - 248);
    }
    if (I != 0 && I != 31) {
      Data[I] = Builder.CreateAnd(Data[I], llvm::APInt(256, 0xFF, false)
                                               << ((31 - I) * 8));
    }
  }
  llvm::Value *Result = Builder.CreateOr(Data[0], Data[1]);
  for (size_t I = 2; I < 32; ++I) {
    Result = Builder.CreateOr(Result, Data[I]);
  }

  Builder.CreateRet(Result);
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
  llvm::PHINode *SrcPHI = Builder.CreatePHI(BytesElemPtrTy, 2);
  llvm::PHINode *DstPHI = Builder.CreatePHI(BytesElemPtrTy, 2);
  llvm::PHINode *LengthPHI = Builder.CreatePHI(Int32Ty, 2);

  llvm::Value *Value = Builder.CreateLoad(SrcPHI);
  Builder.CreateStore(Value, DstPHI);
  llvm::Value *Src2 = Builder.CreateInBoundsGEP(SrcPHI, {One});
  llvm::Value *Dst2 = Builder.CreateInBoundsGEP(DstPHI, {One});
  llvm::Value *Length2 = Builder.CreateSub(LengthPHI, One);
  llvm::Value *Cmp2 = Builder.CreateICmpNE(Length2, Builder.getInt32(0));
  Builder.CreateCondBr(Cmp2, Loop, Return);

  Builder.SetInsertPoint(Return);
  Builder.CreateRetVoid();

  SrcPHI->addIncoming(Src, Entry);
  SrcPHI->addIncoming(Src2, Loop);
  DstPHI->addIncoming(Dst, Entry);
  DstPHI->addIncoming(Dst2, Loop);
  LengthPHI->addIncoming(Length, Entry);
  LengthPHI->addIncoming(Length2, Loop);
}

void CodeGenModule::initPrebuiltContract() {
  llvm::FunctionType *FT = nullptr;

  // ripemd160
  FT = llvm::FunctionType::get(Int160Ty, {BytesTy}, false);
  Func_ripemd160 = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                          "solidity.ripemd160", TheModule);
  Func_ripemd160->addFnAttr(llvm::Attribute::NoUnwind);

  // ecrecover
  FT = llvm::FunctionType::get(Int160Ty,
                               {Int256Ty, Int256Ty, Int256Ty, Int256Ty}, false);
  Func_ecrecover = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                          "solidity.ecrecover", TheModule);
  Func_ecrecover->addFnAttr(llvm::Attribute::NoUnwind);

  if (isEWASM()) {
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
  initRipemd160();
  initEcrecover();
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
    llvm::Value *Result = Builder.CreateCall(
        Func_sha3, {Builder.CreatePtrToInt(Ptr, EVMIntTy),
                    Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
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

    Builder.CreateRet(emitEndianConvert(Result));
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
    llvm::Value *Result = Builder.CreateCall(
        Func_sha3, {Builder.CreatePtrToInt(Ptr, EVMIntTy),
                    Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
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

    Builder.CreateRet(emitEndianConvert(Result));
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::initRipemd160() {
  llvm::Argument *Memory = Func_ripemd160->arg_begin();
  Memory->setName("memory");

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_ripemd160);
  Builder.SetInsertPoint(Entry);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(Memory, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(Memory, {1}, "ptr");

  if (isEVM()) {
    llvm::Value *Fee = emitGetGasLeft();
    llvm::Value *ResultPtr = Builder.CreateAlloca(EVMIntTy);
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 3);
    Builder.CreateStore(Builder.getInt(Address), AddressPtr);
    emitCallStatic(Fee, AddressPtr, Ptr, Length, ResultPtr,
                   Builder.getIntN(256, 0x20));
    llvm::Value *Result =
        Builder.CreateTrunc(Builder.CreateLoad(ResultPtr), Int160Ty);
    Builder.CreateRet(Result);
  } else if (isEWASM()) {
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 3).byteSwap();
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

    Builder.CreateRet(Builder.CreateTrunc(emitEndianConvert(Result), Int160Ty));
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::initEcrecover() {
  std::vector<llvm::Value *> Args;
  if (Func_ecrecover->arg_end() - Func_ecrecover->arg_begin() == 4) {
    llvm::Argument *CurrentArg = Func_ecrecover->arg_begin();
    CurrentArg->setName("hash");
    Args.emplace_back(CurrentArg);
    CurrentArg++;
    CurrentArg->setName("v");
    Args.emplace_back(CurrentArg);
    CurrentArg++;
    CurrentArg->setName("r");
    Args.emplace_back(CurrentArg);
    CurrentArg++;
    CurrentArg->setName("s");
    Args.emplace_back(CurrentArg);
  } else {
    assert(false && "ECRecover should have 4 arguments!");
    __builtin_unreachable();
  }

  llvm::BasicBlock *Entry =
      llvm::BasicBlock::Create(VMContext, "entry", Func_ecrecover);
  Builder.SetInsertPoint(Entry);

  llvm::Value *Bytes = emitConcatBytes(llvm::ArrayRef<llvm::Value *>(Args));

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(Bytes, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(Bytes, {1}, "ptr");

  if (isEVM()) {
    llvm::Value *Fee = emitGetGasLeft();
    llvm::Value *ResultPtr = Builder.CreateAlloca(EVMIntTy);
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 1);
    Builder.CreateStore(Builder.getInt(Address), AddressPtr);
    emitCallStatic(Fee, AddressPtr, Ptr, Length, ResultPtr,
                   Builder.getIntN(256, 0x20));
    llvm::Value *Result =
        Builder.CreateTrunc(Builder.CreateLoad(ResultPtr), AddressTy);
    Builder.CreateRet(Result);
  } else if (isEWASM()) {
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(AddressTy, nullptr, "address.ptr");
    llvm::APInt Address = llvm::APInt(160, 1).byteSwap();
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

    Builder.CreateRet(Builder.CreateTrunc(emitEndianConvert(Result), Int160Ty));
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitContractDecl(const ContractDecl *CD) {
  for (const auto *D : CD->getSubNodes()) {
    // TODO: handle struct definition
    if (const auto *ED = dynamic_cast<const EventDecl *>(D)) {
      emitEventDecl(ED);
    } else if (const auto *FD = dynamic_cast<const FunctionDecl *>(D)) {
      emitFunctionDecl(FD);
    } else if (const auto *VD = dynamic_cast<const VarDecl *>(D)) {
      emitVarDecl(VD);
    } else if (const auto *SD = dynamic_cast<const StructDecl *>(D)) {
      emitStructDecl(SD);
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

llvm::Function *CodeGenModule::emitNestedObjectGetter(llvm::StringRef Name) {
  llvm::FunctionType *FT = llvm::FunctionType::get(BytesTy, false);
  return llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Name,
                                TheModule);
}

void CodeGenModule::emitContractConstructorDecl(const ContractDecl *CD) {
  if (CodeGenOpts.Runtime) {
    // Generate runtime version only, no constructor.
    getEntry() = "solidity.main";
    return;
  }

  llvm::Function *Contract = emitNestedObjectGetter("solidity.contract");
  NestedEntries.emplace_back("solidity.main", "solidity.contract");

  llvm::Function *Ctor;
  {
    llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);

    Ctor = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                  "solidity.ctor", TheModule);
    getEntry() = "solidity.ctor";
  }
  Ctor->addFnAttr(
      llvm::Attribute::get(VMContext, llvm::Attribute::AlwaysInline));

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(VMContext, "entry", Ctor);
  Builder.SetInsertPoint(Entry);

  if (const auto *Constructor = CD->getConstructor()) {
    llvm::Function *Func = TheModule.getFunction(getMangledName(Constructor));
    Builder.CreateCall(Func, {});
  }
  auto *ContractBytes = Builder.CreateCall(Contract);
  auto *DeploySize = Builder.CreateTrunc(
      Builder.CreateExtractValue(ContractBytes, {0}), Int32Ty);
  auto *DeployData = Builder.CreateExtractValue(ContractBytes, {1});
  emitFinish(DeployData, DeploySize);
  Builder.CreateRetVoid();
}

void CodeGenModule::emitContractDispatcherDecl(const ContractDecl *CD) {
  llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);
  llvm::Function *Main = llvm::Function::Create(
      FT, llvm::Function::InternalLinkage, "solidity.main", TheModule);
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
    emitRevert(llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0));
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
    llvm::Value *CallDataSize =
        Builder.CreateCall(Func_getCallDataSize, {}, "size");
    llvm::Value *CmpV = nullptr;
    if (isEVM()) {
      CmpV = Builder.getIntN(256, 4);
    } else if (isEWASM()) {
      CmpV = Builder.getInt32(4);
    } else {
      __builtin_unreachable();
    }
    llvm::Value *Cmp = Builder.CreateICmpUGE(CallDataSize, CmpV, "cmp");
    Builder.CreateCondBr(Cmp, Switch, Error);

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
      emitABILoad(FD, CondBB, Error, CallDataSize);
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
  llvm::Type *LLVMTy;
  if (const auto *ArrayTy = dynamic_cast<const ArrayType *>(Ty)) {
    if (!ArrayTy->isDynamicSized()) {
      const Type *ElemTy = ArrayTy->getElementType().get();
      const std::uint32_t Size = ElemTy->getABIStaticSize();
      llvm::Value *Result = llvm::UndefValue::get(getStaticLLVMType(ArrayTy));
      const uint64_t Length = ArrayTy->getLength().getLimitedValue();
      for (std::uint32_t I = 0; I < Length; ++I) {
        const std::string &SubName = (Name + "." + std::to_string(I)).str();
        Builder.CreateInsertValue(
            Result, emitABILoadParamStatic(ElemTy, SubName, Buffer, Offset), I);
        Offset += Size;
      }
      return Result;
    }
    LLVMTy = Int256Ty;
  } else if (dynamic_cast<const StringType *>(Ty)) {
    LLVMTy = Int256Ty;
  } else if (dynamic_cast<const BytesType *>(Ty)) {
    LLVMTy = Int256Ty;
  } else {
    LLVMTy = getLLVMType(Ty);
  }
  llvm::Value *CPtr = Builder.CreateInBoundsGEP(
      Buffer, {Builder.getInt32(Offset)}, Name + ".cptr");
  llvm::Value *Ptr = Builder.CreateBitCast(CPtr, Int256PtrTy, Name + ".ptr");
  llvm::Value *ValB = Builder.CreateLoad(Int256Ty, Ptr, Name + ".b");
  llvm::Value *Val = getEndianlessValue(ValB);
  return Builder.CreateZExtOrTrunc(Val, LLVMTy, Name);
}

llvm::Value *CodeGenModule::emitABILoadParamDynamic(const Type *Ty,
                                                    llvm::Value *Size,
                                                    llvm::StringRef Name,
                                                    llvm::Value *Buffer,
                                                    llvm::Value *Offset) {
  if (const auto *ArrayTy = dynamic_cast<const ArrayType *>(Ty)) {
    if (!ArrayTy->isDynamicSized()) {
      const Type *ElemTy = ArrayTy->getElementType().get();
      llvm::Value *Result = llvm::UndefValue::get(getLLVMType(ArrayTy));
      const uint64_t Length = ArrayTy->getLength().getLimitedValue();
      for (std::uint64_t I = 0; I < Length; ++I) {
        const std::string &SubName = (Name + "." + std::to_string(I)).str();
        llvm::Value *SubSize = Builder.CreateExtractValue(Size, I);
        llvm::Value *Arg =
            emitABILoadParamDynamic(ElemTy, SubSize, SubName, Buffer, Offset);
        Builder.CreateInsertValue(Result, Arg, I);
      }
      return Result;
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
    return String;
  } else if (dynamic_cast<const BytesType *>(Ty)) {
    llvm::Value *CPtr =
        Builder.CreateInBoundsGEP(Buffer, {Offset}, Name + ".cptr");
    llvm::Value *Bytes = llvm::UndefValue::get(BytesTy);
    Bytes = Builder.CreateInsertValue(Bytes, Size, {0});
    Bytes = Builder.CreateInsertValue(Bytes, CPtr, {1});
    return Bytes;
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
    llvm::Value *RetB =
        getEndianlessValue(Builder.CreateZExtOrTrunc(Result, Int256Ty));

    // put return value to returndata
    llvm::Value *RetPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, Name + ".ret.ptr");
    Builder.CreateStore(RetB, RetPtr);
    llvm::Value *RetVPtr =
        Builder.CreateBitCast(RetPtr, ReturnElemPtrTy, Name + ".ret.vptr");
    emitFinish(RetVPtr, Builder.getInt32(32));
    break;
  }
  case Type::Category::Bytes:
  case Type::Category::String: {
    llvm::Value *RetLen =
        Builder.CreateExtractValue(Result, {0}, Name + ".ret.len");
    llvm::Value *RetLenTrunc =
        Builder.CreateTrunc(RetLen, Int32Ty, Name + ".ret.len.trunc");
    llvm::Value *RetLenB = getEndianlessValue(RetLen);
    llvm::Value *RetData =
        Builder.CreateExtractValue(Result, {1}, Name + ".ret.data");

    // put return value to returndata
    llvm::Value *RetSize = Builder.CreateAdd(RetLenTrunc, Builder.getInt32(32),
                                             Name + ".ret.size");
    llvm::Value *RetPtr =
        Builder.CreateAlloca(BytesElemTy, RetSize, Name + ".ret.ptr");
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
        Builder.CreateBitCast(RetPtr, ReturnElemPtrTy, Name + ".ret.vptr");
    emitFinish(RetVPtr, Builder.CreateTrunc(RetSize, Int32Ty));
    break;
  }
  default:
    assert(false && "unsupported type!");
  }
}

void CodeGenModule::emitABILoad(const FunctionDecl *FD,
                                llvm::BasicBlock *Loader,
                                llvm::BasicBlock *Error,
                                llvm::Value *CallDataSize) {
  Builder.SetInsertPoint(Loader);
  const std::string &MangledName = getMangledName(FD);

  std::vector<llvm::Value *> ArgsVal;
  // get arguments from calldata
  const auto ABIStaticSize = FD->getParams()->getABIStaticSize();
  if (ABIStaticSize > 0) {
    auto *ArgsBuf = Builder.CreateAlloca(
        Int8Ty, Builder.getInt32(ABIStaticSize), MangledName + ".args.buf");
    auto *ArgsPtr = Builder.CreateBitCast(ArgsBuf, ArgsElemPtrTy,
                                          MangledName + ".args.ptr");
    emitCallDataCopy(ArgsPtr, Builder.getInt32(4),
                     Builder.getInt32(ABIStaticSize));

    const auto &Fparams = FD->getParams()->getParams();
    std::uint32_t Offset = 0;
    std::vector<size_t> ArgsDynamic;
    for (std::size_t I = 0; I < Fparams.size(); I++) {
      const Type *Ty = Fparams[I]->GetType().get();
      if (Ty->isDynamic()) {
        ArgsDynamic.push_back(I);
      }
      const std::uint32_t Size = Ty->getABIStaticSize();
      const std::string &ParamName =
          (MangledName + "." + Fparams[I]->getName()).str();
      ArgsVal.push_back(emitABILoadParamStatic(Ty, ParamName, ArgsBuf, Offset));
      Offset += Size;
    }

    if (!ArgsDynamic.empty()) {
      llvm::BasicBlock *DynamicLoader = llvm::BasicBlock::Create(
          VMContext, MangledName + ".dynamic", Loader->getParent());
      Builder.CreateBr(DynamicLoader);

      Builder.SetInsertPoint(DynamicLoader);
      llvm::IntegerType *BufferTy = Int8Ty;
      llvm::PointerType *BufferPtrTy = Int8PtrTy;
      if (isEVM()) {
        BufferTy = Int256Ty;
        BufferPtrTy = Int256PtrTy;
      }
      llvm::Value *SizeBuf = Builder.CreateAlloca(
          BufferTy, Builder.getInt32(32), MangledName + ".size.buf");
      llvm::Value *SizePtr = Builder.CreateBitCast(SizeBuf, BufferPtrTy,
                                                   MangledName + ".size.ptr");
      for (size_t I : ArgsDynamic) {
        llvm::StringRef Name = Fparams[I]->getName();
        llvm::Value *Base =
            Builder.CreateTrunc(ArgsVal[I], Int32Ty, Name + ".base");
        emitCallDataCopy(
            SizePtr,
            Builder.CreateAdd(Base, Builder.getInt32(4), Name + ".offset"),
            Builder.getInt32(32));
        llvm::Value *Ptr =
            Builder.CreateBitCast(SizePtr, Int256PtrTy, Name + ".size.ptr");
        llvm::Value *ValB = Builder.CreateLoad(Int256Ty, Ptr, Name + ".size.b");
        llvm::Value *DynamicSize = getEndianlessValue(ValB);
        llvm::Value *ArgDynPtr =
            Builder.CreateAlloca(ArgsElemTy, DynamicSize, Name + ".dyn.ptr");
        emitCallDataCopy(
            ArgDynPtr,
            Builder.CreateAdd(Base, Builder.getInt32(36), Name + ".offset"),
            Builder.CreateTrunc(DynamicSize, Int32Ty,
                                DynamicSize->getName() + ".trunc"));
        llvm::Value *Arg =
            emitABILoadParamDynamic(Fparams[I]->GetType().get(), DynamicSize,
                                    Name, ArgDynPtr, Builder.getInt32(0));
        ArgsVal[I] = Arg;
      }
    }
  }

  // Call this function
  llvm::Function *F = TheModule.getFunction(MangledName);
  const auto &Returns = FD->getReturnParams()->getParams();
  if (Returns.empty()) {
    Builder.CreateCall(F, ArgsVal);
    emitFinish(llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0));
  } else if (Returns.size() == 1) {
    llvm::Value *Result = Builder.CreateCall(F, ArgsVal, MangledName + ".ret");
    emitABIStore(Returns.front()->GetType().get(), MangledName, Result);
  } else {
    assert(false && "unsupported tuple return!");
  }

  Builder.CreateRetVoid();
}

void CodeGenModule::emitEventDecl(const EventDecl *ED) {
  std::vector<llvm::Constant *> Signature;
  for (auto s : ED->getSignatureHash())
    Signature.push_back(Builder.getInt8(s));
  llvm::ArrayType *ATy = llvm::ArrayType::get(Int8Ty, 32);
  llvm::Constant *Init = llvm::ConstantArray::get(ATy, Signature);
  llvm::GlobalVariable *EventSignature = new llvm::GlobalVariable(
      TheModule, ATy, true, llvm::GlobalVariable::InternalLinkage, Init,
      "solidity.event." + getMangledName(ED));
  EventSignature->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
  EventSignature->setAlignment(llvm::MaybeAlign(1));
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

  llvm::Function *F = createOrGetLLVMFunction(FD);
  CodeGenFunction(*this).generateCode(FD, F);
}

void CodeGenModule::emitVarDecl(const VarDecl *VD) {
  const std::size_t Index = StateVarAddrCursor;
  llvm::GlobalVariable *StateVarAddr = new llvm::GlobalVariable(
      TheModule, Int256Ty, true, llvm::GlobalVariable::InternalLinkage,
      Builder.getIntN(256, Index), VD->getName());
  StateVarAddrCursor += VD->GetType()->getStorageSize() / 32;
  StateVarAddr->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
  StateVarAddr->setAlignment(llvm::MaybeAlign(256));
  StateVarDeclMap.try_emplace(VD, StateVarAddr);
}

void CodeGenModule::emitStructDecl(const StructDecl *SD) {
  if (auto STy = dynamic_cast<StructType *>(SD->getType().get())) {
    std::vector<llvm::Type *> LLVMTy;
    for (auto ET : STy->getElementTypes()) {
      LLVMTy.emplace_back(getLLVMType(ET.get()));
    }
    auto Tp = llvm::StructType::create(VMContext, LLVMTy, SD->getName());
    STy->setLLVMType(Tp);
  }
}

void CodeGenModule::emitYulObject(const YulObject *YO) {
  assert(nullptr != YO->getCode());
  for (const auto *O : YO->getObjectList()) {
    emitYulObject(O);
    {
      const std::string Name = O->getName();
      emitNestedObjectGetter(Name + ".object");
      NestedEntries.emplace_back(Name + ".main", Name + ".object");
      /*
      llvm::Function *DataSize = llvm::Function::Create(
          llvm::FunctionType::get(Int256Ty, false),
          llvm::Function::InternalLinkage, ".datasize", TheModule);
      llvm::Function *DataOffset = llvm::Function::Create(
          llvm::FunctionType::get(Int8PtrTy, false),
          llvm::Function::InternalLinkage, ".dataoffset", TheModule);
      */
      LookupYulDataOrYulObject.try_emplace(Name, O);
    }
    getEntry().clear();
  }
  for (const auto *D : YO->getDataList()) {
    emitYulData(D);
  }
  emitYulCode(YO->getCode(), YO->getName());
}

void CodeGenModule::emitYulCode(const YulCode *YC, llvm::StringRef Name) {
  std::string FunctionName = Name.str() + ".main";
  getEntry() = FunctionName;
  llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);
  llvm::Function *Main = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, FunctionName, TheModule);
  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(VMContext, "entry", Main);
  Builder.SetInsertPoint(Entry);
  CodeGenFunction(*this).generateYulCode(YC);
  Builder.CreateRetVoid();
}

void CodeGenModule::emitYulData(const YulData *YD) {
  llvm::StringRef Name = YD->getName();
  std::string Data = YD->getBody()->getValue();
  llvm::GlobalVariable *Variable =
      createGlobalString(VMContext, getModule(), Data, Name);
  YulDataMap.try_emplace(YD, Variable);
  LookupYulDataOrYulObject.try_emplace(Name, YD);
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
                                  ArrayTy->getLength().getLimitedValue());
    }
  }
  case Type::Category::Struct:
    return dynamic_cast<const StructType *>(Ty)->getLLVMType();
  case Type::Category::Mapping:
    assert(false && "Mapping is unsupported!");
    __builtin_unreachable();
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
          ArrayTy->getLength().getLimitedValue());
    }
  }
  case Type::Category::Struct: // TODO: static struct type
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

  llvm::SmallVector<llvm::Type *, 8> RetTypes;
  for (const auto *Param : CVD->getReturnParams()->getParams()) {
    const Type *Ty = Param->GetType().get();
    RetTypes.push_back(getLLVMType(Ty));
  }

  llvm::Type *RetType;
  if (RetTypes.empty()) {
    RetType = VoidTy;
  } else if (RetTypes.size() == 1) {
    RetType = RetTypes.front();
  } else {
    RetType = llvm::StructType::create(llvm::ArrayRef<llvm::Type *>(RetTypes));
  }

  assert(llvm::FunctionType::isValidReturnType(RetType));
  return llvm::FunctionType::get(RetType, ArgTypes, false);
}

llvm::Function *
CodeGenModule::createOrGetLLVMFunction(const CallableVarDecl *CVD) {
  const std::string &MangledName = getMangledName(CVD);
  llvm::Function *F = getModule().getFunction(MangledName);
  if (F == nullptr) {
    F = llvm::Function::Create(getFunctionType(CVD),
                               llvm::Function::InternalLinkage, MangledName,
                               TheModule);
  }
  assert(F->getName() == MangledName && "name was uniqued!");
  return F;
}

llvm::Value *CodeGenModule::getEndianlessValue(llvm::Value *Val) {
  if (isEVM()) {
    return Val;
  } else if (isEWASM()) {
    return emitEndianConvert(Val);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitEndianConvert(llvm::Value *Val) {
  llvm::StringRef Name = Val->getName();
  llvm::Type *Ty = Val->getType();
  llvm::Value *Ext = Builder.CreateZExtOrTrunc(Val, Int256Ty, "extend_256");
  if (const unsigned BitWidth = Ty->getIntegerBitWidth(); BitWidth != 256) {
    Ext = Builder.CreateShl(Ext, 256 - BitWidth, "shift_left");
  }
  llvm::Value *Reverse =
      Builder.CreateCall(Func_bswap256, {Ext}, Name + ".reverse");
  return Builder.CreateTrunc(Reverse, Ty, Name + ".trunc");
}

llvm::Value *CodeGenModule::emitExp(llvm::Value *Base, llvm::Value *Exp,
                                    bool IsSigned) {
  llvm::Value *Base256;
  if (IsSigned) {
    Base256 = Builder.CreateSExt(Base, Int256Ty);
  } else {
    Base256 = Builder.CreateZExt(Base, Int256Ty);
  }
  llvm::Value *Exp256 = Builder.CreateZExt(Exp, Int256Ty);
  llvm::Value *Result256 = Builder.CreateCall(Func_exp256, {Base256, Exp256});
  return Builder.CreateTrunc(Result256, Base->getType());
}

bool CodeGenModule::isDynamicType(llvm::Type *Ty) {
  return Ty->getTypeID() == BytesTy->getTypeID() ||
         Ty->getTypeID() == StringTy->getTypeID();
}

llvm::Value *
CodeGenModule::emitConcatBytes(llvm::ArrayRef<llvm::Value *> Values) {
  unsigned BitWidth = 8;
  if (isEVM()) {
    BitWidth = 256;
  }

  llvm::Value *ArrayLength = Builder.getInt32(0);
  for (llvm::Value *Value : Values) {
    llvm::Type *Ty = Value->getType();
    if (isDynamicType(Ty)) {
      llvm::Value *Length = Builder.CreateZExtOrTrunc(
          Builder.CreateExtractValue(Value, {0}), Int32Ty);
      ArrayLength = Builder.CreateAdd(ArrayLength, Length);
    } else {
      ArrayLength = Builder.CreateAdd(
          ArrayLength, Builder.getInt32(Ty->getIntegerBitWidth() / BitWidth));
    }
  }

  llvm::Value *Array = Builder.CreateAlloca(BytesElemTy, ArrayLength, "concat");
  llvm::Value *Index = Builder.getInt32(0);
  for (llvm::Value *Value : Values) {
    llvm::Value *Ptr = Builder.CreateInBoundsGEP(Array, {Index});
    llvm::Type *Ty = Value->getType();
    if (isDynamicType(Ty)) {
      llvm::Value *Length = Builder.CreateZExtOrTrunc(
          Builder.CreateExtractValue(Value, {0}), Int32Ty);
      llvm::Value *SrcBytes = Builder.CreateExtractValue(Value, {1});
      Builder.CreateCall(
          Func_memcpy,
          {Builder.CreateBitCast(Ptr, BytesElemPtrTy), SrcBytes, Length});
      Index = Builder.CreateAdd(Index, Length);
    } else {
      llvm::Value *CPtr =
          Builder.CreatePointerCast(Ptr, llvm::PointerType::getUnqual(Ty));
      Builder.CreateStore(Value, CPtr);
      Index = Builder.CreateAdd(
          Index, Builder.getInt32(Ty->getIntegerBitWidth() / BitWidth));
    }
  }

  llvm::Value *Bytes = llvm::ConstantAggregateZero::get(BytesTy);
  Bytes = Builder.CreateInsertValue(
      Bytes, Builder.CreateZExtOrTrunc(ArrayLength, Int256Ty), {0});
  Bytes = Builder.CreateInsertValue(Bytes, Array, {1});
  return Bytes;
}

void CodeGenModule::emitUpdateMemorySize(llvm::Value *Pos, llvm::Value *Range) {
  Builder.CreateCall(Func_updateMemorySize, {Pos, Range});
}

llvm::Value *CodeGenModule::emitGetGasLeft() {
  return Builder.CreateCall(Func_getGasLeft, {});
}

llvm::Value *CodeGenModule::emitGetCallValue() {
  if (isEVM()) {
    return Builder.CreateCall(Func_getCallValue, {});
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(Int128Ty);
    Builder.CreateCall(Func_getCallValue, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetCaller() {
  if (isEVM()) {
    llvm::Value *Address = Builder.CreateCall(Func_getCaller, {});
    return Builder.CreateZExtOrTrunc(Address, AddressTy);
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(AddressTy);
    Builder.CreateCall(Func_getCaller, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitCodeCopy(llvm::Value *MemOffset, llvm::Value *Pos,
                                 llvm::Value *Length) {
  if (isEVM()) {
    Length = Builder.CreateZExtOrTrunc(Length, EVMIntTy);
    Pos = Builder.CreateZExtOrTrunc(Length, EVMIntTy);
    Builder.CreateCall(
        Func_codeCopy,
        {Builder.CreatePtrToInt(
             Builder.CreateBitCast(MemOffset, Builder.getInt8PtrTy()),
             EVMIntTy),
         Pos, Length});
  } else if (isEWASM()) {
    Builder.CreateCall(
        Func_codeCopy,
        {Builder.CreateBitCast(MemOffset, Builder.getInt8PtrTy()),
         Builder.CreateZExtOrTrunc(Pos, Int32Ty),
         Builder.CreateZExtOrTrunc(Length, Int32Ty)});
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitLog(llvm::Value *DataOffset, llvm::Value *DataLength,
                            std::vector<llvm::Value *> &Topics) {
  if (isEVM()) {
    llvm::Value *Length = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    switch (Topics.size()) {
    case 0:
      Builder.CreateCall(
          Func_log0,
          {Builder.CreatePtrToInt(
               Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()),
               EVMIntTy),
           Length});
      break;
    case 1:
      Builder.CreateCall(
          Func_log1,
          {Builder.CreatePtrToInt(
               Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()),
               EVMIntTy),
           Length, Builder.CreateLoad(Topics[0])});
      break;
    case 2:
      Builder.CreateCall(
          Func_log2,
          {Builder.CreatePtrToInt(
               Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()),
               EVMIntTy),
           Length, Builder.CreateLoad(Topics[0]),
           Builder.CreateLoad(Topics[1])});
      break;
    case 3:
      Builder.CreateCall(
          Func_log3,
          {Builder.CreatePtrToInt(
               Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()),
               EVMIntTy),
           Length, Builder.CreateLoad(Topics[0]), Builder.CreateLoad(Topics[1]),
           Builder.CreateLoad(Topics[2])});
      break;
    case 4:
      Builder.CreateCall(
          Func_log4,
          {Builder.CreatePtrToInt(
               Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()),
               EVMIntTy),
           Length, Builder.CreateLoad(Topics[0]), Builder.CreateLoad(Topics[1]),
           Builder.CreateLoad(Topics[2]), Builder.CreateLoad(Topics[3])});
      break;
    default:
      __builtin_unreachable();
    }
  } else if (isEWASM()) {
    llvm::Value *NumberOfTopics = Builder.getInt32(Topics.size());
    for (size_t I = Topics.size(); I < 4; ++I) {
      Topics.emplace_back(llvm::ConstantPointerNull::get(Int256PtrTy));
    }
    Builder.CreateCall(
        Func_log,
        {Builder.CreateBitCast(DataOffset, Builder.getInt8PtrTy()), DataLength,
         NumberOfTopics, Topics[0], Topics[1], Topics[2], Topics[3]});
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitStorageStore(llvm::Value *Address, llvm::Value *Value) {
  if (isEVM()) {
    Builder.CreateCall(Func_storageStore, {Address, Value});
  } else if (isEWASM()) {
    llvm::Value *AddressPtr = Builder.CreateAlloca(Int256Ty, nullptr);
    llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
    Builder.CreateStore(Address, AddressPtr);
    Builder.CreateStore(Value, ValPtr);
    Builder.CreateCall(Func_storageStore, {AddressPtr, ValPtr});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitStorageLoad(llvm::Value *Address) {
  if (isEVM()) {
    return Builder.CreateCall(Func_storageLoad, {Address});
  } else if (isEWASM()) {
    llvm::Value *AddressPtr = Builder.CreateAlloca(Int256Ty, nullptr);
    llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
    Builder.CreateStore(Address, AddressPtr);
    Builder.CreateCall(Func_storageLoad, {AddressPtr, ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitRevert(llvm::Value *DataOffset, llvm::Value *Length) {
  if (isEVM()) {
    Builder.CreateCall(Func_revert,
                       {Builder.CreatePtrToInt(DataOffset, EVMIntTy),
                        Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
  } else if (isEWASM()) {
    Builder.CreateCall(Func_revert, {DataOffset, Length});
  } else {
    __builtin_unreachable();
  }
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
    Builder.CreateCall(Func_callDataCopy, 
                       {Builder.CreateIntToPtr(ResultOffset, Int8PtrTy), 
                        Builder.CreateZExtOrTrunc(DataOffset, Int32Ty), 
                        Builder.CreateZExtOrTrunc(Length, Int32Ty)});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitReturnDataSize() {
  if (isEVM() || isEWASM()) {
    return Builder.CreateCall(Func_returnDataSize, {});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitReturnDataCopyBytes(llvm::Value *DataOffset,
                                                    llvm::Value *DataLength) {
  llvm::Value *ResultOffset = Builder.CreateAlloca(ReturnElemTy, DataLength);
  if (isEVM()) {
    auto DestOffset = Builder.CreatePtrToInt(ResultOffset, EVMIntTy);
    auto Offset = Builder.CreateZExtOrTrunc(DataOffset, EVMIntTy);
    auto Length = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    Builder.CreateCall(Func_returnDataCopy, {DestOffset, Offset, Length});
  } else if (isEWASM()) {
    Builder.CreateCall(Func_returnDataCopy,
                       {ResultOffset, DataOffset, DataLength});
  } else {
    __builtin_unreachable();
  }
  llvm::Value *Bytes = llvm::ConstantAggregateZero::get(BytesTy);
  Bytes = Builder.CreateInsertValue(
      Bytes, Builder.CreateZExtOrTrunc(DataLength, Int256Ty), {0});
  Bytes = Builder.CreateInsertValue(Bytes, ResultOffset, {1});
  return Bytes;
}

void CodeGenModule::emitReturnDataCopy(llvm::Value *ResultOffset,
                                       llvm::Value *DataOffset,
                                       llvm::Value *DataLength) {
  if (isEVM()) {
    auto DestOffset = Builder.CreatePtrToInt(ResultOffset, EVMIntTy);
    auto Offset = Builder.CreateZExtOrTrunc(DataOffset, EVMIntTy);
    auto Length = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    Builder.CreateCall(Func_returnDataCopy, {DestOffset, Offset, Length});
  } else if (isEWASM()) {
    auto DestOffset = Builder.CreatePtrToInt(ResultOffset, Int8PtrTy);
    auto Offset = Builder.CreateZExtOrTrunc(DataOffset, Int32Ty);
    auto Length = Builder.CreateZExtOrTrunc(DataLength, Int32Ty);
    Builder.CreateCall(Func_returnDataCopy, {DestOffset, Offset, Length});
  } else {
    __builtin_unreachable();
  }
  return;
}

llvm::Value *CodeGenModule::emitCallDataLoad(llvm::Value *DataOffset) {
  llvm::Value *CallDataVPtr =
      Builder.CreateAlloca(Int8Ty, Builder.getIntN(256, 32));
  emitCallDataCopy(CallDataVPtr, Builder.CreateZExtOrTrunc(DataOffset, Int32Ty),
                   Builder.getInt32(32));
  llvm::Value *CallDataPtr = Builder.CreateBitCast(CallDataVPtr, Int256PtrTy);
  llvm::Value *CallData = Builder.CreateLoad(Int256Ty, CallDataPtr);
  return CallData;
}

llvm::Value *CodeGenModule::emitCall(llvm::Value *Gas, llvm::Value *AddressPtr,
                                     llvm::Value *ValuePtr,
                                     llvm::Value *DataPtr,
                                     llvm::Value *DataLength,
                                     llvm::Value *RetOffset,
                                     llvm::Value *RetLength) {
  if (isEVM()) {
    auto Addr = Builder.CreatePtrToInt(AddressPtr, EVMIntTy);
    auto Value = Builder.CreatePtrToInt(ValuePtr, EVMIntTy);
    auto ArgOffset = Builder.CreatePtrToInt(DataPtr, EVMIntTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    if (RetOffset == nullptr) {
      RetOffset = ArgOffset;
    }
    if (RetLength == nullptr) {
      RetLength = Builder.CreateZExtOrTrunc(Builder.getIntN(256, 0), EVMIntTy);
    }
    return Builder.CreateCall(Func_call, {Gas, Addr, Value, ArgOffset,
                                          ArgsLength, RetOffset, RetLength});
  } else if (isEWASM()) {
    auto GasConv = Builder.CreateZExtOrTrunc(Gas, Int64Ty);
    auto Addr = Builder.CreateBitCast(AddressPtr, AddressPtrTy);
    auto Value = Builder.CreateBitCast(ValuePtr, Int128PtrTy);
    auto ArgOffset = Builder.CreateBitCast(DataPtr, Int8PtrTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, Int32Ty);
    llvm::Value *Val = Builder.CreateCall(
        Func_call, {GasConv, Addr, Value, ArgOffset, ArgsLength});
    return Val;
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *
CodeGenModule::emitCallCode(llvm::Value *Gas, llvm::Value *AddressPtr,
                            llvm::Value *ValuePtr, llvm::Value *DataPtr,
                            llvm::Value *DataLength, llvm::Value *RetOffset,
                            llvm::Value *RetLength) {
  if (isEVM()) {
    assert(false && "EEI callCode not supported in EVM yet");
  } else if (isEWASM()) {
    auto GasConv = Builder.CreateZExtOrTrunc(Gas, Int64Ty);
    auto Addr = Builder.CreateBitCast(AddressPtr, AddressPtrTy);
    auto Value = Builder.CreateBitCast(ValuePtr, Int128PtrTy);
    auto ArgOffset = Builder.CreateBitCast(DataPtr, Int8PtrTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, Int32Ty);
    llvm::Value *Val = Builder.CreateCall(
        Func_callCode, {GasConv, Addr, Value, ArgOffset, ArgsLength});
    return Val;
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *
CodeGenModule::emitCallStatic(llvm::Value *Gas, llvm::Value *AddressPtr,
                              llvm::Value *DataPtr, llvm::Value *DataLength,
                              llvm::Value *RetOffset, llvm::Value *RetLength) {
  if (isEVM()) {
    Gas = Builder.CreateZExtOrTrunc(Gas, EVMIntTy);
    auto Addr = Builder.CreatePtrToInt(AddressPtr, EVMIntTy);
    auto ArgOffset = Builder.CreatePtrToInt(DataPtr, EVMIntTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    if (RetOffset == nullptr) {
      RetOffset = ArgOffset;
    } else {
      RetOffset = Builder.CreatePtrToInt(RetOffset, EVMIntTy);
    }
    if (RetLength == nullptr) {
      RetLength = Builder.CreateZExtOrTrunc(Builder.getIntN(256, 0), EVMIntTy);
    }
    return Builder.CreateCall(
        Func_callStatic,
        {Gas, Addr, ArgOffset, ArgsLength, RetOffset, RetLength});
  } else if (isEWASM()) {
    llvm::Value *Val = Builder.CreateCall(
        Func_callStatic, {Gas, AddressPtr, DataPtr, DataLength});
    return Val;
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitCallDelegate(llvm::Value *Gas,
                                             llvm::Value *AddressPtr,
                                             llvm::Value *DataPtr,
                                             llvm::Value *DataLength) {
  if (isEVM()) {
    Gas = Builder.CreateZExtOrTrunc(Gas, EVMIntTy);
    auto Addr = Builder.CreatePtrToInt(AddressPtr, EVMIntTy);
    auto ArgOffset = Builder.CreatePtrToInt(DataPtr, EVMIntTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, EVMIntTy);
    auto RetLength = Builder.CreateZExtOrTrunc(Builder.getInt32(0), EVMIntTy);
    return Builder.CreateCall(
        Func_callDelegate,
        {Gas, Addr, ArgOffset, ArgsLength, ArgOffset, RetLength});
  } else if (isEWASM()) {
    auto GasConv = Builder.CreateZExtOrTrunc(Gas, Int64Ty);
    auto Addr = Builder.CreateBitCast(AddressPtr, AddressPtrTy);
    auto ArgOffset = Builder.CreateBitCast(DataPtr, Int8PtrTy);
    auto ArgsLength = Builder.CreateZExtOrTrunc(DataLength, Int32Ty);
    llvm::Value *Val = Builder.CreateCall(
        Func_callDelegate, {GasConv, Addr, ArgOffset, ArgsLength});
    return Val;
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitKeccak256(llvm::Value *Bytes) {
  llvm::Value *Result = nullptr;
  if (isEVM()) {
    llvm::Value *Length =
        Builder.CreateTrunc(Builder.CreateExtractValue(Bytes, {0}), Int256Ty);
    llvm::Value *Offset = Builder.CreateExtractValue(Bytes, {1});
    Result = Builder.CreateCall(Func_sha3,
                                {Builder.CreatePtrToInt(Offset, EVMIntTy),
                                 Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
  } else if (isEWASM()) {
    if (TargetOpts.DeployPlatform == Chain) {
      Result = Builder.CreateCall(Func_sha256, {Bytes});
    } else {
      Result = Builder.CreateCall(Func_keccak256, {Bytes});
    }
  } else {
    __builtin_unreachable();
  }
  return Result;
}

llvm::Value *CodeGenModule::emitKeccak256(llvm::Value *Offset,
                                          llvm::Value *Length) {
  llvm::Value *Result = nullptr;
  if (isEVM()) {
    Result = Builder.CreateCall(Func_sha3,
                                {Builder.CreatePtrToInt(Offset, EVMIntTy),
                                 Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
  } else if (isEWASM()) {
    llvm::Value *Bytes = llvm::ConstantAggregateZero::get(BytesTy);
    Bytes = Builder.CreateInsertValue(
        Bytes, Builder.CreateZExtOrTrunc(Length, Int256Ty), {0});
    Bytes = Builder.CreateInsertValue(Bytes, Offset, {1});
    if (TargetOpts.DeployPlatform == Chain) {
      Result = Builder.CreateCall(Func_sha256, {Bytes});
    } else {
      Result = Builder.CreateCall(Func_keccak256, {Bytes});
    }
  } else {
    __builtin_unreachable();
  }
  return Result;
}

llvm::Value *CodeGenModule::emitSha256(llvm::Value *Bytes) {
  llvm::Value *Result = nullptr;
  if (isEVM()) {
    llvm::Value *Length =
        Builder.CreateTrunc(Builder.CreateExtractValue(Bytes, {0}), Int256Ty);
    llvm::Value *Offset = Builder.CreateExtractValue(Bytes, {1});
    Result = Builder.CreateCall(Func_sha3,
                                {Builder.CreatePtrToInt(Offset, EVMIntTy),
                                 Builder.CreateZExtOrTrunc(Length, EVMIntTy)});
  } else if (isEWASM()) {
    Result = Builder.CreateCall(Func_sha256, {Bytes});
  } else {
    __builtin_unreachable();
  }
  return Result;
}

void CodeGenModule::emitMemcpy(llvm::Value *Dst, llvm::Value *Src,
                               llvm::Value *Length) {
  Builder.CreateCall(Func_memcpy, {Dst, Src, Length});
}

llvm::Value *CodeGenModule::emitGetCallDataSize() {
  if (isEVM() || isEWASM()) {
    return Builder.CreateCall(Func_getCallDataSize, {});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetTxGasPrice() {
  if (isEVM()) {
    return Builder.CreateCall(Func_getTxGasPrice, {});
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(Int128Ty);
    Builder.CreateCall(Func_getTxGasPrice, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetTxOrigin() {
  if (isEVM()) {
    return Builder.CreateTrunc(Builder.CreateCall(Func_getTxOrigin, {}),
                               AddressTy);
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(AddressTy);
    Builder.CreateCall(Func_getTxOrigin, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockCoinbase() {
  if (isEVM()) {
    return Builder.CreateTrunc(Builder.CreateCall(Func_getBlockCoinbase, {}),
                               AddressTy);
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(AddressTy);
    Builder.CreateCall(Func_getBlockCoinbase, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockDifficulty() {
  if (isEVM()) {
    return Builder.CreateCall(Func_getBlockDifficulty, {});
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty);
    Builder.CreateCall(Func_getBlockDifficulty, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockGasLimit() {
  if (isEVM() || isEWASM()) {
    return Builder.CreateZExt(Builder.CreateCall(Func_getBlockGasLimit, {}),
                              Int256Ty);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockNumber() {
  if (isEVM() || isEWASM()) {
    return Builder.CreateZExt(Builder.CreateCall(Func_getBlockNumber, {}),
                              Int256Ty);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockTimestamp() {
  if (isEVM() || isEWASM()) {
    return Builder.CreateZExt(Builder.CreateCall(Func_getBlockTimestamp, {}),
                              Int256Ty);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetBlockHash(llvm::Value *Number) {
  if (isEVM()) {
    auto BlockNumber = Builder.CreateZExtOrTrunc(Number, EVMIntTy);
    return Builder.CreateCall(Func_getBlockHash, {BlockNumber});
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty);
    Builder.CreateCall(Func_getBlockHash,
                       {Builder.CreateZExtOrTrunc(Number, Int64Ty), ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetExternalBalance(llvm::Value *Address) {
  if (isEVM()) {
    auto Addr = Builder.CreatePtrToInt(Address, EVMIntTy);
    return Builder.CreateCall(Func_getExternalBalance, {Addr});
  } else if (isEWASM()) {
    llvm::Value *AddressPtr = Builder.CreateAlloca(Int256Ty);
    llvm::Value *ValPtr = Builder.CreateAlloca(Int128Ty);
    Builder.CreateStore(Builder.CreateZExtOrTrunc(Address, Int256Ty),
                        AddressPtr);
    Builder.CreateCall(Func_getExternalBalance,
                       {Builder.CreateBitCast(AddressPtr, Int32PtrTy),
                        Builder.CreateBitCast(ValPtr, Int32PtrTy)});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetAddress() {
  if (isEVM()) {
    llvm::Value *Val = Builder.CreateCall(Func_getAddress, {});
    return Builder.CreateZExtOrTrunc(Val, AddressTy);
  } else if (isEWASM()) {
    llvm::Value *ValPtr = Builder.CreateAlloca(AddressTy);
    Builder.CreateCall(Func_getAddress, {ValPtr});
    return Builder.CreateLoad(ValPtr);
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetCodeSize() {
  if (isEVM()) {
    assert(false && "EEI getCodeSize not supported in EVM yet");
  } else if (isEWASM()) {
    return Builder.CreateCall(Func_getCodeSize, {});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetExternalCodeSize(llvm::Value *Address) {
  if (isEVM()) {
    assert(false && "EEI getExternalCodeSize not supported in EVM yet");
  } else if (isEWASM()) {
    return Builder.CreateCall(Func_getExternalCodeSize, {Address});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitGetReturnDataSize() {
  if (isEVM()) {
    assert(false && "EEI getReturnDataSize not supported in EVM yet");
  } else if (isEWASM()) {
    return Builder.CreateCall(Func_getReturnDataSize, {});
  } else {
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenModule::emitExternalCodeCopy(llvm::Value *Address,
                                                 llvm::Value *Result,
                                                 llvm::Value *Code,
                                                 llvm::Value *Length) {
  if (isEVM()) {
    assert(false && "EEI getReturnDataSize not supported in EVM yet");
  } else if (isEWASM()) {
    return Builder.CreateCall(Func_externalCodeCopy,
                              {Builder.CreateIntToPtr(Address, Int32PtrTy),
                               Builder.CreateIntToPtr(Result, Int32PtrTy),
                               Builder.CreateZExtOrTrunc(Code, Int32Ty),
                               Builder.CreateZExtOrTrunc(Length, Int32Ty)});
  } else {
    __builtin_unreachable();
  }
}

void CodeGenModule::emitTrap() {
  llvm::Function *Trap =
      llvm::Intrinsic::getDeclaration(&TheModule, llvm::Intrinsic::trap);

  Builder.CreateCall(Trap, {});
}

} // namespace soll::CodeGen
