#include "soll/CodeGen/FuncBodyCodeGen.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

#include <iostream>

using namespace soll;
using llvm::BasicBlock;
using llvm::Function;
using llvm::FunctionType;
using llvm::Value;

void FuncBodyCodeGen::compile(const soll::FunctionDecl &FD) {
  // TODO: replace this temp impl
  // this impl assumes type of functionDecl params and return is uint64
  auto PsSol = FD.getParams()->getParams();
  std::vector<llvm::Type *> Tys;
  for(int i = 0; i < PsSol.size(); i++)
    Tys.push_back(Builder.getInt64Ty());
  llvm::ArrayRef<llvm::Type *> ParamTys(&Tys[0], Tys.size());
  FunctionType *FT = FunctionType::get(Builder.getInt64Ty(), ParamTys, false);
	CurFunc = Function::Create(FT, Function::ExternalLinkage, FD.getName(), &Module);

  BasicBlock *BB = BasicBlock::Create(Context, "entry", CurFunc);
  Builder.SetInsertPoint(BB);

  auto PsLLVM = CurFunc->arg_begin();
  for (int i = 0; i < PsSol.size(); i++) {
    llvm::Value *P = PsLLVM++;
    P->setName(PsSol[i]->getName());
    ParamTable[P->getName()] = P;
  }

  FD.getBody()->accept(*this);
}

void FuncBodyCodeGen::visit(BlockType &B) {
  ConstStmtVisitor::visit(B);
}

void FuncBodyCodeGen::visit(IfStmtType & IF) {
  BasicBlock *ThenBB = BasicBlock::Create(Context, "then", CurFunc);
  BasicBlock *ElseBB = BasicBlock::Create(Context, "else", CurFunc);
  BasicBlock *ContBB = BasicBlock::Create(Context, "ifcont", CurFunc);

  IF.getCond()->accept(*this);
  Value *CondV = findTempValue(IF.getCond());
  Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  Builder.SetInsertPoint(ThenBB);
  IF.getThen()->accept(*this);
  Builder.CreateBr(ContBB);

  Builder.SetInsertPoint(ElseBB);
  if (IF.getElse() != nullptr){
    Builder.SetInsertPoint(ElseBB);
    IF.getElse()->accept(*this);
  }
  Builder.CreateBr(ContBB);
  Builder.SetInsertPoint(ContBB);
}

