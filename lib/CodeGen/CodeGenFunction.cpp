// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "soll/Basic/DiagnosticSema.h"

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

void CodeGenFunction::generateYulFunction(const AsmFunctionDecl *FD,
                                          llvm::Function *Fn) {
  llvm::BasicBlock *EntryBB = createBasicBlock("entry", Fn);
  ReturnBlock = createBasicBlock("return", Fn);
  Builder.SetInsertPoint(EntryBB);

  llvm::Argument *PsLLVM = Fn->arg_begin();
  for (const auto *VD : FD->getParams()->getParams()) {
    llvm::Argument *Arg = PsLLVM++;
    Arg->setName(VD->getName());
    llvm::Value *ArgAddr =
        Builder.CreateAlloca(Arg->getType(), nullptr, Arg->getName() + ".addr");
    Builder.CreateStore(Arg, ArgAddr);
    setAddrOfLocalVar(VD, ArgAddr);
  }
  llvm::SmallVector<llvm::Value *, 8> ReturnValues;
  if (llvm::Type *ReturnType = Fn->getReturnType(); !ReturnType->isVoidTy()) {
    const auto ReturnParams = FD->getReturnParams()->getParams();
    if (!ReturnType->isStructTy()) {
      const auto *VD = ReturnParams.front();
      ReturnValue =
          Builder.CreateAlloca(ReturnType, nullptr, VD->getName() + ".addr");
      Builder.CreateStore(llvm::ConstantInt::get(ReturnType, 0), ReturnValue);
      setAddrOfLocalVar(VD, ReturnValue);
    } else {
      unsigned int Idx = 0;
      for (const auto *VD : ReturnParams) {
        auto *Ty = ReturnType->getStructElementType(Idx++);
        ReturnValue =
            Builder.CreateAlloca(Ty, nullptr, VD->getName() + ".addr");
        Builder.CreateStore(llvm::ConstantInt::get(Ty, 0), ReturnValue);
        setAddrOfLocalVar(VD, ReturnValue);
        ReturnValues.push_back(ReturnValue);
      }
    }
  } else {
    ReturnValue = nullptr;
  }

  emitStmt(FD->getBody());

  {
    llvm::BasicBlock *Tail = Builder.GetInsertBlock();
    if (!Tail->getTerminator()) {
      Builder.CreateBr(ReturnBlock);
    }
  }

  Builder.SetInsertPoint(ReturnBlock);

  if (llvm::Type *ReturnType = Fn->getReturnType(); !ReturnType->isVoidTy()) {
    llvm::Value *RetVal;
    if (!ReturnType->isStructTy()) {
      RetVal = Builder.CreateLoad(ReturnType, ReturnValue);
    } else {
      RetVal = llvm::ConstantAggregateZero::get(ReturnType);
      unsigned int Idx = 0;
      for (auto *V : ReturnValues) {
        auto *Ty = ReturnType->getStructElementType(Idx);
        RetVal = Builder.CreateInsertValue(RetVal, Builder.CreateLoad(Ty, V),
                                           {Idx++});
      }
    }
    Builder.CreateRet(RetVal);
  } else {
    Builder.CreateRetVoid();
  }
}

void CodeGenFunction::generateYulCode(const YulCode *YC) {
  emitBlock(YC->getBody());
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
  if (auto FS = dynamic_cast<const AsmForStmt *>(S)) {
    return emitAsmForStmt(FS);
  }
  if (auto SS = dynamic_cast<const AsmSwitchStmt *>(S)) {
    return emitAsmSwitchStmt(SS);
  }
  if (auto AS = dynamic_cast<const AsmAssignmentStmt *>(S)) {
    return emitAsmAssignmentStmt(AS);
  }
  if (auto FDS = dynamic_cast<const AsmFunctionDeclStmt *>(S)) {
    return emitAsmFunctionDeclStmt(FDS);
  }
  if (auto LS = dynamic_cast<const AsmLeaveStmt *>(S)) {
    return emitAsmLeaveStmt(LS);
  }
}

void CodeGenFunction::emitDeclStmt(const DeclStmt *DS) {
  llvm::SmallVector<llvm::Value *, 8> Addrs;
  for (const auto *VD : DS->getVarDecls()) {
    Addrs.push_back(emitVarDecl(VD));
  }
  if (DS->getValue()) {
    llvm::Value *Val = emitExpr(DS->getValue())->load(Builder, CGM);
    if (Addrs.size() == 1) {
      Builder.CreateStore(Val, Addrs.front());
    } else {
      auto Ty = Val->getType();
      assert(Ty->isStructTy() && "expect multiple values on RHS");
      assert(static_cast<llvm::StructType *>(Ty)->getNumElements() ==
                 Addrs.size() &&
             "the number of values does not match the number of declarations");

      unsigned int Idx = 0;
      for (auto *Addr : Addrs) {
        Builder.CreateStore(Builder.CreateExtractValue(Val, Idx++), Addr);
      }
    }
  }
}

