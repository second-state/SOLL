// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/LoweringInteger.h"
#include <llvm/IR/Argument.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/Support/Casting.h>
#include <mutex>

namespace soll {

namespace {

static bool isSigned(unsigned OpCode) {
  switch (OpCode) {
  case llvm::Instruction::AShr:
  case llvm::Instruction::SDiv:
  case llvm::Instruction::SRem:
    return true;
  default:
    return false;
  }
}

static std::string createName(llvm::StringRef Prefix, unsigned int BitWidth) {
  std::string Result;
  llvm::raw_string_ostream os(Result);
  os << Prefix << BitWidth;
  return Result;
}

} // namespace

llvm::Function *LoweringInteger::GetMulFunction(const unsigned int BitWidth) {
  {
    auto iter = MulFunction.find(BitWidth);
    if (iter != MulFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::Type *IntHiTy = Builder.getIntNTy(BitWidth);
  llvm::Type *IntLoTy = Builder.getIntNTy(BitWidth / 2);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(IntHiTy, {IntHiTy, IntHiTy}, false),
      llvm::Function::InternalLinkage, createName("__mul", BitWidth),
      *TheModule);
  MulFunction.try_emplace(BitWidth, Result);

  llvm::Argument *LHS = Result->arg_begin();
  LHS->setName("lhs");
  llvm::Argument *RHS = LHS + 1;
  RHS->setName("rhs");

  const uint64_t Shift = BitWidth / 2;
  const llvm::APInt HalfMask =
      llvm::APInt::getLowBitsSet(BitWidth / 2, BitWidth / 4);
  const uint64_t HalfShift = BitWidth / 4;

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Result);

  Builder.SetInsertPoint(Entry);
  llvm::Value *LHS_L = Builder.CreateTrunc(LHS, IntLoTy, "lhs_l");
  llvm::Value *LHS_H =
      Builder.CreateTrunc(Builder.CreateLShr(LHS, Shift), IntLoTy, "lhs_h");
  llvm::Value *RHS_L = Builder.CreateTrunc(RHS, IntLoTy, "rhs_l");
  llvm::Value *RHS_H =
      Builder.CreateTrunc(Builder.CreateLShr(RHS, Shift), IntLoTy, "rhs_h");

  llvm::Value *LHS_LL = Builder.CreateAnd(LHS_L, HalfMask, "lhs_ll");
  llvm::Value *RHS_LL = Builder.CreateAnd(RHS_L, HalfMask, "rhs_ll");
  llvm::Value *T = Builder.CreateMul(LHS_LL, RHS_LL, "t");

  llvm::Value *T_L = Builder.CreateAnd(T, HalfMask, "t_l");
  llvm::Value *T_H = Builder.CreateLShr(T, HalfShift, "t_h");

  llvm::Value *LHS_LH = Builder.CreateLShr(LHS_L, HalfShift, "lhs_lh");
  llvm::Value *RHS_LH = Builder.CreateLShr(RHS_L, HalfShift, "rhs_lh");

  llvm::Value *U =
      Builder.CreateAdd(Builder.CreateMul(LHS_LH, RHS_LL), T_H, "u");
  llvm::Value *U_L = Builder.CreateAnd(U, HalfMask, "u_l");
  llvm::Value *U_H = Builder.CreateLShr(U, HalfShift, "u_h");

  llvm::Value *V =
      Builder.CreateAdd(Builder.CreateMul(LHS_LL, RHS_LH), U_L, "v");
  llvm::Value *V_H = Builder.CreateLShr(V, HalfShift, "v_h");

  llvm::Value *W = Builder.CreateAdd(Builder.CreateMul(LHS_LH, RHS_LH),
                                     Builder.CreateAdd(U_H, V_H), "w");

  llvm::Value *O_L = Builder.CreateZExt(
      Builder.CreateAdd(T_L, Builder.CreateShl(V, HalfShift)), IntHiTy, "o_l");
  llvm::Value *O_H = Builder.CreateZExt(
      Builder.CreateAdd(W, Builder.CreateAdd(Builder.CreateMul(RHS_H, LHS_L),
                                             Builder.CreateMul(RHS_L, LHS_H))),
      IntHiTy, "o_h");

  llvm::Value *O = Builder.CreateAdd(O_L, Builder.CreateShl(O_H, Shift), "o");

  Builder.CreateRet(O);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

llvm::Function *LoweringInteger::GetUDivFunction(const unsigned int BitWidth) {
  {
    auto iter = UDivFunction.find(BitWidth);
    if (iter != UDivFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::IntegerType *DivTy = Builder.getIntNTy(BitWidth);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(DivTy, {DivTy, DivTy}, false),
      llvm::Function::InternalLinkage, createName("__udiv", BitWidth),
      *TheModule);
  UDivFunction.try_emplace(BitWidth, Result);

  llvm::Argument *Dividend = Result->arg_begin();
  Dividend->setName("dividend");
  llvm::Argument *Divisor = Dividend + 1;
  Divisor->setName("divisor");

  llvm::ConstantInt *Zero = Builder.getIntN(BitWidth, 0);
  llvm::ConstantInt *One = Builder.getIntN(BitWidth, 1);
  llvm::ConstantInt *MSB = Builder.getIntN(BitWidth, BitWidth - 1);
  llvm::ConstantInt *True = Builder.getTrue();
  llvm::Function *CTLZ =
      llvm::Intrinsic::getDeclaration(TheModule, llvm::Intrinsic::ctlz, DivTy);

  // Our CFG is going to look like:
  // +----------------+
  // | special-cases  |
  // |   ...          |
  // +----------------+
  //  |       |
  //  |   +-----------+
  //  |   | preheader |
  //  |   |  ...      |
  //  |   +-----------+
  //  |       |
  //  |       |     +---+
  //  |       |     |   |
  //  |   +-----------+ |
  //  |   | do-while  | |
  //  |   |  ...      | |
  //  |   +-----------+ |
  //  |       |     |   |
  //  |       |     +---+
  //  |       |
  //  |   +-----------+
  //  |   | loop-exit |
  //  |   |  ...      |
  //  |   +-----------+
  //  |     |
  // +-------+
  // | ...   |
  // | end   |
  // +-------+

  llvm::BasicBlock *SpecialCases =
      llvm::BasicBlock::Create(Context, "special-cases", Result);
  llvm::BasicBlock *Preheader =
      llvm::BasicBlock::Create(Context, "preheader", Result);
  llvm::BasicBlock *DoWhile =
      llvm::BasicBlock::Create(Context, "do-while", Result);
  llvm::BasicBlock *LoopExit =
      llvm::BasicBlock::Create(Context, "loop-exit", Result);
  llvm::BasicBlock *End = llvm::BasicBlock::Create(Context, "end", Result);

  // First off, check for special cases: dividend or divisor is zero, divisor
  // is greater than dividend, and divisor is 1.
  // ; special-cases:
  // ;   %ret0_1      = icmp eq i32 %divisor, 0
  // ;   %ret0_2      = icmp eq i32 %dividend, 0
  // ;   %ret0_3      = or i1 %ret0_1, %ret0_2
  // ;   %tmp0        = tail call i32 @llvm.ctlz.i32(i32 %divisor, i1 true)
  // ;   %tmp1        = tail call i32 @llvm.ctlz.i32(i32 %dividend, i1 true)
  // ;   %sr          = sub nsw i32 %tmp0, %tmp1
  // ;   %ret0_4      = icmp ugt i32 %sr, 31
  // ;   %ret0        = or i1 %ret0_3, %ret0_4
  // ;   %retDividend = icmp eq i32 %tmp0, 31
  // ;   %retVal      = select i1 %ret0, i32 0, i32 %dividend
  // ;   %earlyRet    = or i1 %ret0, %retDividend
  // ;   br i1 %earlyRet, label %end, label %preheader
  Builder.SetInsertPoint(SpecialCases);
  llvm::Value *Ret0_1 = Builder.CreateICmpEQ(Divisor, Zero);
  llvm::Value *Ret0_2 = Builder.CreateICmpEQ(Dividend, Zero);
  llvm::Value *Ret0_3 = Builder.CreateOr(Ret0_1, Ret0_2);
  llvm::Value *Tmp0 = Builder.CreateCall(CTLZ, {Divisor, True});
  llvm::Value *Tmp1 = Builder.CreateCall(CTLZ, {Dividend, True});
  llvm::Value *SR = Builder.CreateSub(Tmp0, Tmp1);

  llvm::Value *Ret0_4 = Builder.CreateICmpUGT(SR, MSB);
  llvm::Value *Ret0 = Builder.CreateOr(Ret0_3, Ret0_4);
  llvm::Value *RetDividend = Builder.CreateICmpEQ(Tmp0, MSB);
  llvm::Value *RetVal = Builder.CreateSelect(Ret0, Zero, Dividend);
  llvm::Value *EarlyRet = Builder.CreateOr(Ret0, RetDividend);
  Builder.CreateCondBr(EarlyRet, End, Preheader);

  // ; preheader:                                           ; preds =
  // %special-case ;   %sr_1     = add i32 %sr, 1 ;   %tmp2     = sub i32 31,
  // %sr ;   %q        = shl i32 %dividend, %tmp2 ;   %tmp3 = lshr i32
  // %dividend, %sr_1 ;   %tmp4 = add i32 %divisor, -1 ;   br label %do-while
  Builder.SetInsertPoint(Preheader);
  llvm::Value *SR_1 = Builder.CreateAdd(SR, One);
  llvm::Value *Tmp2 = Builder.CreateSub(MSB, SR);

  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Dividend, Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 32),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 64),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 96),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 128),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 160),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 196),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Dividend, 228),
  // Builder.getInt32Ty()));

  llvm::Value *Q = Builder.CreateShl(Dividend, Tmp2);

  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Q, Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 32),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 64),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 96),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 128),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 160),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 196),
  // Builder.getInt32Ty()));
  // Builder.CreateCall(TheModule->getFunction("print32"),
  // Builder.CreateZExtOrTrunc(Builder.CreateLShr(Q, 228),
  // Builder.getInt32Ty()));

  llvm::Value *Tmp3 = Builder.CreateLShr(Dividend, SR_1);
  llvm::Value *Tmp4 = Builder.CreateSub(Divisor, One);
  Builder.CreateBr(DoWhile);

  // ; do-while:                                 ; preds = %do-while, %preheader
  // ;   %carry_1 = phi i32 [ 0, %preheader ], [ %carry, %do-while ]
  // ;   %sr_3    = phi i32 [ %sr_1, %preheader ], [ %sr_2, %do-while ]
  // ;   %r_1     = phi i32 [ %tmp3, %preheader ], [ %r, %do-while ]
  // ;   %q_2     = phi i32 [ %q, %preheader ], [ %q_1, %do-while ]
  // ;   %tmp5  = shl i32 %r_1, 1
  // ;   %tmp6  = lshr i32 %q_2, 31
  // ;   %r_2  = or i32 %tmp5, %tmp6
  // ;   %tmp7  = shl i32 %q_2, 1
  // ;   %q_1   = or i32 %carry_1, %tmp7
  // ;   %tmp8  = sub i32 %tmp4, %r_2
  // ;   %s = ashr i32 %tmp8, 31
  // ;   %carry = and i32 %s, 1
  // ;   %tmp9 = and i32 %s, %divisor
  // ;   %r     = sub i32 %r_2, %tmp9
  // ;   %sr_2  = add i32 %sr_3, -1
  // ;   %tmp10 = icmp eq i32 %sr_2, 0
  // ;   br i1 %tmp10, label %loop-exit, label %do-while
  Builder.SetInsertPoint(DoWhile);
  llvm::PHINode *Carry_1 = Builder.CreatePHI(DivTy, 2);
  llvm::PHINode *SR_3 = Builder.CreatePHI(DivTy, 2);
  llvm::PHINode *R_1 = Builder.CreatePHI(DivTy, 2);
  llvm::PHINode *Q_2 = Builder.CreatePHI(DivTy, 2);
  llvm::Value *Tmp5 = Builder.CreateShl(R_1, One);
  llvm::Value *Tmp6 = Builder.CreateLShr(Q_2, MSB);
  llvm::Value *R_2 = Builder.CreateOr(Tmp5, Tmp6);

  llvm::Value *Tmp7 = Builder.CreateShl(Q_2, One);
  llvm::Value *Q_1 = Builder.CreateOr(Carry_1, Tmp7);
  llvm::Value *Tmp8 = Builder.CreateSub(Tmp4, R_2);
  llvm::Value *S = Builder.CreateAShr(Tmp8, MSB);

  llvm::Value *Carry = Builder.CreateAnd(S, One);
  llvm::Value *Tmp9 = Builder.CreateAnd(S, Divisor);
  llvm::Value *R = Builder.CreateSub(R_2, Tmp9);
  llvm::Value *SR_2 = Builder.CreateSub(SR_3, One);
  llvm::Value *Tmp10 = Builder.CreateICmpEQ(SR_2, Zero);
  Builder.CreateCondBr(Tmp10, LoopExit, DoWhile);

  // ; loop-exit:                                      ; preds = %do-while, %bb1
  // ;   %tmp11 = shl i32 %q_1, 1
  // ;   %q_4   = or i32 %carry, %tmp11
  // ;   br label %end
  Builder.SetInsertPoint(LoopExit);
  llvm::Value *Tmp11 = Builder.CreateShl(Q_1, One);
  llvm::Value *Q_4 = Builder.CreateOr(Carry, Tmp11);
  Builder.CreateBr(End);

  // ; end:                                 ; preds = %loop-exit, %special-cases
  // ;   %q_5 = phi i32 [ %q_4, %loop-exit ], [ %retVal, %special-cases ]
  // ;   ret i32 %q_5
  Builder.SetInsertPoint(End, End->begin());
  llvm::PHINode *Q_5 = Builder.CreatePHI(DivTy, 2);

  // Populate the Phis, since all values have now been created. Our Phis were:
  // ;   %carry_1 = phi i32 [ 0, %preheader ], [ %carry, %do-while ]
  Carry_1->addIncoming(Zero, Preheader);
  Carry_1->addIncoming(Carry, DoWhile);
  // ;   %sr_3 = phi i32 [ %sr_1, %preheader ], [ %sr_2, %do-while ]
  SR_3->addIncoming(SR_1, Preheader);
  SR_3->addIncoming(SR_2, DoWhile);
  // ;   %r_1 = phi i32 [ %tmp3, %preheader ], [ %r, %do-while ]
  R_1->addIncoming(Tmp3, Preheader);
  R_1->addIncoming(R, DoWhile);
  // ;   %q_2 = phi i32 [ %q, %preheader ], [ %q_1, %do-while ]
  Q_2->addIncoming(Q, Preheader);
  Q_2->addIncoming(Q_1, DoWhile);
  // ;   %q_5 = phi i32 [ %q_4, %loop-exit ], [ %retVal, %special-cases ]
  Q_5->addIncoming(Q_4, LoopExit);
  Q_5->addIncoming(RetVal, SpecialCases);

  Builder.CreateRet(Q_5);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

