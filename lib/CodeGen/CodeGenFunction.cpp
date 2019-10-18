// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenFunction.h"
#include "CodeGenModule.h"

namespace soll::CodeGen {

void CodeGenFunction::generateCode(const FunctionDecl *FD, llvm::Function *Fn) {
  llvm::BasicBlock *EntryBB = createBasicBlock("entry", Fn);
  ReturnBlock = createBasicBlock("return", Fn);
  Builder.SetInsertPoint(EntryBB);
  emitCheckPayable(FD);
  llvm::Argument *PsLLVM = Fn->arg_begin();
  for (auto *VD : FD->getParams()->getParams()) {
    llvm::Argument *Arg = PsLLVM++;
    Arg->setName(VD->getName());
    llvm::Value *ArgAddr =
        Builder.CreateAlloca(Arg->getType(), nullptr, Arg->getName() + ".addr");
    Builder.CreateStore(Arg, ArgAddr);
    setAddrOfLocalVar(VD, ArgAddr);
  }
  if (llvm::Type *ReturnType = Fn->getReturnType(); !ReturnType->isVoidTy()) {
    ReturnValue = Builder.CreateAlloca(ReturnType, nullptr, "retval");
  } else {
    ReturnValue = nullptr;
  }

  emitBlock(FD->getBody());
  {
    llvm::BasicBlock *Tail = Builder.GetInsertBlock();
    if (!Tail->getTerminator()) {
      Builder.CreateBr(ReturnBlock);
    }
  }

  Builder.SetInsertPoint(ReturnBlock);

  if (llvm::Type *ReturnType = Fn->getReturnType(); !ReturnType->isVoidTy()) {
    llvm::Value *RetVal = Builder.CreateLoad(ReturnType, ReturnValue);
    Builder.CreateRet(RetVal);
  } else {
    Builder.CreateRetVoid();
  }
}

void CodeGenFunction::emitStmt(const Stmt *S) {
  if (auto DS = dynamic_cast<const DeclStmt *>(S)) {
    return emitDeclStmt(DS);
  }
  if (auto ES = dynamic_cast<const ExprStmt *>(S)) {
    return emitExprStmt(ES);
  }
  if (auto B = dynamic_cast<const Block *>(S)) {
    return emitBlock(B);
  }
  if (auto IS = dynamic_cast<const IfStmt *>(S)) {
    return emitIfStmt(IS);
  }
  if (auto WS = dynamic_cast<const WhileStmt *>(S)) {
    return emitWhileStmt(WS);
  }
  if (auto FS = dynamic_cast<const ForStmt *>(S)) {
    return emitForStmt(FS);
  }
  if (auto CS = dynamic_cast<const ContinueStmt *>(S)) {
    return emitContinueStmt(CS);
  }
  if (auto BS = dynamic_cast<const BreakStmt *>(S)) {
    return emitBreakStmt(BS);
  }
  if (auto RS = dynamic_cast<const ReturnStmt *>(S)) {
    return emitReturnStmt(RS);
  }
  if (auto ES = dynamic_cast<const EmitStmt *>(S)) {
    return emitEmitStmt(ES);
  }
}

void CodeGenFunction::emitDeclStmt(const DeclStmt *DS) {
  assert(DS->getVarDecls().size() == 1 && "unsupported tuple decoupling!");
  auto *VD = DS->getVarDecls().front();
  auto *Ty = VD->GetType().get();
  auto *LLVMTy = CGM.getLLVMType(Ty);

  llvm::Value *Addr =
      Builder.CreateAlloca(LLVMTy, nullptr, VD->getName() + ".addr");
  setAddrOfLocalVar(VD, Addr);
  if (DS->getValue()) {
    Builder.CreateStore(emitExpr(DS->getValue()).load(Builder, CGM), Addr);
  } else {
    switch (Ty->getCategory()) {
      case Type::Category::Bool:
      case Type::Category::Integer:
      case Type::Category::Address:
        Builder.CreateStore(llvm::ConstantInt::get(LLVMTy, 0), Addr);
        break;
      case Type::Category::String:
      case Type::Category::Bytes:
        Builder.CreateStore(llvm::ConstantAggregateZero::get(LLVMTy), Addr);
        break;
      case Type::Category::Array:
        assert(false && "zero-init array not implement");
        __builtin_unreachable();
        break;
      default:
        assert(false && "unknown type");
        __builtin_unreachable();
    }
  }
}

void CodeGenFunction::emitExprStmt(const ExprStmt *ES) {
  emitExpr(dynamic_cast<const Expr *>(ES));
}

void CodeGenFunction::emitBlock(const Block *B) {
  for (const Stmt *S : B->getStmts()) {
    emitStmt(S);
  }
}

void CodeGenFunction::emitIfStmt(const IfStmt *IS) {
  const bool ElseExist = (IS->getElse() != nullptr);
  llvm::BasicBlock *ThenBlock = createBasicBlock("if.then");
  llvm::BasicBlock *ContBlock = createBasicBlock("if.end");
  llvm::BasicBlock *ElseBlock =
      ElseExist ? createBasicBlock("if.else") : ContBlock;

  emitBranchOnBoolExpr(IS->getCond(), ThenBlock, ElseBlock);

  Builder.SetInsertPoint(ThenBlock);
  emitStmt(IS->getThen());
  Builder.CreateBr(ElseBlock);

  if (ElseExist) {
    Builder.SetInsertPoint(ElseBlock);
    emitStmt(IS->getElse());
    Builder.CreateBr(ContBlock);
  }

  Builder.SetInsertPoint(ContBlock);
}

void CodeGenFunction::emitWhileStmt(const WhileStmt *WS) {
  llvm::BasicBlock *LoopHeader = createBasicBlock("while.cond");
  llvm::BasicBlock *LoopExit = createBasicBlock("while.end");
  llvm::BasicBlock *LoopBody = createBasicBlock("while.body");

  BreakContinueStack.push_back(BreakContinue(LoopExit, LoopHeader));

  if (WS->isDoWhile()) {
    Builder.CreateBr(LoopBody);
  } else {
    Builder.CreateBr(LoopHeader);
  }

  Builder.SetInsertPoint(LoopHeader);
  emitBranchOnBoolExpr(WS->getCond(), LoopBody, LoopExit);

  Builder.SetInsertPoint(LoopBody);
  emitStmt(WS->getBody());
  Builder.CreateBr(LoopHeader);

  BreakContinueStack.pop_back();
  Builder.SetInsertPoint(LoopExit);
}

void CodeGenFunction::emitForStmt(const ForStmt *FS) {
  if (const Stmt *Init = FS->getInit()) {
    emitStmt(Init);
  }

  llvm::BasicBlock *LoopExit = createBasicBlock("for.end");
  llvm::BasicBlock *Continue = createBasicBlock("for.cond");
  llvm::BasicBlock *ForBody = createBasicBlock("for.body");

  Builder.SetInsertPoint(Continue);
  if (const Expr *Cond = FS->getCond()) {
    emitBranchOnBoolExpr(Cond, ForBody, LoopExit);
  } else {
    Builder.CreateBr(ForBody);
  }

  BreakContinueStack.push_back(BreakContinue(LoopExit, Continue));

  Builder.SetInsertPoint(ForBody);
  emitStmt(FS->getBody());
  Builder.CreateBr(Continue);

  BreakContinueStack.pop_back();
  Builder.SetInsertPoint(LoopExit);
}

void CodeGenFunction::emitContinueStmt(const ContinueStmt *CS) {
  assert(!BreakContinueStack.empty() && "continue stmt not in a loop!");
  llvm::BasicBlock *AfterContinue = createBasicBlock("continue.after");
  Builder.CreateBr(BreakContinueStack.back().ContinueBlock);
  Builder.SetInsertPoint(AfterContinue);
}

void CodeGenFunction::emitBreakStmt(const BreakStmt *BS) {
  assert(!BreakContinueStack.empty() && "break stmt not in a loop!");
  llvm::BasicBlock *AfterBreak = createBasicBlock("break.after");
  Builder.CreateBr(BreakContinueStack.back().BreakBlock);
  Builder.SetInsertPoint(AfterBreak);
}

void CodeGenFunction::emitReturnStmt(const ReturnStmt *RS) {
  if (const Expr *E = RS->getRetValue()) {
    assert(ReturnValue != nullptr && "return on void function?");
    Builder.CreateStore(emitExpr(E).load(Builder, CGM), ReturnValue);
  } else {
    assert(ReturnValue == nullptr && "return nothing on non-void function?");
  }
  llvm::BasicBlock *AfterReturn = createBasicBlock("return.after");
  Builder.CreateBr(ReturnBlock);
  Builder.SetInsertPoint(AfterReturn);
}

void CodeGenFunction::emitEmitStmt(const EmitStmt *ES) {
  emitCallExpr(ES->getCall());
}

ExprValue CodeGenFunction::emitBoolExpr(const Expr *E) {
  llvm::Value *Condition = emitExpr(E).load(Builder, CGM);
  if (llvm::Type *Ty = Condition->getType(); !Ty->isIntegerTy(1)) {
    Condition = Builder.CreateICmpNE(Condition, llvm::ConstantInt::get(Ty, 0));
  }
  return ExprValue::getRValue(E, Condition);
}

void CodeGenFunction::emitBranchOnBoolExpr(const Expr *E,
                                           llvm::BasicBlock *TrueBlock,
                                           llvm::BasicBlock *FalseBlock) {
  Builder.CreateCondBr(emitBoolExpr(E).load(Builder, CGM), TrueBlock,
                       FalseBlock);
}

void CodeGenFunction::emitCheckPayable(const FunctionDecl *FD) {
  if (StateMutability::Payable != FD->getStateMutability()) {
    llvm::Function *getCallValue =
        CGM.getModule().getFunction("ethereum.getCallValue");
    llvm::Value *ValPtr = Builder.CreateAlloca(Int128Ty);
    Builder.CreateCall(getCallValue, {ValPtr});
    llvm::BasicBlock *Continue = createBasicBlock("continue");
    llvm::BasicBlock *Revert = createBasicBlock("revert");

    llvm::Value *Cond = Builder.CreateICmpNE(Builder.CreateLoad(ValPtr),
                                             Builder.getIntN(128, 0));
    Builder.CreateCondBr(Cond, Revert, Continue);

    using namespace std::string_literals;
    static const std::string Message = "Function is not payable"s;
    llvm::Value *MessageValue =
        createGlobalStringPtr(getLLVMContext(), CGM.getModule(), Message);
    Builder.SetInsertPoint(Revert);
    Builder.CreateCall(CGM.getModule().getFunction("ethereum.revert"),
                       {MessageValue, Builder.getInt32(Message.size())});
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(Continue);
  }
}

} // namespace soll::CodeGen
