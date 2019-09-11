// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

#include <cassert>
#include <iostream>

namespace soll {

using llvm::BasicBlock;
using llvm::Function;
using llvm::Value;

FuncBodyCodeGen::FuncBodyCodeGen(llvm::LLVMContext &Context,
                                 llvm::IRBuilder<llvm::NoFolder> &Builder,
                                 llvm::Module &Module, ASTContext &Ctx)
    : Context(Context), Builder(Builder), Module(Module), ASTCtx(Ctx) {
  Int256Ty = Builder.getIntNTy(256);
  VoidTy = Builder.getVoidTy();
  AddressTy = Builder.getIntNTy(160);
  StringTy = Module.getTypeByName("string");
  BytesTy = Module.getTypeByName("bytes");
  Zero256 = Builder.getIntN(256, 0);
  One256 = Builder.getIntN(256, 1);
  False = Builder.getFalse();
  True = Builder.getTrue();
}

void FuncBodyCodeGen::compile(const FunctionDecl &FD) {
  CurFunc = Module.getFunction(FD.getName());
  assert(CurFunc != nullptr);

  BasicBlock *BB = BasicBlock::Create(Context, "entry", CurFunc);
  Builder.SetInsertPoint(BB);

  auto PsLLVM = CurFunc->arg_begin();
  auto PsSol = FD.getParams()->getParams();
  for (auto *VD : PsSol) {
    llvm::Value *P = PsLLVM++;
    P->setName(VD->getName());
    llvm::Value *paramAddr =
        Builder.CreateAlloca(getLLVMTy(VD), nullptr, P->getName() + ".addr");
    Builder.CreateStore(P, paramAddr);
    LocalVarAddrTable[P->getName()] = paramAddr;
  }

  EndOfFunc = BasicBlock::Create(Context, "return", CurFunc);

  const auto &Returns = FD.getReturnParams()->getParams();
  if (Returns.empty()) {
    // is void function
    FD.getBody()->accept(*this);
    Builder.CreateBr(EndOfFunc);
    Builder.SetInsertPoint(EndOfFunc);
    Builder.CreateRetVoid();
  } else if (Returns.size() == 1) {
    RetVal = Builder.CreateAlloca(getLLVMTy(FD), nullptr, "retval");
    FD.getBody()->accept(*this);
    Builder.CreateBr(EndOfFunc);
    Builder.SetInsertPoint(EndOfFunc);
    llvm::Value *V = Builder.CreateLoad(RetVal);
    Builder.CreateRet(V);
  } else {
    assert(false && "tuple return not supported!");
  }
  // move EndOfFunc to the end of CurFunc
  EndOfFunc->removeFromParent();
  EndOfFunc->insertInto(CurFunc);
}

void FuncBodyCodeGen::visit(BlockType &B) { ConstStmtVisitor::visit(B); }

void FuncBodyCodeGen::visit(IfStmtType &IF) {
  const bool Else_exist = (IF.getElse() != nullptr);
  BasicBlock *ThenBB, *ElseBB, *EndBB;
  ThenBB = BasicBlock::Create(Context, "if.then", CurFunc);
  if (Else_exist) {
    ElseBB = BasicBlock::Create(Context, "if.else", CurFunc);
  }
  EndBB = BasicBlock::Create(Context, "if.end", CurFunc);

  IF.getCond()->accept(*this);
  Value *cond =
      Builder.CreateICmpNE(findTempValue(IF.getCond()), False, "cond");
  Builder.CreateCondBr(cond, ThenBB, Else_exist ? ElseBB : EndBB);

  ThenBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(ThenBB);
  IF.getThen()->accept(*this);
  Builder.CreateBr(EndBB);

  if (Else_exist) {
    ElseBB->moveAfter(&CurFunc->back());
    Builder.SetInsertPoint(ElseBB);
    IF.getElse()->accept(*this);
    Builder.CreateBr(EndBB);
  }

  EndBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(EndBB);
}

void FuncBodyCodeGen::visit(WhileStmtType &While) {
  BasicBlock *CondBB = BasicBlock::Create(Context, "while.cond", CurFunc);
  BasicBlock *BodyBB = BasicBlock::Create(Context, "while.body", CurFunc);
  BasicBlock *EndBB = BasicBlock::Create(Context, "while.end", CurFunc);

  // TODO: replace this temp impl
  BasicBlockTable[While.getCond()] = CondBB;
  BasicBlockTable[&While] = EndBB;

  if (While.isDoWhile()) {
    Builder.CreateBr(BodyBB);

    BodyBB->moveAfter(&CurFunc->back());
    Builder.SetInsertPoint(BodyBB);
    While.getBody()->accept(*this);
    Builder.CreateBr(CondBB);

    CondBB->moveAfter(&CurFunc->back());
    Builder.CreateBr(While.isDoWhile() ? BodyBB : CondBB);
    Builder.SetInsertPoint(CondBB);
    While.getCond()->accept(*this);
    Value *cond =
        Builder.CreateICmpNE(findTempValue(While.getCond()), False, "cond");
    Builder.CreateCondBr(cond, BodyBB, EndBB);
  } else {
    Builder.CreateBr(CondBB);
    CondBB->moveAfter(&CurFunc->back());
    Builder.SetInsertPoint(CondBB);
    While.getCond()->accept(*this);
    Value *cond =
        Builder.CreateICmpNE(findTempValue(While.getCond()), False, "cond");
    Builder.CreateCondBr(cond, BodyBB, EndBB);

    BodyBB->moveAfter(&CurFunc->back());
    Builder.SetInsertPoint(BodyBB);
    While.getBody()->accept(*this);
    Builder.CreateBr(CondBB);
  }

  EndBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(EndBB);
}

void FuncBodyCodeGen::visit(ForStmtType &FS) {
  const bool Init_exist = (FS.getInit() != nullptr);
  const bool Cond_exist = (FS.getCond() != nullptr);
  const bool Loop_exist = (FS.getLoop() != nullptr);

  BasicBlock *CondBB = BasicBlock::Create(Context, "for.cond", CurFunc);
  BasicBlock *BodyBB = BasicBlock::Create(Context, "for.body", CurFunc);
  BasicBlock *LoopBB = BasicBlock::Create(Context, "for.loop", CurFunc);
  BasicBlock *EndBB = BasicBlock::Create(Context, "for.end", CurFunc);

  // TODO: replace this temp impl
  BasicBlockTable[FS.getCond()] = CondBB;
  BasicBlockTable[&FS] = EndBB;

  if (Init_exist) {
    FS.getInit()->accept(*this);
  }
  Builder.CreateBr(CondBB);

  CondBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(CondBB);
  if (Cond_exist) {
    FS.getCond()->accept(*this);
    Value *cond = Builder.CreateICmpNE(findTempValue(FS.getCond()), False);
    Builder.CreateCondBr(cond, BodyBB, EndBB);
  } else
    Builder.CreateBr(BodyBB);

  BodyBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(BodyBB);
  FS.getBody()->accept(*this);
  Builder.CreateBr(LoopBB);
  if (Loop_exist) {
    LoopBB->moveAfter(&CurFunc->back());
    Builder.SetInsertPoint(LoopBB);
    FS.getLoop()->accept(*this);
  }
  Builder.CreateBr(CondBB);

  EndBB->moveAfter(&CurFunc->back());
  Builder.SetInsertPoint(EndBB);
}

void FuncBodyCodeGen::visit(ContinueStmtType &CS) {
  BasicBlock *EndBB = BasicBlock::Create(Context, "cont.end", CurFunc);
  if (auto dest = dynamic_cast<const WhileStmt *>(CS.getLoopStmt())) {
    Builder.CreateBr(findBasicBlock(dest->getCond()));
  } else if (auto dest = dynamic_cast<const ForStmt *>(CS.getLoopStmt())) {
    Builder.CreateBr(findBasicBlock(dest->getCond()));
  } else
    assert(false && "Not a WhileStmt or ForStmt");
  Builder.SetInsertPoint(EndBB);
}

void FuncBodyCodeGen::visit(BreakStmtType &BS) {
  BasicBlock *EndBB = BasicBlock::Create(Context, "break.end", CurFunc);
  if (auto dest = dynamic_cast<const WhileStmt *>(BS.getLoopStmt())) {
    Builder.CreateBr(findBasicBlock(dest));
  } else if (auto dest = dynamic_cast<const ForStmt *>(BS.getLoopStmt())) {
    Builder.CreateBr(findBasicBlock(dest));
  } else
    assert(false && "Not a WhileStmt or ForStmt");
  Builder.SetInsertPoint(EndBB);
}

void FuncBodyCodeGen::visit(ReturnStmtType &RS) {
  if (RS.getRetValue() != nullptr) {
    RS.getRetValue()->accept(*this);
    // TODO: move lrvalue cast to another pass
    llvm::Value *V = findTempValue(RS.getRetValue());
    if (RS.getRetValue()->isLValue()) {
      V = Builder.CreateLoad(V);
    }
    Builder.CreateStore(V, RetVal);
  }
  BasicBlock *RetBB = BasicBlock::Create(Context, "return.end", CurFunc);
  Builder.CreateBr(EndOfFunc);
  Builder.SetInsertPoint(RetBB);
}

void FuncBodyCodeGen::visit(DeclStmtType &DS) {
  // TODO: replace this temp impl
  // this impl assumes declared variables are uint64
  for (auto &D : DS.getVarDecls()) {
    auto *p =
        Builder.CreateAlloca(getLLVMTy(D), nullptr, D->getName() + "_addr");
    LocalVarAddrTable[D->getName()] = p;
  }
  // TODO: replace this
  // this impl. assumes no tuple expression;
  if (DS.getValue() != nullptr) {
    DS.getValue()->accept(*this);
    Builder.CreateStore(findTempValue(DS.getValue()),
                        findLocalVarAddr(DS.getVarDecls()[0]->getName()));
  }
}

void FuncBodyCodeGen::visit(UnaryOperatorType &UO) {
  ConstStmtVisitor::visit(UO);
  llvm::Value *V = nullptr;
  if (UO.isArithmeticOp()) {
    llvm::Value *subVal = findTempValue(UO.getSubExpr());
    // TODO: move lrvalue cast to another pass
    if (UO.getSubExpr()->isLValue()) {
      subVal = Builder.CreateLoad(subVal, "BO_SubExpr");
    }
    switch (UO.getOpcode()) {
    case UnaryOperatorKind::UO_Plus:
      V = subVal;
      break;
    case UnaryOperatorKind::UO_Minus:
      V = Builder.CreateNeg(subVal, "UO_Minus");
      break;
    case UnaryOperatorKind::UO_Not:
      V = Builder.CreateNot(subVal, "UO_Not");
      break;
    case UnaryOperatorKind::UO_LNot:
      V = Builder.CreateICmpEQ(subVal, False, "UO_LNot");
      break;
    default:;
    }
  } else {
    // assume subExpr is a LValue
    llvm::Value *subRef =
        findTempValue(UO.getSubExpr()); // the LValue(address) of subexpr
    llvm::Value *subVal = nullptr;      // to store value of subexpr if needed
    llvm::Value *tmp = nullptr;
    switch (UO.getOpcode()) {
    case UnaryOperatorKind::UO_PostInc:
      subVal = Builder.CreateLoad(subRef, "BO_Subexpr");
      tmp = Builder.CreateAdd(subVal, One256, "UO_PostInc");
      Builder.CreateStore(tmp, subRef);
      V = subVal;
      break;
    case UnaryOperatorKind::UO_PostDec:
      subVal = Builder.CreateLoad(subRef, "BO_Subexpr");
      tmp = Builder.CreateSub(subVal, One256, "UO_PostDec");
      Builder.CreateStore(tmp, subRef);
      V = subVal;
      break;
    case UnaryOperatorKind::UO_PreInc:
      subVal = Builder.CreateLoad(subRef, "BO_Subexpr");
      tmp = Builder.CreateAdd(subVal, One256, "UO_PreInc");
      Builder.CreateStore(tmp, subRef);
      V = subRef;
      break;
    case UnaryOperatorKind::UO_PreDec:
      subVal = Builder.CreateLoad(subRef, "BO_Subexpr");
      tmp = Builder.CreateSub(subVal, One256, "UO_PreDec");
      Builder.CreateStore(tmp, subRef);
      V = subRef;
      break;
    default:;
    }
  }
  TempValueTable[&UO] = V;
}

Value *FuncBodyCodeGen::loadValue(const Expr *Expr) {
  Value *Addr = findTempValue(Expr);
  Value *Val = nullptr;
  if (auto *ID = dynamic_cast<const Identifier *>(Expr)) {
    auto *D = dynamic_cast<const VarDecl *>(ID->getCorrespondDecl());
    if (D->isStateVariable()) {
      Value *AddrPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Value *ValPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Builder.CreateStore(Addr, AddrPtr);
      Builder.CreateCall(Module.getFunction("ethereum.storageLoad"),
                         {AddrPtr, ValPtr});
      Val = Builder.CreateLoad(ValPtr);
    } else {
      Val = Builder.CreateLoad(Addr);
    }
  } else if (auto IA = dynamic_cast<const IndexAccess *>(Expr)) {
    if (IA->isStateVariable()) {
      Value *AddrPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Value *ValPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Builder.CreateStore(Addr, AddrPtr);
      Builder.CreateCall(Module.getFunction("ethereum.storageLoad"),
                         {AddrPtr, ValPtr});
      Val = Builder.CreateLoad(ValPtr);
    } else {
      Val = Builder.CreateLoad(Addr);
    }
  } else {
    Val = Builder.CreateLoad(Addr);
  }
  return Val;
}

void FuncBodyCodeGen::storeValue(const Expr *Expr, Value *Val) {
  Value *Addr = findTempValue(Expr);
  if (auto *ID = dynamic_cast<const Identifier *>(Expr)) {
    auto *D = dynamic_cast<const VarDecl *>(ID->getCorrespondDecl());
    if (D->isStateVariable()) {
      Value *AddrPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Value *ValPtr = Builder.CreateAlloca(Val->getType(), nullptr);
      Builder.CreateStore(Addr, AddrPtr);
      Builder.CreateStore(Val, ValPtr);
      Builder.CreateCall(Module.getFunction("ethereum.storageStore"),
                         {AddrPtr, ValPtr});
    } else {
      Builder.CreateStore(Val, Addr);
    }
  } else if (auto IA = dynamic_cast<const IndexAccess *>(Expr)) {
    if (IA->isStateVariable()) {
      Value *AddrPtr = Builder.CreateAlloca(Addr->getType(), nullptr);
      Value *ValPtr = Builder.CreateAlloca(Val->getType(), nullptr);
      Builder.CreateStore(Addr, AddrPtr);
      Builder.CreateStore(Val, ValPtr);
      Builder.CreateCall(Module.getFunction("ethereum.storageStore"),
                         {AddrPtr, ValPtr});
    } else {
      Builder.CreateStore(Val, Addr);
    }
  } else {
    Builder.CreateStore(Val, Addr);
  }
}

void FuncBodyCodeGen::visit(BinaryOperatorType &BO) {
  // TODO: replace this temp impl (visit(BinaryOperatorType &BO))
  // This impl assumes:
  //   every type is uint64
  bool isSigned;
  const Type *Ty = BO.getType().get();
  if (auto TyNow = dynamic_cast<const IntegerType *>(Ty))
    isSigned = TyNow->isSigned();
  else if (auto TyNow = dynamic_cast<const BooleanType *>(Ty))
    isSigned = false;
  else if (auto TyNow = dynamic_cast<const AddressType *>(Ty))
    isSigned = false;
  else
    assert(false && "Wrong type in binary operator!");
  llvm::Value *V = nullptr;
  if (BO.isAssignmentOp()) {
    // TODO: replace this temp impl
    // because we havn't properly dealed with lvalue/rvalue
    // This impl assumes:
    //   lhs of assignment operator (=, +=, -=, ...) is a Identifier,
    ConstStmtVisitor::visit(BO);
    llvm::Value *lhsAddr = findTempValue(BO.getLHS()); // required lhs as LValue
    llvm::Value *rhsVal = findTempValue(BO.getRHS());
    if (BO.getOpcode() == BO_Assign) {
      storeValue(BO.getLHS(), rhsVal);
    } else {
      Value *lhsVal = loadValue(BO.getLHS());
      switch (BO.getOpcode()) {
      case BO_MulAssign:
        lhsVal = Builder.CreateMul(lhsVal, rhsVal, "BO_MulAssign");
        break;
      case BO_DivAssign:
        lhsVal = Builder.CreateUDiv(lhsVal, rhsVal, "BO_DivAssign");
        break;
      case BO_RemAssign:
        lhsVal = Builder.CreateURem(lhsVal, rhsVal, "BO_RemAssign");
        break;
      case BO_AddAssign:
        lhsVal = Builder.CreateAdd(lhsVal, rhsVal, "BO_AddAssign");
        break;
      case BO_SubAssign:
        lhsVal = Builder.CreateSub(lhsVal, rhsVal, "BO_SubAssign");
        break;
      case BO_ShlAssign:
        lhsVal = Builder.CreateShl(lhsVal, rhsVal, "BO_ShlAssign");
        break;
      case BO_ShrAssign:
        lhsVal = Builder.CreateAShr(lhsVal, rhsVal, "BO_ShrAssign");
        break;
      case BO_AndAssign:
        lhsVal = Builder.CreateAnd(lhsVal, rhsVal, "BO_AndAssign");
        break;
      case BO_XorAssign:
        lhsVal = Builder.CreateXor(lhsVal, rhsVal, "BO_XorAssign");
        break;
      case BO_OrAssign:
        lhsVal = Builder.CreateOr(lhsVal, rhsVal, "BO_OrAssign");
        break;
      default:;
      }
      storeValue(BO.getLHS(), lhsVal);
    }
    V = rhsVal;
  }

  if (BO.isAdditiveOp() || BO.isMultiplicativeOp() || BO.isComparisonOp() ||
      BO.isShiftOp() || BO.isBitwiseOp()) {
    using Pred = llvm::CmpInst::Predicate;
    ConstStmtVisitor::visit(BO);
    llvm::Value *lhs = findTempValue(BO.getLHS());
    llvm::Value *rhs = findTempValue(BO.getRHS());
    switch (BO.getOpcode()) {
    case BinaryOperatorKind::BO_Add:
      V = Builder.CreateAdd(lhs, rhs, "BO_ADD");
      break;
    case BinaryOperatorKind::BO_Sub:
      V = Builder.CreateSub(lhs, rhs, "BO_SUB");
      break;
    case BinaryOperatorKind::BO_Mul:
      V = Builder.CreateMul(lhs, rhs, "BO_MUL");
      break;
    case BinaryOperatorKind::BO_Div:
      if (isSigned)
        V = Builder.CreateSDiv(lhs, rhs, "BO_DIV");
      else
        V = Builder.CreateUDiv(lhs, rhs, "BO_DIV");
      break;
    case BinaryOperatorKind::BO_Rem:
      if (isSigned)
        V = Builder.CreateSRem(lhs, rhs, "BO_Rem");
      else
        V = Builder.CreateURem(lhs, rhs, "BO_Rem");
      break;
    case BinaryOperatorKind::BO_GE:
      V = Builder.CreateICmp(Pred(Pred::ICMP_UGE + (isSigned << 2)), lhs, rhs,
                             "BO_GE");
      break;
    case BinaryOperatorKind::BO_GT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_UGT + (isSigned << 2)), lhs, rhs,
                             "BO_GT");
      break;
    case BinaryOperatorKind::BO_LE:
      V = Builder.CreateICmp(Pred(Pred::ICMP_ULE + (isSigned << 2)), lhs, rhs,
                             "BO_LE");
      break;
    case BinaryOperatorKind::BO_LT:
      V = Builder.CreateICmp(Pred(Pred::ICMP_ULT + (isSigned << 2)), lhs, rhs,
                             "BO_LT");
      break;
    case BinaryOperatorKind::BO_EQ:
      V = Builder.CreateICmpEQ(lhs, rhs, "BO_EQ");
      break;
    case BinaryOperatorKind::BO_NE:
      V = Builder.CreateICmpNE(lhs, rhs, "BO_NE");
      break;
    case BinaryOperatorKind::BO_Shl:
      V = Builder.CreateShl(lhs, rhs, "BO_Shl");
      break;
    case BinaryOperatorKind::BO_Shr:
      V = Builder.CreateAShr(lhs, rhs, "BO_Shr");
      break;
    case BinaryOperatorKind::BO_And:
      V = Builder.CreateAnd(lhs, rhs, "BO_And");
      break;
    case BinaryOperatorKind::BO_Xor:
      V = Builder.CreateXor(lhs, rhs, "BO_Xor");
      break;
    case BinaryOperatorKind::BO_Or:
      V = Builder.CreateOr(lhs, rhs, "BO_Or");
      break;
    default:;
    }
  }

  if (BO.isLogicalOp()) {
    // TODO : refactor logical op with phi node
    if (BO.getOpcode() == BO_LAnd) {
      llvm::Value *res = Builder.CreateAlloca(Builder.getIntNTy(1));
      BasicBlock *trueBB = BasicBlock::Create(Context, "BO_LAnd.true", CurFunc);
      BasicBlock *falseBB =
          BasicBlock::Create(Context, "BO_LAnd.false", CurFunc);
      BasicBlock *endBB = BasicBlock::Create(Context, "BO_LAnd.end", CurFunc);

      BO.getLHS()->accept(*this);
      llvm::Value *lhs = findTempValue(BO.getLHS());
      // TODO: move lrvalue cast to another pass
      if (BO.getLHS()->isLValue()) {
        lhs = Builder.CreateLoad(lhs, "BO_Lhs");
      }
      Builder.CreateCondBr(Builder.CreateICmpNE(lhs, False), trueBB, falseBB);

      Builder.SetInsertPoint(trueBB);
      BO.getRHS()->accept(*this);
      llvm::Value *rhs = findTempValue(BO.getRHS());
      // TODO: move lrvalue cast to another pass
      if (BO.getRHS()->isLValue()) {
        rhs = Builder.CreateLoad(rhs, "BO_Rhs");
      }
      Builder.CreateStore(Builder.CreateICmpNE(rhs, False), res);
      Builder.CreateBr(endBB);

      Builder.SetInsertPoint(falseBB);
      Builder.CreateStore(False, res);
      Builder.CreateBr(endBB);

      Builder.SetInsertPoint(endBB);
      // in order to store result of LAnd in TempValueTable
      V = Builder.CreateLoad(res, "BO_LAnd");
    } else if (BO.getOpcode() == BO_LOr) {
      llvm::Value *res = Builder.CreateAlloca(Builder.getIntNTy(1));
      BasicBlock *trueBB = BasicBlock::Create(Context, "BO_LOr.true", CurFunc);
      BasicBlock *falseBB =
          BasicBlock::Create(Context, "BO_LOr.false", CurFunc);
      BasicBlock *endBB = BasicBlock::Create(Context, "BO_LOr.end", CurFunc);

      BO.getLHS()->accept(*this);
      llvm::Value *lhs = findTempValue(BO.getLHS());
      // TODO: move lrvalue cast to another pass
      if (BO.getLHS()->isLValue()) {
        lhs = Builder.CreateLoad(lhs, "BO_Lhs");
      }
      Builder.CreateCondBr(Builder.CreateICmpNE(lhs, False), trueBB, falseBB);

      Builder.SetInsertPoint(trueBB);
      Builder.CreateStore(True, res);
      Builder.CreateBr(endBB);

      Builder.SetInsertPoint(falseBB);
      BO.getRHS()->accept(*this);
      llvm::Value *rhs = findTempValue(BO.getRHS());
      // TODO: move lrvalue cast to another pass
      if (BO.getRHS()->isLValue()) {
        rhs = Builder.CreateLoad(rhs, "BO_Rhs");
      }
      Builder.CreateStore(Builder.CreateICmpNE(rhs, False), res);
      Builder.CreateBr(endBB);

      Builder.SetInsertPoint(endBB);
      // in order to store result of LAnd in TempValueTable
      V = Builder.CreateLoad(res, "BO_LOr");
    }
  }
  TempValueTable[&BO] = V;
}

