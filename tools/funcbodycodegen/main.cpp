// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>

using namespace soll;

using llvm::IRBuilder;
using llvm::LLVMContext;
// using llvm::Value;
// using llvm::Type;
// using llvm::FunctionType;
// using llvm::ConstantInt;
// using llvm::Module;
// using llvm::Function;
// using llvm::BasicBlock;

int main(int argc, const char **argv) {
  using std::make_unique;

  SourceUnit source(make_unique_vector<Decl>(make_unique<ContractDecl>(
      "C", make_unique_vector<InheritanceSpecifier>(),
      make_unique_vector<Decl>(make_unique<FunctionDecl>(
          "add", Decl::Visibility::Public, StateMutability::Pure, false,
          make_unique<ParamList>(make_unique_vector<VarDecl>(
              make_unique<VarDecl>(
                  make_unique<IntegerType>(IntegerType::IntKind::I64), "a",
                  nullptr, Decl::Visibility::Default),
              make_unique<VarDecl>(
                  make_unique<IntegerType>(IntegerType::IntKind::I64), "b",
                  nullptr, Decl::Visibility::Default))),
          make_unique_vector<ModifierInvocation>(),
          make_unique<ParamList>(
              make_unique_vector<VarDecl>(make_unique<VarDecl>(
                  make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                  nullptr, Decl::Visibility::Default))),
          make_unique<Block>(make_unique_vector<Stmt>(
              make_unique<DeclStmt>(
                  make_unique_vector<VarDecl>(make_unique<VarDecl>(
                      nullptr, "c", nullptr, Decl::Visibility::Default)),
                  nullptr),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("c"),
                  make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                              make_unique<Identifier>("b"),
                                              BinaryOperatorKind::BO_Add),
                  BinaryOperatorKind::BO_Assign))))),
      ContractDecl::ContractKind::Contract)));

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, source);

  LLVMContext Context;
  soll::ASTContext ASTCtx;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module, ASTCtx);

  FBCG.compile(*static_cast<const FunctionDecl *>(
      static_cast<const ContractDecl *>(source.getNodes().front())
          ->getSubNodes()
          .front()));
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
