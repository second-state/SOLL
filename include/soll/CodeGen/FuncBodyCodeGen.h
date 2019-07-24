#pragma once

#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/NoFolder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

class FuncBodyCodeGen : public StmtVisitor {
  llvm::LLVMContext &Context;
  llvm::IRBuilder<llvm::NoFolder> &Builder;
  llvm::Module &Module;

  // TODO: codegen in visit functions
  // void visit(const Block &) override;
  // ...
public:

  FuncBodyCodeGen(llvm::LLVMContext &Context,
                  llvm::IRBuilder<llvm::NoFolder> &Builder,
                  llvm::Module &Module): 
                    Context(Context), Builder(Builder),
                    Module(Module) {}
  // codegen a certain function
  void compile(const FunctionDecl &);
};
