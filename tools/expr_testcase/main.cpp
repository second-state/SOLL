// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

using namespace soll;

int main(int argc, const char **argv) {
  using std::make_unique;
  SourceUnit source(make_unique_vector<Decl>(make_unique<ContractDecl>(
      "C", make_unique_vector<InheritanceSpecifier>(),
      make_unique_vector<Decl>(make_unique<FunctionDecl>(
          "add", Decl::Visibility::Public, StateMutability::Pure, false, false,
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
                      nullptr, "a", nullptr, Decl::Visibility::Default)),
                  nullptr),
              make_unique<DeclStmt>(
                  make_unique_vector<VarDecl>(make_unique<VarDecl>(
                      nullptr, "b", nullptr, Decl::Visibility::Default)),
                  nullptr),
              make_unique<DeclStmt>(
                  make_unique_vector<VarDecl>(make_unique<VarDecl>(
                      nullptr, "c", nullptr, Decl::Visibility::Default)),
                  nullptr),
              make_unique<DeclStmt>(
                  make_unique_vector<VarDecl>(make_unique<VarDecl>(
                      nullptr, "d", nullptr, Decl::Visibility::Default)),
                  nullptr),
              make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                          make_unique<Identifier>("b"),
                                          BinaryOperatorKind::BO_Add),
              make_unique<UnaryOperator>(make_unique<Identifier>("b"),
                                         UnaryOperatorKind::UO_Minus),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("b"),
                  make_unique<BinaryOperator>(
                      make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                  make_unique<Identifier>("d"),
                                                  BinaryOperatorKind::BO_Div),
                      make_unique<Identifier>("b"), BinaryOperatorKind::BO_Add),
                  BinaryOperatorKind::BO_Assign),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("c"),
                  make_unique<UnaryOperator>(make_unique<Identifier>("c"),
                                             UnaryOperatorKind::UO_Minus),
                  BinaryOperatorKind::BO_Assign),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("d"),
                  make_unique<BinaryOperator>(
                      make_unique<BinaryOperator>(
                          make_unique<BinaryOperator>(
                              make_unique<Identifier>("d"),
                              make_unique<Identifier>("a"),
                              BinaryOperatorKind::BO_Rem),
                          make_unique<NumberLiteral>(5),
                          BinaryOperatorKind::BO_Mul),
                      make_unique<NumberLiteral>(6),
                      BinaryOperatorKind::BO_Add),
                  BinaryOperatorKind::BO_Assign))))),
      nullptr, nullptr, ContractDecl::ContractKind::Contract)));

  ASTContext Ctx;
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(Ctx, source);

  llvm::LLVMContext Context;
  llvm::IntrusiveRefCntPtr<soll::DiagnosticOptions> Opts(new soll::DiagnosticOptions());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticIDs> DiagID(new soll::DiagnosticIDs());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticsEngine> Diags = new soll::DiagnosticsEngine(DiagID, Opts);
  soll::TargetOptions TO;
  soll::CodeGenerator *Gen = soll::CreateLLVMCodeGen(*Diags, "FuncBodyCGTest", Context, TO);
  soll::ASTContext ASTC;

  Gen->HandleSourceUnit(Ctx, source);
  Gen->getModule()->print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