llvm::Value *CodeGenFunction::emitVarDecl(const Decl *VD) {
  TypePtr Ty;
  if (auto D = dynamic_cast<const VarDecl *>(VD))
    Ty = D->getType();
  else if (auto D = dynamic_cast<const AsmVarDecl *>(VD))
    Ty = D->getType();
  auto *LLVMTy = CGM.getLLVMType(Ty.get());

  llvm::Value *Addr =
      Builder.CreateAlloca(LLVMTy, nullptr, VD->getName() + ".addr");
  setAddrOfLocalVar(VD, Addr);
  switch (Ty->getCategory()) {
  case Type::Category::Bool:
  case Type::Category::Integer:
  case Type::Category::Address:
    Builder.CreateStore(llvm::ConstantInt::get(LLVMTy, 0), Addr);
    break;
  case Type::Category::String:
  case Type::Category::Bytes:
  case Type::Category::Array:
  case Type::Category::Struct:
    Builder.CreateStore(llvm::ConstantAggregateZero::get(LLVMTy), Addr);
    break;
  default:
    assert(false && "unknown type");
    __builtin_unreachable();
  }
  return Addr;
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
  Builder.CreateBr(ContBlock);

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

  llvm::BasicBlock *ForCond = createBasicBlock("for.cond");
  llvm::BasicBlock *ForBody = createBasicBlock("for.body");
  llvm::BasicBlock *Continue = createBasicBlock("for.cont");
  llvm::BasicBlock *LoopExit = createBasicBlock("for.end");

  Builder.CreateBr(ForCond);
  Builder.SetInsertPoint(ForCond);
  if (const Expr *Cond = FS->getCond()) {
    emitBranchOnBoolExpr(Cond, ForBody, LoopExit);
  } else {
    Builder.CreateBr(ForBody);
  }

  BreakContinueStack.push_back(BreakContinue(LoopExit, Continue));

  Builder.SetInsertPoint(ForBody);
  emitStmt(FS->getBody());
  Builder.CreateBr(Continue);

  Builder.SetInsertPoint(Continue);
  if (const Expr *Loop = FS->getLoop()) {
    emitExpr(Loop);
  }
  Builder.CreateBr(ForCond);

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
    Builder.CreateStore(emitExpr(E)->load(Builder, CGM), ReturnValue);
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

void CodeGenFunction::emitAsmForStmt(const AsmForStmt *FS) {
  llvm::BasicBlock *ForCond = createBasicBlock("for.cond");
  llvm::BasicBlock *ForBody = createBasicBlock("for.body");
  llvm::BasicBlock *Continue = createBasicBlock("for.cont");
  llvm::BasicBlock *LoopExit = createBasicBlock("for.end");

  emitBlock(FS->getInit());
  Builder.CreateBr(ForCond);

  Builder.SetInsertPoint(ForCond);
  emitBranchOnBoolExpr(FS->getCond(), ForBody, LoopExit);

  BreakContinueStack.push_back(BreakContinue(LoopExit, Continue));

  Builder.SetInsertPoint(ForBody);
  emitBlock(FS->getBody());
  Builder.CreateBr(Continue);

  Builder.SetInsertPoint(Continue);
  emitBlock(FS->getLoop());
  Builder.CreateBr(ForCond);

  BreakContinueStack.pop_back();
  Builder.SetInsertPoint(LoopExit);
}

void CodeGenFunction::emitAsmSwitchCase(const AsmSwitchCase *SC,
                                        llvm::SwitchInst *Switch,
                                        llvm::BasicBlock *SwitchExit) {
  if (auto CS = dynamic_cast<const AsmCaseStmt *>(SC)) {
    emitAsmCaseStmt(CS, Switch);
  } else if (auto DS = dynamic_cast<const AsmDefaultStmt *>(SC)) {
    emitAsmDefaultStmt(DS, Switch);
  } else { ///< Got something not a "case" nor a "default".
    __builtin_unreachable();
  }
  emitStmt(SC->getSubStmt());
  Builder.CreateBr(SwitchExit);
}

void CodeGenFunction::emitAsmCaseStmt(const AsmCaseStmt *CS,
                                      llvm::SwitchInst *Switch) {
  // TODO:
  // - handle non-integer case-value.
  llvm::BasicBlock *CaseBB = createBasicBlock("switch.case");
  llvm::Value *CaseV = emitExpr(CS->getLHS())->load(Builder, CGM);
  if (auto ConstV = llvm::dyn_cast<llvm::ConstantInt>(CaseV)) {
    Switch->addCase(ConstV, CaseBB);
  } else { ///< wrong code
    __builtin_unreachable();
  }
  Builder.SetInsertPoint(CaseBB);
}

void CodeGenFunction::emitAsmDefaultStmt(const AsmDefaultStmt *DS,
                                         llvm::SwitchInst *Switch) {
  llvm::BasicBlock *DefaultBB = Switch->getDefaultDest();
  Builder.SetInsertPoint(DefaultBB);
}

void CodeGenFunction::emitAsmSwitchStmt(const AsmSwitchStmt *SS) {
  // TODO:
  // - handle nested switch statements.
  // - handle large case range.
  llvm::Value *CondV = emitExpr(SS->getCond())->load(Builder, CGM);
  llvm::Value *CondVconv = Builder.CreateIntCast(CondV, llvm::Type::getIntNTy(getLLVMContext(), 256), true);
  llvm::BasicBlock *DefaultBB = createBasicBlock("switch.default");
  llvm::BasicBlock *SwitchExit = createBasicBlock("switch.end");
  llvm::SwitchInst *Switch = Builder.CreateSwitch(CondVconv, DefaultBB);

  for (auto *Case : SS->getCases()) {
    emitAsmSwitchCase(Case, Switch, SwitchExit);
  }

  if (DefaultBB->empty()) { ///< default block was never emitted
    Builder.SetInsertPoint(DefaultBB);
    Builder.CreateBr(SwitchExit);
  }
  Builder.SetInsertPoint(SwitchExit);
}

void CodeGenFunction::emitAsmAssignmentStmt(const AsmAssignmentStmt *AS) {
  llvm::SmallVector<ExprValuePtr, 8> LHSs;
  for (const auto *AI : AS->getLHS()->getIdentifiers()) {
    LHSs.push_back(emitExpr(AI));
  }
  llvm::Value *RHSV = emitExpr(AS->getRHS())->load(Builder, CGM);
  if (LHSs.size() == 1) {
    LHSs.front()->store(Builder, CGM, RHSV);
  } else {
    const auto RTy = RHSV->getType();
    assert(RTy->isStructTy() && "expect mutiple values on RHS");
    assert(static_cast<llvm::StructType *>(RTy)->getNumElements() ==
               LHSs.size() &&
           "the number of values does not match the number of identifiers");

    unsigned int Idx = 0;
    for (auto &LHS : LHSs) {
      LHS->store(Builder, CGM, Builder.CreateExtractValue(RHSV, Idx++));
    }
  }
}

void CodeGenFunction::emitAsmFunctionDeclStmt(const AsmFunctionDeclStmt *FDS) {
  const AsmFunctionDecl *FD = FDS->getDecl();
  llvm::Function *Fn = CGM.createOrGetLLVMFunction(FD);
  const auto OldBlock = Builder.GetInsertBlock();
  const auto OldPoint = Builder.GetInsertPoint();
  CodeGenFunction(CGM).generateYulFunction(FD, Fn);
  Builder.SetInsertPoint(OldBlock, OldPoint);
}

void CodeGenFunction::emitAsmLeaveStmt(const AsmLeaveStmt *LS) {
  llvm::BasicBlock *LeaveReturn = createBasicBlock("leave.after");
  Builder.CreateBr(ReturnBlock);
  Builder.SetInsertPoint(LeaveReturn);
}

ExprValuePtr CodeGenFunction::emitBoolExpr(const Expr *E) {
  llvm::Value *Condition = emitExpr(E)->load(Builder, CGM);
  return ExprValue::getRValue(E, Condition);
}

void CodeGenFunction::emitBranchOnBoolExpr(const Expr *E,
                                           llvm::BasicBlock *TrueBlock,
                                           llvm::BasicBlock *FalseBlock) {
  Builder.CreateCondBr(emitBoolExpr(E)->load(Builder, CGM), TrueBlock,
                       FalseBlock);
}

void CodeGenFunction::emitCheckPayable(const FunctionDecl *FD) {
  if (StateMutability::Payable != FD->getStateMutability()) {
    llvm::BasicBlock *Continue = createBasicBlock("continue");
    llvm::BasicBlock *Revert = createBasicBlock("revert");

    int N = 256;
    if (CGM.isEVM()) {
      N = 256;
    } else if (CGM.isEWASM()) {
      N = 128;
    } else {
      __builtin_unreachable();
    }
    llvm::Value *Cond =
        Builder.CreateICmpNE(CGM.emitGetCallValue(), Builder.getIntN(N, 0));
    Builder.CreateCondBr(Cond, Revert, Continue);

    using namespace std::string_literals;
    static const std::string Message = "Function is not payable"s;
    llvm::Value *MessageValue =
        createGlobalStringPtr(getLLVMContext(), CGM.getModule(), Message);
    Builder.SetInsertPoint(Revert);
    CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(Continue);
  }
}

} // namespace soll::CodeGen
