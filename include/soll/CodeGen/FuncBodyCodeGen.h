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

class FuncBodyCodeGen : public soll::ConstStmtVisitor {
  llvm::LLVMContext &Context;
  llvm::IRBuilder<llvm::NoFolder> &Builder;
  llvm::Module &Module;
  llvm::Function *CurFunc;
  std::map<soll::Decl *, llvm::Value *> LocalVarTable;
  // codegen LLVM IR in the visit functions
  void visit(BlockType &) override;
  void visit(IfStmtType &) override;
  void visit(ForStmtType &) override;
  void visit(ContinueStmtType &) override;
  void visit(BreakStmtType &) override;
  void visit(ReturnStmtType &) override;
  void visit(ExprStmtType &) override;
  void visit(DeclStmtType &) override;
  void visit(UnaryOperatorType &) override;
  void visit(BinaryOperatorType &) override;
  void visit(CallExprType &) override;
  void visit(IdentifierType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;
public:

  FuncBodyCodeGen(llvm::LLVMContext &Context,
                  llvm::IRBuilder<llvm::NoFolder> &Builder,
                  llvm::Module &Module): 
                    Context(Context), Builder(Builder),
                    Module(Module) {}
  // codegen a certain function
  void compile(const soll::FunctionDecl &);
};