llvm::Function *LoweringInteger::GetURemFunction(const unsigned int BitWidth) {
  {
    auto iter = URemFunction.find(BitWidth);
    if (iter != URemFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::IntegerType *DivTy = Builder.getIntNTy(BitWidth);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(DivTy, {DivTy, DivTy}, false),
      llvm::Function::InternalLinkage, createName("__urem", BitWidth),
      *TheModule);
  URemFunction.try_emplace(BitWidth, Result);

  llvm::Argument *Dividend = Result->arg_begin();
  Dividend->setName("dividend");
  llvm::Argument *Divisor = Dividend + 1;
  Divisor->setName("divisor");

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Result);

  // Remainder = Dividend - Quotient*Divisor

  // ;   %quotient  = udiv i32 %dividend, %divisor
  // ;   %product   = mul i32 %divisor, %quotient
  // ;   %remainder = sub i32 %dividend, %product
  Builder.SetInsertPoint(Entry);
  llvm::Value *Quotient = Builder.CreateUDiv(Dividend, Divisor);
  llvm::Value *Product = Builder.CreateMul(Divisor, Quotient);
  llvm::Value *Remainder = Builder.CreateSub(Dividend, Product);
  Builder.CreateRet(Remainder);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

llvm::Function *LoweringInteger::GetShlFunction(const unsigned int BitWidth) {
  {
    auto iter = ShlFunction.find(BitWidth);
    if (iter != ShlFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::IntegerType *IntHiTy = Builder.getIntNTy(BitWidth);
  llvm::IntegerType *IntLoTy = Builder.getIntNTy(BitWidth / 2);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(IntHiTy, {IntHiTy, IntHiTy}, false),
      llvm::Function::InternalLinkage, createName("__shl", BitWidth),
      *TheModule);
  // Result->addFnAttr(llvm::Attribute::get(Context,
  // llvm::Attribute::NoInline));
  ShlFunction.try_emplace(BitWidth, Result);

  llvm::Argument *In = Result->arg_begin();
  In->setName("in");
  llvm::Argument *Amount = In + 1;
  Amount->setName("amount");

  llvm::ConstantInt *HalfWidth = Builder.getIntN(BitWidth, BitWidth / 2);
  const uint64_t Shift = BitWidth / 2;

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Result);

  Builder.SetInsertPoint(Entry);
  llvm::Value *Amount2 = Builder.CreateTrunc(Amount, IntLoTy, "amount2");
  llvm::Value *AmountLack = Builder.CreateTrunc(
      Builder.CreateSub(HalfWidth, Amount), IntLoTy, "amount_lack");
  llvm::Value *AmountExcess = Builder.CreateTrunc(
      Builder.CreateSub(Amount, HalfWidth), IntLoTy, "amount_excess");
  llvm::Value *IsShort = Builder.CreateICmpULT(Amount, HalfWidth, "is_short");
  llvm::Value *IsZero =
      Builder.CreateICmpEQ(Amount, Builder.getIntN(BitWidth, 0), "is_zero");

  llvm::Value *InL = Builder.CreateTrunc(In, IntLoTy, "in_l");
  llvm::Value *const InH =
      Builder.CreateTrunc(Builder.CreateLShr(In, Shift), IntLoTy, "in_h");

  llvm::Value *LoS = Builder.CreateShl(InL, Amount2, "lo_s");
  llvm::Value *HiS =
      Builder.CreateOr(Builder.CreateShl(InH, Amount2),
                       Builder.CreateLShr(InL, AmountLack), "hi_s");
  llvm::Value *LoL = Builder.getIntN(BitWidth / 2, 0);
  llvm::Value *HiL = Builder.CreateShl(InL, AmountExcess, "hi_l");

  llvm::Value *Lo = Builder.CreateZExt(Builder.CreateSelect(IsShort, LoS, LoL),
                                       IntHiTy, "lo");
  llvm::Value *Hi = Builder.CreateZExt(
      Builder.CreateSelect(IsZero, InH,
                           Builder.CreateSelect(IsShort, HiS, HiL)),
      IntHiTy, "hi");

  llvm::Value *O = Builder.CreateOr(Lo, Builder.CreateShl(Hi, Shift), "o");

  Builder.CreateRet(O);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

llvm::Function *LoweringInteger::GetLShrFunction(const unsigned int BitWidth) {
  {
    auto iter = LShrFunction.find(BitWidth);
    if (iter != LShrFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::IntegerType *IntHiTy = Builder.getIntNTy(BitWidth);
  llvm::IntegerType *IntLoTy = Builder.getIntNTy(BitWidth / 2);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(IntHiTy, {IntHiTy, IntHiTy}, false),
      llvm::Function::InternalLinkage, createName("__lshr", BitWidth),
      *TheModule);
  // Result->addFnAttr(llvm::Attribute::get(Context,
  // llvm::Attribute::NoInline));
  LShrFunction.try_emplace(BitWidth, Result);

  llvm::Argument *In = Result->arg_begin();
  In->setName("in");
  llvm::Argument *Amount = In + 1;
  Amount->setName("amount");

  llvm::ConstantInt *HalfWidth = Builder.getIntN(BitWidth, BitWidth / 2);
  const uint64_t Shift = BitWidth / 2;

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Result);

  Builder.SetInsertPoint(Entry);
  llvm::Value *Amount2 = Builder.CreateTrunc(Amount, IntLoTy, "amount2");

  llvm::Value *AmountLack = Builder.CreateTrunc(
      Builder.CreateSub(HalfWidth, Amount), IntLoTy, "amount_lack");
  llvm::Value *AmountExcess = Builder.CreateTrunc(
      Builder.CreateSub(Amount, HalfWidth), IntLoTy, "amount_excess");
  llvm::Value *IsShort = Builder.CreateICmpULT(Amount, HalfWidth, "is_short");
  llvm::Value *IsZero =
      Builder.CreateICmpEQ(Amount, Builder.getIntN(BitWidth, 0), "is_zero");

  llvm::Value *InL = Builder.CreateTrunc(In, IntLoTy, "in_l");
  llvm::Value *const InH =
      Builder.CreateTrunc(Builder.CreateLShr(In, Shift), IntLoTy, "in_h");

  llvm::Value *HiS = Builder.CreateLShr(InH, Amount2, "hi_s");
  llvm::Value *LoS = Builder.CreateOr(Builder.CreateShl(InH, AmountLack),
                                      Builder.CreateLShr(InL, Amount2), "lo_s");
  llvm::Value *HiL = Builder.getIntN(BitWidth / 2, 0);
  llvm::Value *LoL = Builder.CreateLShr(InH, AmountExcess, "lo_l");

  llvm::Value *Hi = Builder.CreateZExt(Builder.CreateSelect(IsShort, HiS, HiL),
                                       IntHiTy, "hi");
  llvm::Value *Lo = Builder.CreateZExt(
      Builder.CreateSelect(IsZero, InL,
                           Builder.CreateSelect(IsShort, LoS, LoL)),
      IntHiTy, "lo");

  llvm::Value *O = Builder.CreateOr(Lo, Builder.CreateShl(Hi, Shift), "o");

  Builder.CreateRet(O);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

llvm::Function *LoweringInteger::GetAShrFunction(const unsigned int BitWidth) {
  {
    auto iter = AShrFunction.find(BitWidth);
    if (iter != AShrFunction.end()) {
      return iter->getSecond();
    }
  }
  llvm::LLVMContext &Context = TheModule->getContext();
  llvm::IRBuilder<> Builder(Context);
  llvm::IntegerType *IntHiTy = Builder.getIntNTy(BitWidth);
  llvm::IntegerType *IntLoTy = Builder.getIntNTy(BitWidth / 2);
  llvm::Function *Result = llvm::Function::Create(
      llvm::FunctionType::get(IntHiTy, {IntHiTy, IntHiTy}, false),
      llvm::Function::InternalLinkage, createName("__ashr", BitWidth),
      *TheModule);
  // Result->addFnAttr(llvm::Attribute::get(Context,
  // llvm::Attribute::NoInline));
  AShrFunction.try_emplace(BitWidth, Result);

  llvm::Argument *In = Result->arg_begin();
  In->setName("in");
  llvm::Argument *Amount = In + 1;
  Amount->setName("amount");

  llvm::ConstantInt *HalfWidth = Builder.getIntN(BitWidth, BitWidth / 2);
  const uint64_t Shift = BitWidth / 2;

  llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Result);

  Builder.SetInsertPoint(Entry);
  llvm::Value *Amount2 = Builder.CreateTrunc(Amount, IntLoTy, "amount2");

  llvm::Value *AmountLack = Builder.CreateTrunc(
      Builder.CreateSub(HalfWidth, Amount), IntLoTy, "amount_lack");
  llvm::Value *AmountExcess = Builder.CreateTrunc(
      Builder.CreateSub(Amount, HalfWidth), IntLoTy, "amount_excess");
  llvm::Value *IsShort = Builder.CreateICmpULT(Amount, HalfWidth, "is_short");
  llvm::Value *IsZero =
      Builder.CreateICmpEQ(Amount, Builder.getIntN(BitWidth, 0), "is_zero");

  llvm::Value *InL = Builder.CreateTrunc(In, IntLoTy, "in_l");
  llvm::Value *const InH =
      Builder.CreateTrunc(Builder.CreateLShr(In, Shift), IntLoTy, "in_h");

  llvm::Value *HiS = Builder.CreateAShr(InH, Amount2, "hi_s");
  llvm::Value *LoS = Builder.CreateOr(Builder.CreateShl(InH, AmountLack),
                                      Builder.CreateLShr(InL, Amount2), "lo_s");
  llvm::Value *HiL = Builder.CreateAShr(InH, Shift - 1, "hi_l");
  llvm::Value *LoL = Builder.CreateAShr(InH, AmountExcess, "lo_l");

  llvm::Value *Hi = Builder.CreateZExt(Builder.CreateSelect(IsShort, HiS, HiL),
                                       IntHiTy, "hi");
  llvm::Value *Lo = Builder.CreateZExt(
      Builder.CreateSelect(IsZero, InL,
                           Builder.CreateSelect(IsShort, LoS, LoL)),
      IntHiTy, "lo");

  llvm::Value *O = Builder.CreateOr(Lo, Builder.CreateShl(Hi, Shift), "o");

  Builder.CreateRet(O);

  for (llvm::BasicBlock &BB : *Result) {
    for (llvm::Instruction &I : BB) {
      checkInstruction(&I);
    }
  }

  return Result;
}

void LoweringInteger::checkWorllist() {
  while (!Worklist.empty()) {
    llvm::Instruction *I = Worklist.pop_back_val();
    llvm::Type *Ty = I->getType();
    const auto BitWidth = Ty->getIntegerBitWidth();
    for (unsigned int Lower = 128; Lower >= 64; Lower >>= 1) {
      unsigned int Higher = Lower << 1;
      if (BitWidth > Higher || Lower >= BitWidth) {
        continue;
      }
      if (BitWidth != Higher) {
        llvm::IRBuilder<> Builder(I);
        llvm::Type *HigherTy = Builder.getIntNTy(Higher);
        llvm::Value *LHS = nullptr;
        llvm::Value *RHS = nullptr;
        llvm::Value *OP = nullptr;
        llvm::Value *Trunc = nullptr;
        if (isSigned(I->getOpcode())) {
          LHS = Builder.CreateSExt(I->getOperand(0), HigherTy);
          RHS = Builder.CreateSExt(I->getOperand(1), HigherTy);
        } else {
          LHS = Builder.CreateZExt(I->getOperand(0), HigherTy);
          RHS = Builder.CreateZExt(I->getOperand(1), HigherTy);
        }
        switch (I->getOpcode()) {
        case llvm::Instruction::Mul:
          OP = Builder.CreateMul(LHS, RHS);
          break;
        case llvm::Instruction::Shl:
          OP = Builder.CreateShl(LHS, RHS);
          break;
        case llvm::Instruction::UDiv:
          OP = Builder.CreateUDiv(LHS, RHS);
          break;
        case llvm::Instruction::SDiv:
          OP = Builder.CreateSDiv(LHS, RHS);
          break;
        case llvm::Instruction::URem:
          OP = Builder.CreateURem(LHS, RHS);
          break;
        case llvm::Instruction::SRem:
          OP = Builder.CreateSRem(LHS, RHS);
          break;
        case llvm::Instruction::LShr:
          OP = Builder.CreateLShr(LHS, RHS);
          break;
        case llvm::Instruction::AShr:
          OP = Builder.CreateAShr(LHS, RHS);
          break;
        default:
          llvm_unreachable("unexcepted opcode");
        }
        Trunc = Builder.CreateTrunc(OP, Ty);
        I->replaceAllUsesWith(Trunc);
        I->dropAllReferences();
        I->eraseFromParent();
        I = llvm::dyn_cast<llvm::Instruction>(OP);
      }
      break;
    }
    llvm::IRBuilder<> Builder(I);
    llvm::Value *Result = nullptr;
    switch (I->getOpcode()) {
    case llvm::Instruction::Mul:
      Result = Builder.CreateCall(
          GetMulFunction(I->getType()->getIntegerBitWidth()),
          {I->getOperand(0), I->getOperand(1)}, I->getName());
      break;
    case llvm::Instruction::UDiv:
      Result = Builder.CreateCall(
          GetUDivFunction(I->getType()->getIntegerBitWidth()),
          {I->getOperand(0), I->getOperand(1)}, I->getName());
      break;
    case llvm::Instruction::URem:
      Result = Builder.CreateCall(
          GetURemFunction(I->getType()->getIntegerBitWidth()),
          {I->getOperand(0), I->getOperand(1)}, I->getName());
      break;
    case llvm::Instruction::Shl:
      Result = Builder.CreateCall(
          GetShlFunction(I->getType()->getIntegerBitWidth()),
          {I->getOperand(0), I->getOperand(1)}, I->getName());
      break;
    case llvm::Instruction::LShr:
      Result = Builder.CreateCall(
          GetLShrFunction(I->getType()->getIntegerBitWidth()),
          {I->getOperand(0), I->getOperand(1)}, I->getName());
      break;
    }
    if (Result) {
      I->replaceAllUsesWith(Result);
      I->dropAllReferences();
      I->eraseFromParent();
    }
  }
}

void LoweringInteger::checkInstruction(llvm::Instruction *I) {
  switch (I->getOpcode()) {
  case llvm::Instruction::Shl:
  case llvm::Instruction::LShr:
  case llvm::Instruction::AShr:
    if (llvm::dyn_cast<llvm::ConstantInt>(I->getOperand(1))) {
      return;
    }
    [[fallthrough]];
  case llvm::Instruction::Mul:
  case llvm::Instruction::UDiv:
  case llvm::Instruction::SDiv:
  case llvm::Instruction::URem:
  case llvm::Instruction::SRem: {
    llvm::Type *Ty = I->getType();
    if (!Ty->isIntegerTy() || 64 >= Ty->getIntegerBitWidth()) {
      return;
    }
    break;
  }
  default:
    return;
  }
  Worklist.push_back(I);
}

llvm::PreservedAnalyses LoweringInteger::run(llvm::Module &M,
                                             llvm::ModuleAnalysisManager &MAM) {
  TheModule = &M;
  for (llvm::Function &F : M) {
    /*
    if (F.getName() == "__bswapi256") {
      continue;
    }
    */
    for (llvm::BasicBlock &BB : F) {
      for (llvm::Instruction &I : BB) {
        checkInstruction(&I);
      }
    }
  }
  checkWorllist();
  TheModule = nullptr;
  return llvm::PreservedAnalyses::all();
}

} // namespace soll
