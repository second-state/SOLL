// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "CGValue.h"
#include "CodeGenModule.h"
#include "CodeGenTypeCache.h"
#include "soll/AST/Decl.h"
#include "soll/AST/StmtAsm.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ConstantFolder.h>
#include <llvm/IR/IRBuilder.h>

namespace soll::CodeGen {

class CodeGenFunction : public CodeGenTypeCache {
  struct BreakContinue {
    BreakContinue(llvm::BasicBlock *Break, llvm::BasicBlock *Continue)
        : BreakBlock(Break), ContinueBlock(Continue) {}
    llvm::BasicBlock *BreakBlock;
    llvm::BasicBlock *ContinueBlock;
  };
  llvm::SmallVector<BreakContinue, 8> BreakContinueStack;
  CodeGenModule &CGM;
  llvm::IRBuilder<llvm::ConstantFolder> &Builder;
  llvm::BasicBlock *ReturnBlock;
  llvm::Value *ReturnValue;

  using DeclMapTy = llvm::DenseMap<const Decl *, llvm::Value *>;
  DeclMapTy LocalDeclMap;
  void setAddrOfLocalVar(const Decl *VD, llvm::Value *Addr) {
    assert(!LocalDeclMap.count(VD) && "Decl already exists in LocalDeclMap!");
    LocalDeclMap.insert({VD, Addr});
  }
  llvm::Value *getAddrOfLocalVar(const Decl *VD) {
    auto it = LocalDeclMap.find(VD);
    assert(it != LocalDeclMap.end() &&
           "Invalid argument to getAddrOfLocalVar(), no decl!");
    return it->second;
  }
  llvm::BasicBlock *createBasicBlock(llvm::StringRef Name,
                                     llvm::Function *Parent = nullptr) {
    if (Parent == nullptr) {
      Parent = Builder.GetInsertBlock()->getParent();
    }
    return llvm::BasicBlock::Create(getLLVMContext(), Name, Parent);
  }

public:
  CodeGenFunction(const CodeGenFunction &) = delete;
  CodeGenFunction &operator=(const CodeGenFunction &) = delete;

  CodeGenFunction(CodeGenModule &CGM)
      : CodeGenTypeCache(CGM), CGM(CGM), Builder(CGM.getBuilder()) {}

  llvm::IRBuilder<llvm::ConstantFolder> &getBuilder() { return Builder; }
  llvm::LLVMContext &getLLVMContext() const { return CGM.getLLVMContext(); }
  CodeGenModule &getCodeGenModule() const { return CGM; }

  void generateCode(const FunctionDecl *FD, llvm::Function *Fn);
  void generateYulFunction(const AsmFunctionDecl *FD, llvm::Function *Fn);
  void generateYulCode(const YulCode *YC);

private:
  void emitStmt(const Stmt *S);
  void emitBlock(const Block *B);
  void emitDeclStmt(const DeclStmt *DS);
  void emitExprStmt(const ExprStmt *S);
  void emitIfStmt(const IfStmt *S);
  void emitWhileStmt(const WhileStmt *S);
  void emitForStmt(const ForStmt *S);
  void emitContinueStmt(const ContinueStmt *S);
  void emitBreakStmt(const BreakStmt *S);
  void emitReturnStmt(const ReturnStmt *S);
  void emitEmitStmt(const EmitStmt *S);

  void emitAsmForStmt(const AsmForStmt *S);
  void emitAsmSwitchCase(const AsmSwitchCase *S, llvm::SwitchInst *Switch,
                         llvm::BasicBlock *SwitchExit);
  void emitAsmCaseStmt(const AsmCaseStmt *S, llvm::SwitchInst *Switch);
  void emitAsmDefaultStmt(const AsmDefaultStmt *S, llvm::SwitchInst *Switch);
  void emitAsmSwitchStmt(const AsmSwitchStmt *S);
  void emitAsmAssignmentStmt(const AsmAssignmentStmt *AS);
  void emitAsmFunctionDeclStmt(const AsmFunctionDeclStmt *FDS);
  void emitAsmLeaveStmt(const AsmLeaveStmt *LS);

  llvm::Value *emitVarDecl(const Decl *VD);

