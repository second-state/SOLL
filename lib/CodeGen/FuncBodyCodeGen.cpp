#include "soll/CodeGen/FuncBodyCodeGen.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>

using namespace soll;
using llvm::FunctionType;
using llvm::Function;
using llvm::BasicBlock;

void FuncBodyCodeGen::compile(const soll::FunctionDecl &FD) {
  // TODO: parse FuncDecl and generate correct FunctionType
  FunctionType *FT = FunctionType::get(llvm::Type::getVoidTy(Context), false);
	CurFunc = Function::Create(FT, Function::ExternalLinkage, "Fun", &Module);
  visit(*FD.getBody());
}

void FuncBodyCodeGen::visit(BlockType &) {
  // TODO: the following is just temp demo
  BasicBlock *BB = BasicBlock::Create(Context, "entry", CurFunc);
	Builder.SetInsertPoint(BB);
  
	llvm::Value *PA = Builder.CreateAlloca(llvm::Type::getIntNTy(Context, 64), nullptr, "pa");
	llvm::Value *A = Builder.CreateLoad(PA, "a");
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

void FuncBodyCodeGen::visit(ExprStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(DeclStmtType &) {
  // TODO
}

void FuncBodyCodeGen::visit(UnaryOperatorType &) {
  // TODO
}

void FuncBodyCodeGen::visit(BinaryOperatorType &) {
  // TODO
}

void FuncBodyCodeGen::visit(CallExprType &) {
  // TODO
}

void FuncBodyCodeGen::visit(IdentifierType &) {
  // TODO
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