void FuncBodyCodeGen::visit(CallExprType &CALL) {
  auto Callee = dynamic_cast<const Identifier *>(CALL.getCalleeExpr());
  auto funcName = Callee->getName();
  if (funcName.compare("require") == 0) {
    // require function
    auto Arguments = CALL.getArguments();
    Arguments[0]->accept(*this);
    Value *CondV = findTempValue(Arguments[0]);
    Arguments[1]->accept(*this);
    Value *Length = Builder.getInt32(
        dynamic_cast<const StringLiteral *>(Arguments[1])->getValue().length());

    BasicBlock *RevertBB = BasicBlock::Create(Context, "revert", CurFunc);
    BasicBlock *ContBB = BasicBlock::Create(Context, "continue", CurFunc);

    Builder.CreateCondBr(CondV, ContBB, RevertBB);
    Builder.SetInsertPoint(RevertBB);
    auto *MSG = Builder.CreateInBoundsGEP(
        findTempValue(Arguments[1]), {Builder.getInt32(0), Builder.getInt32(0)},
        "msg.ptr");
    Builder.CreateCall(Module.getFunction("ethereum.revert"), {MSG, Length});
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(ContBB);
  } else if (funcName.compare("assert") == 0) {
    // assert function
    auto Arguments = CALL.getArguments();
    Arguments[0]->accept(*this);
    Value *CondV = findTempValue(Arguments[0]);

    std::string assertFailMsg = "\"Assertion Fail\"";
    Value *Length = Builder.getInt32(assertFailMsg.length() + 1);
    Value *errStr = Builder.CreateGlobalString(assertFailMsg, "assertFailMsg");
    BasicBlock *RevertBB = BasicBlock::Create(Context, "revert", CurFunc);
    BasicBlock *ContBB = BasicBlock::Create(Context, "continue", CurFunc);

    Builder.CreateCondBr(CondV, ContBB, RevertBB);
    Builder.SetInsertPoint(RevertBB);
    Value *MSG = Builder.CreateInBoundsGEP(
        errStr, {Builder.getInt32(0), Length}, "msg.ptr");
    Builder.CreateCall(Module.getFunction("ethereum.revert"), {MSG, Length});
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(ContBB);
  } else if (funcName.compare("revert") == 0) {
    // revert function
    auto Arguments = CALL.getArguments();
    Arguments[0]->accept(*this);
    Value *Length = Builder.getInt32(
        dynamic_cast<const StringLiteral *>(Arguments[0])->getValue().length() +
        1);

    BasicBlock *RevertBB = BasicBlock::Create(Context, "revert", CurFunc);
    Builder.CreateBr(RevertBB);
    Builder.SetInsertPoint(RevertBB);
    auto *MSG = Builder.CreateInBoundsGEP(
        findTempValue(Arguments[0]), {Builder.getInt32(0), Length}, "msg.ptr");
    Builder.CreateCall(Module.getFunction("ethereum.revert"), {MSG, Length});
    Builder.CreateUnreachable();
  } else {
    ConstStmtVisitor::visit(CALL);
    if (auto FD =
            dynamic_cast<const FunctionDecl *>(Callee->getCorrespondDecl())) {

      llvm::Value *RetV = nullptr;
      auto Arguments = CALL.getArguments();
      std::vector<llvm::Value *> argsValue(Arguments.size());
      if (CALL.isNamedCall()) {
        // Call's arguments are with name
        // TODO: implement getParamDecl()
        // current AST cannot get the order the params are declared
        /*
        std::vector<std::string>> declArgOrder = getParamDecl();  // get the
        order the params are declared std::vector<std::string>> passedArgOrder =
        CALL.getNames(); std::map<std::string, llvm::Value*> argName2value; for
        (size_t i = 0; i < Arguments.size(); i++) {
          argName2value[passedArgOrder[i]] = findTempValue(Arguments[i]);
        }
        for (size_t i = 0; i < Arguments.size(); i++) {
          argsValue[i] = argName2value[declArgOrder[i]];
        }
        */
      } else {
        // normal function call
        for (size_t i = 0; i < Arguments.size(); i++) {
          argsValue[i] = findTempValue(Arguments[i]);
        }
      }

      llvm::Function *F = Module.getFunction(funcName);
      if (F->getReturnType()->isVoidTy()) {
        Builder.CreateCall(F, argsValue);
      } else {
        RetV = Builder.CreateCall(F, argsValue, funcName);
        TempValueTable[&CALL] = RetV;
      }
    } else if (auto ED = dynamic_cast<const CallableVarDecl *>(
                   Callee->getCorrespondDecl())) {
    } else {
      assert(false && "Unhandle CallExprType CodeGen case.");
    }
  }
}

