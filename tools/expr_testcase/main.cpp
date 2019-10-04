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
  auto I64 = std::make_shared<IntegerType>(IntegerType::IntKind::I64);
  auto A = make_unique<VarDecl>(I64, "a", nullptr, Decl::Visibility::Default);
  auto B = make_unique<VarDecl>(I64, "b", nullptr, Decl::Visibility::Default);
  auto C = make_unique<VarDecl>(I64, "c", nullptr, Decl::Visibility::Default);
  auto D = make_unique<VarDecl>(I64, "d", nullptr, Decl::Visibility::Default);
  SourceUnit source(make_unique_vector<Decl>(make_unique<ContractDecl>(
      "C", make_unique_vector<InheritanceSpecifier>(),
      make_unique_vector<Decl>(make_unique<FunctionDecl>(
          "add", Decl::Visibility::Public, StateMutability::Pure, false, false,
          make_unique<ParamList>(make_unique_vector<VarDecl>()),
          make_unique_vector<ModifierInvocation>(),
          make_unique<ParamList>(make_unique_vector<VarDecl>()),

          make_unique<Block>(make_unique_vector<Stmt>(
              make_unique<DeclStmt>(make_unique_vector<VarDecl>(std::move(A)),
                                    nullptr),
              make_unique<DeclStmt>(make_unique_vector<VarDecl>(std::move(B)),
                                    nullptr),
              make_unique<DeclStmt>(make_unique_vector<VarDecl>(std::move(C)),
                                    nullptr),
              make_unique<DeclStmt>(make_unique_vector<VarDecl>(std::move(D)),
                                    nullptr),
              make_unique<BinaryOperator>(make_unique<Identifier>("a", A.get()),
                                          make_unique<Identifier>("b", B.get()),
                                          BinaryOperatorKind::BO_Add, I64),
              make_unique<UnaryOperator>(make_unique<Identifier>("b", B.get()),
                                         UnaryOperatorKind::UO_Minus, I64),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("b", B.get()),
                  make_unique<BinaryOperator>(
                      make_unique<BinaryOperator>(
                          make_unique<Identifier>("a", A.get()),
                          make_unique<Identifier>("d", D.get()),
                          BinaryOperatorKind::BO_Div, I64),
                      make_unique<Identifier>("b", B.get()),
                      BinaryOperatorKind::BO_Add, I64),
                  BinaryOperatorKind::BO_Assign, I64),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("c", C.get()),
                  make_unique<UnaryOperator>(
                      make_unique<Identifier>("c", C.get()),
                      UnaryOperatorKind::UO_Minus, I64),
                  BinaryOperatorKind::BO_Assign, I64),
              make_unique<BinaryOperator>(
                  make_unique<Identifier>("d", D.get()),
                  make_unique<BinaryOperator>(
                      make_unique<BinaryOperator>(
                          make_unique<BinaryOperator>(
                              make_unique<Identifier>("d", D.get()),
                              make_unique<Identifier>("a", A.get()),
                              BinaryOperatorKind::BO_Rem, I64),
                          make_unique<NumberLiteral>(5, I64),
                          BinaryOperatorKind::BO_Mul, I64),
                      make_unique<NumberLiteral>(6, I64),
                      BinaryOperatorKind::BO_Add, I64),
                  BinaryOperatorKind::BO_Assign, I64))))),
      nullptr, nullptr, ContractDecl::ContractKind::Contract)));

  ASTContext Ctx;
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(Ctx, source);

  llvm::LLVMContext Context;
  llvm::IntrusiveRefCntPtr<soll::DiagnosticOptions> Opts(new soll::DiagnosticOptions());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticIDs> DiagID(new soll::DiagnosticIDs());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticsEngine> Diags = new soll::DiagnosticsEngine(DiagID, Opts);
  soll::TargetOptions TO;
  std::unique_ptr<soll::CodeGenerator> Gen(
      soll::CreateLLVMCodeGen(*Diags, "FuncBodyCGTest", Context, TO));
  soll::ASTContext ASTC;
  Gen->Initialize(ASTC);

  Gen->HandleSourceUnit(Ctx, source);
  Gen->getModule()->print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
