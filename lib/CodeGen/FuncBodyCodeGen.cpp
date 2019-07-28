#include "soll/CodeGen/FuncBodyCodeGen.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>

#include <iostream>

using namespace soll;
using llvm::FunctionType;
using llvm::Function;
using llvm::BasicBlock;

void FuncBodyCodeGen::compile(const soll::FunctionDecl &FD) {
  // TODO: replace this temp impl
  // this impl assumes type of functionDecl params is uint64
  
  auto PsSol = FD.getParams()->getParams();
  std::vector<llvm::Type *> Tys;
  for(int i = 0; i < PsSol.size(); i++)
    Tys.push_back(llvm::Type::getInt64Ty(Context));
  llvm::ArrayRef<llvm::Type *> ParamTys(&Tys[0], Tys.size());
  FunctionType *FT = FunctionType::get(llvm::Type::getVoidTy(Context), ParamTys, false);
	CurFunc = Function::Create(FT, Function::ExternalLinkage, FD.getName(), &Module);
  
  auto PsLLVM = CurFunc->arg_begin();
  for(int i = 0; i < PsSol.size(); i++) {
    llvm::Value *P = PsLLVM++;
    P->setName(PsSol[i]->getName());
    ParamTable[P->getName()] = P;
  }
  
  FD.getBody()->accept(*this);
}

void FuncBodyCodeGen::visit(BlockType &B) {
  // TODO: the following is just temp demo
  BasicBlock *BB = BasicBlock::Create(Context, "entry", CurFunc);
	Builder.SetInsertPoint(BB);
  
  ConstStmtVisitor::visit(B);
}

void FuncBodyCodeGen::visit(IfStmtType &) {
  // TODO
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

void FuncBodyCodeGen::visit(ReturnStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(DeclStmtType &DS) {
  // TODO: replace this temp impl
  // this impl assumes declared variables are uint64
  for (auto &D : DS.getDecls()){
    auto *p = Builder.CreateAlloca(llvm::Type::getInt64Ty(Context), nullptr, D->getName()+"_addr");
    LocalVarAddrTable[D->getName()] = p;
  }
}

void FuncBodyCodeGen::visit(UnaryOperatorType &) {
  // TODO
}

void FuncBodyCodeGen::visit(BinaryOperatorType &BO) {
  // return;
  llvm::Value *V = nullptr;
  if (BO.isAssignmentOp()) {
    // TODO: replace this temp impl
    // because we havn't properly dealed with lvalue/rvalue
    // This impl assumes:
    //   lhs of assignment operator (=, +=, -=, ...) is a Identifier,
    //   and its type is uint64
    
    BO.getRHS()->accept(*this);
    if (auto ID = dynamic_cast<const Identifier *>(BO.getLHS())) {
      if (auto Addr = findLocalVarAddr(ID->getName())) {
        auto *Val = findTempValue(BO.getRHS());
        if (Val==nullptr) while(1);
        Builder.CreateStore(Val, Addr);
      }
    }
  }

  if (BO.isAdditiveOp()) {
    BO.getLHS()->accept(*this);
    BO.getRHS()->accept(*this);
    V = Builder.CreateAdd(findTempValue(BO.getLHS()), findTempValue(BO.getRHS()), "BO_ADD");
  }

  TempValueTable[&BO] = V;
}

void FuncBodyCodeGen::visit(CallExprType &) {
  // TODO
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
    V = Builder.getInt64(7122);
  }
  
  TempValueTable[&ID] = V;
}

void FuncBodyCodeGen::visit(BooleanLiteralType &) {
  // TODO
}

void FuncBodyCodeGen::visit(StringLiteralType &) {
  // TODO
}

void FuncBodyCodeGen::visit(NumberLiteralType &) {
  // TODO
}