  void emitBranchOnBoolExpr(const Expr *E, llvm::BasicBlock *TrueBlock,
                            llvm::BasicBlock *FalseBlock);
  ExprValuePtr emitExpr(const Expr *E);
  ExprValuePtr emitBoolExpr(const Expr *E);
  void emitCallRequire(const CallExpr *CE);
  void emitCallAssert(const CallExpr *CE);
  void emitCallRevert(const CallExpr *CE);
  void emitCheckPayable(const FunctionDecl *FD);
  llvm::Value *emitCallAddmod(const CallExpr *CE);
  llvm::Value *emitCallMulmod(const CallExpr *CE);
  llvm::Value *emitCallKeccak256(const CallExpr *CE);
  llvm::Value *emitCallSha256(const CallExpr *CE);
  llvm::Value *emitCallRipemd160(const CallExpr *CE);
  llvm::Value *emitCallEcrecover(const CallExpr *CE);
  llvm::Value *emitCallBlockHash(const CallExpr *CE);
  llvm::Value *emitCallAddressCall(const CallExpr *CE, const MemberExpr *ME);
  llvm::Value *emitCallAddressStaticcall(const CallExpr *CE,
                                         const MemberExpr *ME);
  llvm::Value *emitCallAddressDelegatecall(const CallExpr *CE,
                                           const MemberExpr *ME);
  llvm::Value *emitCallAddressSend(const CallExpr *CE, const MemberExpr *ME,
                                   bool NeedRevert);
  llvm::Value *emitAbiEncodePacked(const CallExpr *CE);
  llvm::Value *emitAbiEncode(const CallExpr *CE);
  llvm::Value *emitStructConstructor(const CallExpr *CE);

  llvm::Value *emitAsmCallDataSize(const CallExpr *CE);
  llvm::Value *emitAsmCallDataOffset(const CallExpr *CE);
  llvm::Value *emitAsmGetBalance(const CallExpr *CE, bool isSelf);
  llvm::Value *emitAsmCallMLoad(const CallExpr *CE);
  void emitAsmCallMStore(const CallExpr *CE);
  void emitAsmCallMStore8(const CallExpr *CE);
  llvm::Value *emitAsmCallMSize(const CallExpr *CE);
  llvm::Value *emitAsmCallSLoad(const CallExpr *CE);
  void emitAsmCallSStore(const CallExpr *CE);
  void emitAsmCallReturn(const CallExpr *CE);
  void emitAsmCallRevert(const CallExpr *CE);
  void emitAsmCallLog(const CallExpr *CE);
  llvm::Value *emitAsmCallCallDataLoad(const CallExpr *CE);
  void emitAsmCallCodeCopy(const CallExpr *CE);
  void emitAsmExternalCallCodeCopy(const CallExpr *CE);
  void emitAsmCallDataCopy(const CallExpr *CE);
  llvm::Value *emitAsmExternalGetCodeSize(const CallExpr *CE);
  llvm::Value *emitAsmExternalCallCodeHash(const CallExpr *CE);
  llvm::Value *emitAsmCallkeccak256(const CallExpr *CE);
  llvm::Value *emitAsmGetBlockHash(const CallExpr *CE);
  void emitAsmReturnDataCopy(const CallExpr *CE);
  llvm::Value *emitAsmCall(const CallExpr *CE);
  llvm::Value *emitAsmCallCode(const CallExpr *CE);
  llvm::Value *emitAsmDelegatecall(const CallExpr *CE);
  llvm::Value *emitAsmCallStaticcall(const CallExpr *CE);
  llvm::Value *emitAsmCreate(const CallExpr *CE);
  llvm::Value *emitAsmCreate2(const CallExpr *CE);
  llvm::Value *emitAsmByte(const CallExpr *CE);
  void emitAsmSelfDestruct(const CallExpr *CE);
  llvm::Value *emitAsmChainId(const CallExpr *CE);


  ExprValuePtr emitCallExpr(const CallExpr *CE);
  ExprValuePtr emitSpecialCallExpr(const Identifier *SI, const CallExpr *CE,
                                   const MemberExpr *ME);
  ExprValuePtr emitAsmSpecialCallExpr(const AsmIdentifier *SI,
                                      const CallExpr *CE);

  friend class ExprEmitter;
};

} // namespace soll::CodeGen