void FuncBodyCodeGen::visit(ImplicitCastExprType &IC) {
  ConstStmtVisitor::visit(IC);
  emitCast(IC);
}

void FuncBodyCodeGen::visit(ExplicitCastExprType &EC) {
  ConstStmtVisitor::visit(EC);
  emitCast(EC);
}

void FuncBodyCodeGen::emitCast(const CastExpr &Cast) {
  Value *result = nullptr;
  switch (Cast.getCastKind()) {
  case CastKind::LValueToRValue: {
    // TODO: emit load instruction
    // current impl. just let visit(Identifier&) emit load
    // which does not work for general cases
    result = loadValue(Cast.getSubExpr());
    break;
  }
  case CastKind::IntegralCast: {
    assert(Cast.getType()->getCategory() == Type::Category::Integer &&
           Cast.getSubExpr()->getType()->getCategory() ==
               Type::Category::Integer &&
           "IntegralCast should have int");
    auto CastITy = dynamic_cast<const IntegerType *>(Cast.getType().get());
    auto BaseITy =
        dynamic_cast<const IntegerType *>(Cast.getSubExpr()->getType().get());
    if (BaseITy->isSigned())
      result =
          Builder.CreateSExtOrTrunc(findTempValue(Cast.getSubExpr()),
                                    Builder.getIntNTy(CastITy->getBitNum()));
    else
      result =
          Builder.CreateZExtOrTrunc(findTempValue(Cast.getSubExpr()),
                                    Builder.getIntNTy(CastITy->getBitNum()));
  }
  case CastKind::TypeCast: {
    // TODO : address to sign
    if (auto BaseATy = dynamic_cast<const AddressType *>(
            Cast.getSubExpr()->getType().get())) {
      if (auto CastITy =
              dynamic_cast<const IntegerType *>(Cast.getType().get())) {
        assert(BaseATy->getBitNum() <= CastITy->getBitNum() &&
               "Integer Type too small");
        result =
            Builder.CreateZExtOrTrunc(findTempValue(Cast.getSubExpr()),
                                      Builder.getIntNTy(CastITy->getBitNum()));
      }
    }
    if (auto BaseITy = dynamic_cast<const IntegerType *>(
            Cast.getSubExpr()->getType().get())) {
      assert(!BaseITy->isSigned() && "Cannot cast from signed to address");
      if (auto CastATy =
              dynamic_cast<const AddressType *>(Cast.getType().get())) {
        result =
            Builder.CreateZExtOrTrunc(findTempValue(Cast.getSubExpr()),
                                      Builder.getIntNTy(CastATy->getBitNum()));
      }
    }

    break;
  }
  default:
    break;
  }
  TempValueTable[&Cast] = result;
}