void FuncBodyCodeGen::visit(ForStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(ContinueStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(BreakStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(ReturnStmtType &RS) {
  // TODO: replace this
  // this impl assumes return type is uint64
  RS.getRetValue()->accept(*this);
  Builder.CreateRet(findTempValue(RS.getRetValue()));
}

void FuncBodyCodeGen::visit(DeclStmtType &DS) {
  // TODO: replace this temp impl
  // this impl assumes declared variables are uint64
  for (auto &D : DS.getVarDecls()) {
    auto *p = Builder.CreateAlloca(llvm::Type::getInt64Ty(Context), nullptr,
                                   D->getName() + "_addr");
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

void FuncBodyCodeGen::visit(UnaryOperatorType &) {
  // TODO
}

void FuncBodyCodeGen::visit(BinaryOperatorType &BO) {
  // TODO: replace this temp impl (visit(BinaryOperatorType &BO))
  // This impl assumes:
  //   every type is uint64
  llvm::Value *V = nullptr;
  if (BO.isAssignmentOp()) {
    // TODO: replace this temp impl
    // because we havn't properly dealed with lvalue/rvalue
    // This impl assumes:
    //   lhs of assignment operator (=, +=, -=, ...) is a Identifier,

    BO.getRHS()->accept(*this);
    if (auto ID = dynamic_cast<const Identifier *>(BO.getLHS())) {
      if (auto Addr = findLocalVarAddr(ID->getName())) {
        auto *Val = findTempValue(BO.getRHS());
        Builder.CreateStore(Val, Addr);
      }
    }
  }

  if (BO.isAdditiveOp() || BO.isMultiplicativeOp() || BO.isComparisonOp()) {
    BO.getLHS()->accept(*this);
    BO.getRHS()->accept(*this);

    switch (BO.getOpcode()) {
    case BinaryOperatorKind::BO_Add:
      V = Builder.CreateAdd(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_ADD");
      break;
    case BinaryOperatorKind::BO_Sub:
      V = Builder.CreateSub(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_SUB");
      break;
    case BinaryOperatorKind::BO_Mul:
      V = Builder.CreateMul(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_MUL");
      break;
    case BinaryOperatorKind::BO_Div:
      V = Builder.CreateUDiv(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_DIV");
      break;
    case BinaryOperatorKind::BO_GE:
      V = Builder.CreateICmpUGE(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_UGE");
      break;
    case BinaryOperatorKind::BO_GT:
      V = Builder.CreateICmpUGT(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_UGT");
      break;
    case BinaryOperatorKind::BO_LE:
      V = Builder.CreateICmpULE(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_ULE");
      break;
    case BinaryOperatorKind::BO_LT:
      V = Builder.CreateICmpULT(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_ULT");
      break;
    case BinaryOperatorKind::BO_EQ:
      V = Builder.CreateICmpEQ(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_EQ");
      break;
    case BinaryOperatorKind::BO_NE:
      V = Builder.CreateICmpNE(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_NE");
      break;
    // TODO: other operators
    default:
      ;
    }
  }
  
  TempValueTable[&BO] = V;
}

void FuncBodyCodeGen::visit(CallExprType &CALL) {
  auto Name = dynamic_cast<const Identifier *>(CALL.getCalleeExpr())->getName();
  if (Name.compare("require") == 0) {
    auto Arguments = CALL.getArguments();
    Arguments[0]->accept(*this);
    Value *CondV = findTempValue(Arguments[0]);
    Arguments[1]->accept(*this);
    Value *StrValue = findTempValue(Arguments[1]);
    Value *Length = llvm::ConstantInt::get(Context, llvm::APInt(32, dynamic_cast<const StringLiteral *>(Arguments[1])->getValue().length() + 1, true));
    BasicBlock *RevertBB = BasicBlock::Create(Context, "revert", CurFunc);
    BasicBlock *ContBB = BasicBlock::Create(Context, "continue", CurFunc);

    Builder.CreateCondBr(CondV, ContBB, RevertBB);
    Builder.SetInsertPoint(RevertBB);

    // revert
    std::vector<llvm::Type *> ArgsType;
    ArgsType.push_back(llvm::Type::getInt8PtrTy(Context));
    ArgsType.push_back(llvm::Type::getInt32Ty(Context));
    FunctionType *FT = FunctionType::get(llvm::Type::getVoidTy(Context), ArgsType, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "revert", Module);

    std::vector<Value *> ArgsV;
    ArgsV.push_back(StrValue);
    ArgsV.push_back(Length);
    Builder.CreateCall(F, ArgsV, "calltmp");
    Builder.CreateUnreachable();

    Builder.SetInsertPoint(ContBB);
  }
}

void FuncBodyCodeGen::visit(IdentifierType &ID) {
  // TODO: replace this temp impl
  // this impl assumes visited Identifier is rvalue
  llvm::Value *V;
  if (llvm::Value *Addr = findLocalVarAddr(ID.getName()))
    V = Builder.CreateLoad(llvm::Type::getInt64Ty(Context), Addr, ID.getName());
  else if (llvm::Value *Val = findParam(ID.getName()))
    V = Val;
  else {
    V = Builder.getInt64(7122); // TODO: replace this
  }

  TempValueTable[&ID] = V;
}

void FuncBodyCodeGen::visit(BooleanLiteralType &) {
  // TODO
}

void FuncBodyCodeGen::visit(StringLiteralType &SL) {
  TempValueTable[&SL] = Builder.CreateGlobalString(SL.getValue(),"str");
}

void FuncBodyCodeGen::visit(NumberLiteralType &NL) {
  TempValueTable[&NL] = Builder.getInt64(NL.getValue());
}
