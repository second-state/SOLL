// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/NoFolder.h>

#include <map>
#include <unordered_map>

class FuncBodyCodeGen : public soll::ConstStmtVisitor {
  llvm::LLVMContext &Context;
  llvm::IRBuilder<llvm::NoFolder> &Builder;
  llvm::Module &Module;
  llvm::Function *CurFunc;

  llvm::BasicBlock *EndOfFunc;
  llvm::Value *RetVal;

  llvm::IntegerType *Int256Ty = nullptr;
  llvm::Type *VoidTy = nullptr;

  llvm::StructType *BytesTy = nullptr;
  llvm::StructType *StringTy = nullptr;

  llvm::ConstantInt *Zero256 = nullptr;
  llvm::ConstantInt *One256 = nullptr;

  soll::ASTContext &ASTCtx;

  // TODO: replace this temp impl
  // proper impl is like Decl* -> llvm::Value *
  // but it requires more consideration
  // LocalVarAddrTable stores the addresses NOT values
  std::unordered_map<std::string, llvm::Value *> LocalVarAddrTable;
  // TempValueTable stores temperary values
  // assume LValue will store address in TempValueTable
  // assume RValue will store value in TempValueTable
  std::unordered_map<const soll::Stmt *, llvm::Value *> TempValueTable;
  // TODO: replace this temp impl
  std::unordered_map<const soll::Stmt *, llvm::BasicBlock *> BasicBlockTable;

  // codegen LLVM IR in the visit functions
  void visit(BlockType &) override;
  void visit(IfStmtType &) override;
  void visit(WhileStmtType &) override;
  void visit(ForStmtType &) override;
  void visit(ContinueStmtType &) override;
  void visit(BreakStmtType &) override;
  void visit(ReturnStmtType &) override;
  void visit(DeclStmtType &) override;

  void visit(UnaryOperatorType &) override;
  void visit(BinaryOperatorType &) override;
  void visit(CallExprType &) override;
  void visit(ImplicitCastExprType &) override;
  void visit(ExplicitCastExprType &) override;
  void visit(ParenExprType &) override;
  void visit(IdentifierType &) override;
  void visit(IndexAccessType &) override;
  void visit(MemberExprType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;

  void emitCast(const soll::CastExpr &Cast);

  // create load instruction based on DataLocation
  llvm::Value *loadValue(const soll::Expr *ID);
  // create store instruction based on DataLocation
  void storeValue(const soll::Expr *Expr, llvm::Value *Val);

  // for mapping and dynamic storage array codegen
  // codegen function for concating {idx, base}
  void emitConcate(llvm::Value *, unsigned, unsigned, llvm::Value *,
                   llvm::Value *);

  // for array codegen
  // codegen function for checking whether array idx is out of bound
  void emitCheckArrayOutOfBound(llvm::Value *, llvm::Value *);

  // ast type -> llvm type ptr
  // currently support integer and integer memory array ONLY
  // TODO: add other types
  llvm::Type *getLLVMTy(const soll::Type *Ty) {
    if (auto *ArrTy = dynamic_cast<const soll::ArrayType *>(Ty)) {
      return llvm::ArrayType::get(getLLVMTy(ArrTy->getElementType().get()),
                                  ArrTy->getLength());
    } else {
      return Builder.getIntNTy(Ty->getBitNum());
    }
  }
  llvm::Type *getLLVMTy(const soll::VarDecl *VD) {
    return getLLVMTy(VD->GetType().get());
  }
  llvm::Type *getLLVMTy(const soll::FunctionDecl &FD) {
    auto *Ty = FD.getReturnParams()->getParams()[0]->GetType().get();
    return getLLVMTy(Ty);
  }

  llvm::Value *findLocalVarAddr(const std::string &S) {
    if (LocalVarAddrTable.count(S))
      return LocalVarAddrTable[S];
    else
      return nullptr;
  }

  llvm::Value *findTempValue(const soll::Stmt *S) {
    if (TempValueTable.count(S))
      return TempValueTable[S];
    else
      return nullptr;
  }

  llvm::BasicBlock *findBasicBlock(const soll::Stmt *S) {
    if (BasicBlockTable.count(S))
      return BasicBlockTable[S];
    else
      return nullptr;
  }

public:
  FuncBodyCodeGen(llvm::LLVMContext &Context,
                  llvm::IRBuilder<llvm::NoFolder> &Builder,
                  llvm::Module &Module, soll::ASTContext &Ctx);
  // codegen a certain function
  void compile(const soll::FunctionDecl &FD);
};