void FuncBodyCodeGen::visit(ParenExprType &P) {
  ConstStmtVisitor::visit(P);
  TempValueTable[&P] = findTempValue(P.getSubExpr());
}

void FuncBodyCodeGen::visit(IdentifierType &ID) {
  // TODO: replace this temp impl
  // this impl assumes visited Identifier is lvalue
  llvm::Value *V = nullptr;

  const Decl *D = ID.getCorrespondDecl();

  if (auto *VD = dynamic_cast<const VarDecl *>(D)) {
    if (VD->isStateVariable()) {
      // allocate storage position if not allocated
      int PosInStorage = ASTCtx.findStoragePosition(ID.getName());
      V = Builder.getIntN(256, PosInStorage);
    } else {
      if (llvm::Value *Addr = findLocalVarAddr(ID.getName()))
        V = Addr;
      else
        assert(false && "undeclared identifier");
    }
  }

  TempValueTable[&ID] = V;
}

void FuncBodyCodeGen::emitConcate(llvm::Value *emitConcateArr,
                                  unsigned BaseBitNum, unsigned IdxBitNum,
                                  llvm::Value *BaseV, llvm::Value *IdxV) {
  std::vector<unsigned> BitNum{BaseBitNum, IdxBitNum};
  std::vector<llvm::Value *> Val{BaseV, IdxV};
  for (int i = 0, idx = 0; i < 2; i++) {
    llvm::Value *Ptr = Builder.CreateInBoundsGEP(
        emitConcateArr, {Builder.getInt32(0), Builder.getInt32(idx)});
    llvm::Value *CPtr = Builder.CreatePointerCast(
        Ptr, llvm::Type::getIntNPtrTy(Context, BitNum[i]), "PtrCast");
    Builder.CreateStore(Val[i], CPtr);
    idx += BitNum[i] / 8;
  }
}

