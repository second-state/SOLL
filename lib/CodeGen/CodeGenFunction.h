// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "CGValue.h"
#include "CodeGenModule.h"
#include "CodeGenTypeCache.h"
#include "soll/AST/Decl.h"
#include "soll/AST/StmtYul.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ConstantFolder.h>
#include <llvm/IR/IRBuilder.h>

namespace soll::CodeGen {

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

  using DeclMapTy = llvm::DenseMap<const VarDecl *, llvm::Value *>;
  DeclMapTy LocalDeclMap;
  void setAddrOfLocalVar(const VarDecl *VD, llvm::Value *Addr) {
    assert(!LocalDeclMap.count(VD) && "Decl already exists in LocalDeclMap!");
    LocalDeclMap.insert({VD, Addr});
  }
  llvm::Value *getAddrOfLocalVar(const VarDecl *VD) {
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

  void emitYulForStmt(const YulForStmt *S);
  void emitYulCaseStmt(const YulCaseStmt *S);
  void emitYulDefaultStmt(const YulDefaultStmt *S);
  void emitYulSwitchStmt(const YulSwitchStmt *S);

  void emitBranchOnBoolExpr(const Expr *E, llvm::BasicBlock *TrueBlock,
                            llvm::BasicBlock *FalseBlock);
  ExprValue emitExpr(const Expr *E);
  ExprValue emitBoolExpr(const Expr *E);
  void emitCallRequire(const CallExpr *CE);
  void emitCallAssert(const CallExpr *CE);
  void emitCallRevert(const CallExpr *CE);
  void emitCheckPayable(const FunctionDecl *FD);
  llvm::Value *emitAddmod(const CallExpr *CE);
  llvm::Value *emitMulmod(const CallExpr *CE);
  llvm::Value *emitCallkeccak256(const CallExpr *CE);
  llvm::Value *emitCallsha256(const CallExpr *CE);
  llvm::Value *emitCallripemd160(const CallExpr *CE);
  llvm::Value *emitCallecrecover(const CallExpr *CE);
  llvm::Value *emitCallblockhash(const CallExpr *CE);

  ExprValue emitCallExpr(const CallExpr *CE);

  friend class ExprEmitter;
};

} // namespace soll::CodeGen
