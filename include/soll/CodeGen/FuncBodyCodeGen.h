// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/NoFolder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>

#include <map>
#include <unordered_map>

class FuncBodyCodeGen : public soll::ConstStmtVisitor {
  llvm::LLVMContext &Context;
  llvm::IRBuilder<llvm::NoFolder> &Builder;
  llvm::Module &Module;
  llvm::Function *CurFunc;
  // TODO: replace this temp impl
  // proper impl is like Decl* -> llvm::Value *
  // but it requires more consideration
  // ParamTable stores the values
  std::unordered_map<std::string, llvm::Value *> ParamTable;
  // LocalVarAddrTable stores the addresses NOT values
  std::unordered_map<std::string, llvm::Value *> LocalVarAddrTable;
  // TempValueTable stores temperary values
  std::unordered_map<const soll::Stmt *, llvm::Value *> TempValueTable;
  // codegen LLVM IR in the visit functions
  void visit(BlockType &) override;
  void visit(IfStmtType &) override;
  void visit(ForStmtType &) override;
  void visit(ContinueStmtType &) override;
  void visit(BreakStmtType &) override;
  void visit(ReturnStmtType &) override;
  void visit(DeclStmtType &) override;

  void visit(UnaryOperatorType &) override;
  void visit(BinaryOperatorType &) override;
  void visit(CallExprType &) override;
  void visit(IdentifierType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;

  llvm::Value *findParam(const std::string &S) {
    if (ParamTable.count(S))
      return ParamTable[S];
    else
      return nullptr;
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

public:

  FuncBodyCodeGen(llvm::LLVMContext &Context,
                  llvm::IRBuilder<llvm::NoFolder> &Builder,
                  llvm::Module &Module): 
                    Context(Context), Builder(Builder),
                    Module(Module) {}
  // codegen a certain function
  void compile(const soll::FunctionDecl &);
};