// codegen for checking whether array idx is out of bound
void FuncBodyCodeGen::emitCheckArrayOutOfBound(llvm::Value *ArrSz,
                                               llvm::Value *Idx) {
  static std::string ErrMsg = "\"Array out of bound\"";
  static llvm::Value *ErrStr =
      Builder.CreateGlobalString(ErrMsg, "ExceptionMsg");
  static llvm::Value *Length = Builder.getInt32(ErrMsg.length() + 1);
  BasicBlock *RevertBB = BasicBlock::Create(Context, "out_of_bound", CurFunc);
  BasicBlock *ContBB = BasicBlock::Create(Context, "continue", CurFunc);

  llvm::Value *OutOfBound = Builder.CreateICmpUGE(Idx, ArrSz, "BO_GE");
  Builder.CreateCondBr(OutOfBound, RevertBB, ContBB);

  Builder.SetInsertPoint(RevertBB);
  Value *MSG = Builder.CreateInBoundsGEP(ErrStr, {Builder.getInt32(0), Length},
                                         "msg.ptr");
  Builder.CreateCall(Module.getFunction("ethereum.revert"), {MSG, Length});
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(ContBB);
}

void FuncBodyCodeGen::visit(IndexAccessType &IA) {
  ConstStmtVisitor::visit(IA);
  llvm::Value *BaseV = findTempValue(IA.getBase());
  llvm::Value *IdxV = findTempValue(IA.getIndex());
  llvm::Value *V = nullptr;
  const Type *ExprTy = IA.getBase()->getType().get();

  if (IA.getBase()->getType()->getCategory() == Type::Category::Mapping) {
    // mapping : store i256 hash value in TempValueTable
    const MappingType *MapTy = dynamic_cast<const MappingType *>(ExprTy);
    unsigned BaseBitNum = MapTy->getBitNum(); // const 256
    unsigned IdxBitNum = MapTy->getKeyType()->getBitNum();
    unsigned emitConcateArrLength = (BaseBitNum + IdxBitNum) / 8;
    llvm::ArrayType *emitConcateArrTy =
        llvm::ArrayType::get(Builder.getInt8Ty(), emitConcateArrLength);
    llvm::Value *emitConcateArr =
        Builder.CreateAlloca(emitConcateArrTy, nullptr, "emitConcateArr");
    emitConcate(emitConcateArr, BaseBitNum, IdxBitNum, BaseV, IdxV);

    llvm::Value *bytes = llvm::ConstantAggregateZero::get(BytesTy);
    bytes = Builder.CreateInsertValue(
        bytes, Builder.getInt32(emitConcateArrLength), {0});
    bytes = Builder.CreateInsertValue(
        bytes,
        Builder.CreateInBoundsGEP(emitConcateArr,
                                  {Builder.getInt32(0), Builder.getInt32(0)}),
        {1});

    V = Builder.CreateCall(Module.getFunction("solidity.keccak256"), {bytes},
                           "Mapping");
  } else if (IA.getBase()->getType()->getCategory() == Type::Category::Array) {
    // Array Type : Fixed Size Mem Array, Fixed Sized Storage Array, Dynamic
    // Sized Storage Array
    // Require Index to be unsigned 256-bit Int

    const ArrayType *ArrTy = dynamic_cast<const ArrayType *>(ExprTy);

    if (!IA.isStateVariable()) {
      // Fixed size memory array : store array address in TempValueTable
      // TODO : Assume only Integer Array
      unsigned ArraySize = ArrTy->getLength();
      emitCheckArrayOutOfBound(Builder.getIntN(256, ArraySize), IdxV);
      llvm::Type *Ty = getLLVMTy(ArrTy);
      V = Builder.CreateInBoundsGEP(Ty, BaseV, {Builder.getIntN(256, 0), IdxV},
                                    "arrIdxAddr");
    } else if (ArrTy->isDynamicSized()) {
      // Dynamic Storage Array : store hash value in TempValueTable
      // TODO: modify this, dyn array size is stored in memory but currently I
      // don't know where can I load the correct value
      unsigned ArraySize = 7122;
      emitCheckArrayOutOfBound(Builder.getIntN(256, ArraySize), IdxV);
      unsigned BaseBitNum = ArrTy->getBitNum(); // always 256 bit
      unsigned IdxBitNum =
          IA.getIndex()->getType()->getBitNum(); // always 256 bit
      unsigned emitConcateArrLength = (BaseBitNum + IdxBitNum) / 8;
      llvm::ArrayType *emitConcateArrTy =
          llvm::ArrayType::get(Builder.getInt8Ty(), emitConcateArrLength);
      llvm::Value *emitConcateArr =
          Builder.CreateAlloca(emitConcateArrTy, nullptr, "emitConcateArr");
      emitConcate(emitConcateArr, BaseBitNum, IdxBitNum, BaseV, IdxV);

      llvm::Value *bytes = llvm::ConstantAggregateZero::get(BytesTy);
      bytes = Builder.CreateInsertValue(
          bytes, Builder.getInt32(emitConcateArrLength), {0});
      bytes = Builder.CreateInsertValue(
          bytes,
          Builder.CreateInBoundsGEP(emitConcateArr,
                                    {Builder.getInt32(0), Builder.getInt32(0)}),
          {1});

      V = Builder.CreateCall(Module.getFunction("solidity.keccak256"), {bytes},
                             "DynArrEntry");
    } else {
      // Fixed Size Storage Array : store storage address of slot the accessed
      // element belongs to in TempValueTable
      unsigned ArraySize = ArrTy->getLength();
      emitCheckArrayOutOfBound(Builder.getIntN(256, ArraySize), IdxV);
      unsigned BytePerElement = ArrTy->getElementType()->getBitNum() / 8;
      llvm::Value *Slot =
          Builder.CreateUDiv(Builder.getIntN(256, 32),
                             Builder.getIntN(256, BytePerElement), "BO_Div");
      V = Builder.CreateAdd(BaseV, Builder.CreateUDiv(IdxV, Slot, "BO_Div"));
    }
  }
  TempValueTable[&IA] = V;
}

