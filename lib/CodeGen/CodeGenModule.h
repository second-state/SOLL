// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "CodeGenTypeCache.h"
#include "soll/AST/Decl.h"
#include "soll/AST/DeclAsm.h"
#include "soll/AST/DeclYul.h"
#include "soll/Basic/TargetOptions.h"
#include <llvm/ADT/APInt.h>
#include <llvm/IR/ConstantFolder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace soll {

class ASTContext;
class DiagnosticsEngine;

inline llvm::GlobalVariable *createGlobalString(llvm::LLVMContext &Context,
                                                llvm::Module &Module,
                                                llvm::StringRef Str,
                                                const llvm::Twine &Name = "") {
  llvm::Constant *StrConstant =
      llvm::ConstantDataArray::getString(Context, Str, false);
  auto *GV = new llvm::GlobalVariable(Module, StrConstant->getType(), true,
                                      llvm::GlobalValue::PrivateLinkage,
                                      StrConstant, Name);
  GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
  GV->setAlignment(1);
  return GV;
}
inline llvm::Constant *createGlobalStringPtr(llvm::LLVMContext &Context,
                                             llvm::Module &Module,
                                             llvm::StringRef Str,
                                             const llvm::Twine &Name = "") {
  llvm::GlobalVariable *GV = createGlobalString(Context, Module, Str, Name);
  llvm::Constant *Zero =
      llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0);
  llvm::Constant *Indices[] = {Zero, Zero};
  return llvm::ConstantExpr::getInBoundsGetElementPtr(GV->getValueType(), GV,
                                                      Indices);
}

namespace CodeGen {

class CodeGenModule : public CodeGenTypeCache {
  ASTContext &Context;
  llvm::Module &TheModule;
  DiagnosticsEngine &Diags;
  const TargetOptions &TargetOpts;
  llvm::LLVMContext &VMContext;
  llvm::IRBuilder<llvm::ConstantFolder> Builder;
  llvm::GlobalVariable *MemorySize;
  llvm::GlobalVariable *HeapBase;
  llvm::DenseMap<const VarDecl *, llvm::GlobalVariable *> StateVarDeclMap;
  llvm::DenseMap<const YulData *, llvm::Constant *> YulDataMap;
  std::size_t StateVarAddrCursor;

  llvm::Function *Func_call = nullptr;
  llvm::Function *Func_callDataCopy = nullptr;
  llvm::Function *Func_callStatic = nullptr;
  llvm::Function *Func_callDelegate = nullptr;
  llvm::Function *Func_finish = nullptr;
  llvm::Function *Func_getCallDataSize = nullptr;
  llvm::Function *Func_getCallValue = nullptr;
  llvm::Function *Func_getCaller = nullptr;
  llvm::Function *Func_getGasLeft = nullptr;
  llvm::Function *Func_log = nullptr;
  llvm::Function *Func_log0 = nullptr;
  llvm::Function *Func_log1 = nullptr;
  llvm::Function *Func_log2 = nullptr;
  llvm::Function *Func_log3 = nullptr;
  llvm::Function *Func_log4 = nullptr;
  llvm::Function *Func_returnDataSize = nullptr;
  llvm::Function *Func_returnDataCopy = nullptr;
  llvm::Function *Func_revert = nullptr;
  llvm::Function *Func_storageLoad = nullptr;
  llvm::Function *Func_storageStore = nullptr;
  llvm::Function *Func_getTxGasPrice = nullptr;
  llvm::Function *Func_getTxOrigin = nullptr;
  llvm::Function *Func_getBlockCoinbase = nullptr;
  llvm::Function *Func_getBlockDifficulty = nullptr;
  llvm::Function *Func_getBlockGasLimit = nullptr;
  llvm::Function *Func_getBlockNumber = nullptr;
  llvm::Function *Func_getBlockTimestamp = nullptr;
  llvm::Function *Func_getBlockHash = nullptr;
  llvm::Function *Func_getExternalBalance = nullptr;
  llvm::Function *Func_getAddress = nullptr;

  llvm::Function *Func_print32 = nullptr;

  llvm::Function *Func_keccak256 = nullptr;
  llvm::Function *Func_sha256 = nullptr;
  llvm::Function *Func_sha3 = nullptr;
  llvm::Function *Func_ripemd160 = nullptr;
  llvm::Function *Func_ecrecover = nullptr;

  llvm::Function *Func_exp256 = nullptr;
  llvm::Function *Func_bswap256 = nullptr;
  llvm::Function *Func_memcpy = nullptr;
  llvm::Function *Func_updateMemorySize = nullptr;

  void initTypes();
  void initMemorySection();
  void initUpdateMemorySize();

  void initEVMOpcodeDeclaration();
  void initEEIDeclaration();

  void initHelperDeclaration();
  void initExpI256();
  void initBswapI256();
  void initMemcpy();

  void initPrebuiltContract();
  void initKeccak256();
  void initSha256();
  void initRipemd160();
  void initEcrecover();

public:
  CodeGenModule(const CodeGenModule &) = delete;
  void operator=(const CodeGenModule &) = delete;

  CodeGenModule(ASTContext &C, llvm::Module &module, DiagnosticsEngine &Diags,
                const TargetOptions &TargetOpts);
  llvm::Function *getIntrinsic(unsigned IID,
                               llvm::ArrayRef<llvm::Type *> Typs = llvm::None);
  llvm::Module &getModule() const { return TheModule; }
  llvm::LLVMContext &getLLVMContext() const { return VMContext; }
  llvm::IRBuilder<llvm::ConstantFolder> &getBuilder() { return Builder; }
  DiagnosticsEngine &getDiags() { return Diags; }