void FuncBodyCodeGen::visit(MemberExprType &ME) {
  // XXX: only implement msg.sender
  // need to handle more msg.*, tx.*, block.*
  ConstStmtVisitor::visit(ME);
  llvm::Value *V = nullptr;

  auto *BaseID = dynamic_cast<const Identifier *>(ME.getBase());
  if (BaseID != nullptr && BaseID->getName().compare("msg") == 0) {
    if (ME.getName()->getName().compare("sender") == 0) {
      Value *ValPtr = Builder.CreateAlloca(AddressTy);
      Builder.CreateCall(Module.getFunction("getCaller"), {ValPtr});
      V = Builder.CreateLoad(ValPtr);
    } else {
      assert(false && "Unsuuported member access for msg");
    }
  } else {
    assert(false && "Can only suuport msg.sender now");
  }

  TempValueTable[&ME] = V;
}

void FuncBodyCodeGen::visit(BooleanLiteralType &BL) {
  TempValueTable[&BL] = Builder.getInt1(BL.getValue());
}

void FuncBodyCodeGen::visit(StringLiteralType &SL) {
  TempValueTable[&SL] = Builder.CreateGlobalString(SL.getValue(), "str");
}

void FuncBodyCodeGen::visit(NumberLiteralType &NL) {
  TempValueTable[&NL] =
      Builder.getIntN(NL.getType()->getBitNum(), NL.getValue());
}

} // namespace soll