  bool isEVM() const noexcept { return TargetOpts.BackendTarget == EVM; }
  bool isEWASM() const noexcept { return TargetOpts.BackendTarget == EWASM; }

  void emitContractDecl(const ContractDecl *CD);
  void emitYulObject(const YulObject *YO);
  llvm::Value *emitExp(llvm::Value *Base, llvm::Value *Exp, bool IsSigned);
  llvm::Value *emitEndianConvert(llvm::Value *Val);
  llvm::Value *getEndianlessValue(llvm::Value *Val);
  bool isBytesType(llvm::Type *Ty);
  llvm::Value *emitConcateBytes(llvm::ArrayRef<llvm::Value *> Values);
  void emitUpdateMemorySize(llvm::Value *Pos, llvm::Value *Range);

  llvm::Value *emitGetGasLeft();
  llvm::Value *emitGetCallValue();
  llvm::Value *emitGetCaller();
  void emitFinish(llvm::Value *DataOffset, llvm::Value *Length);
  void emitLog(llvm::Value *DataOffset, llvm::Value *DataLength,
               std::vector<llvm::Value *> &Topics);
  void emitRevert(llvm::Value *DataOffset, llvm::Value *Length);
  void emitCallDataCopy(llvm::Value *ResultOffset, llvm::Value *DataOffset,
                        llvm::Value *Length);
  llvm::Value *emitStorageLoad(llvm::Value *Key);
  void emitStorageStore(llvm::Value *Key, llvm::Value *Value);

  llvm::Value *emitReturnDataSize();
  llvm::Value *emitReturnDataCopyBytes(llvm::Value *DataOffset,
                                       llvm::Value *Length);
  llvm::Value *emitCallDataLoad(llvm::Value *DataOffset);
  llvm::Value *emitCall(llvm::Value *Gas, llvm::Value *AddressPtr,
                        llvm::Value *ValuePtr, llvm::Value *DataPtr,
                        llvm::Value *DataLength,
                        llvm::Value *RetOffset = nullptr,
                        llvm::Value *RetLength = nullptr);
  llvm::Value *emitCallStatic(llvm::Value *Gas, llvm::Value *AddressPtr,
                              llvm::Value *DataPtr, llvm::Value *DataLength,
                              llvm::Value *RetOffset = nullptr,
                              llvm::Value *RetLength = nullptr);
  llvm::Value *emitCallDelegate(llvm::Value *Gas, llvm::Value *AddressPtr,
                                llvm::Value *DataPtr, llvm::Value *DataLength);
  llvm::Value *emitKeccak256(llvm::Value *Bytes);
  llvm::Value *emitSha256(llvm::Value *Bytes);
  llvm::Value *emitGetCallDataSize();
  llvm::Value *emitGetTxGasPrice();
  llvm::Value *emitGetTxOrigin();
  llvm::Value *emitGetBlockCoinbase();
  llvm::Value *emitGetBlockDifficulty();
  llvm::Value *emitGetBlockGasLimit();
  llvm::Value *emitGetBlockNumber();
  llvm::Value *emitGetBlockTimestamp();
  llvm::Value *emitGetBlockHash(llvm::Value *Number);
  llvm::Value *emitGetExternalBalance(llvm::Value *AddressOffset);
  llvm::Value *emitGetAddress();

private:
  void emitContractConstructorDecl(const ContractDecl *CD);
  void emitContractDispatcherDecl(const ContractDecl *CD);
  void emitEventDecl(const EventDecl *ED);
  void emitFunctionDecl(const FunctionDecl *FD);
  void emitVarDecl(const VarDecl *VD);

  void emitYulCode(const YulCode *YC);
  void emitYulData(const YulData *YD);
  void emitAsmVarDecl(const AsmVarDecl *VD);

  void emitABILoad(const FunctionDecl *FD, llvm::BasicBlock *Loader,
                   llvm::BasicBlock *Error, llvm::Value *CallDataSize);
  llvm::Value *emitABILoadParamStatic(const Type *Ty, llvm::StringRef Name,
                                      llvm::Value *Buffer,
                                      std::uint32_t Offset);
  llvm::Value *emitABILoadParamDynamic(const Type *Ty, llvm::Value *Size,
                                       llvm::StringRef Name,
                                       llvm::Value *Buffer,
                                       llvm::Value *Offset);
  void emitABIStore(const Type *Ty, llvm::StringRef Name, llvm::Value *Result);

public:
  std::string getMangledName(const CallableVarDecl *CVD);
  llvm::Type *getLLVMType(const Type *Ty);
  llvm::Type *getStaticLLVMType(const Type *Ty);
  llvm::FunctionType *getFunctionType(const CallableVarDecl *CVD);
  llvm::Function *createOrGetLLVMFunction(const CallableVarDecl *CVD);
  llvm::GlobalVariable *getMemorySize() { return MemorySize; }
  llvm::GlobalVariable *getHeapBase() { return HeapBase; }
  llvm::GlobalVariable *getStateVarAddr(const VarDecl *VD) {
    return StateVarDeclMap.lookup(VD);
  }
};

} // namespace CodeGen
} // namespace soll
