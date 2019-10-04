// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/Frontend/ASTConsumers.h"
#include <cstring>
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

using namespace soll;

int main(int argc, const char **argv) {
  if (argc == 1)
    return 0;
  using std::make_unique;
  auto I64 = std::make_shared<IntegerType>(IntegerType::IntKind::I64);
  auto Bool = std::make_shared<BooleanType>();
  auto A = make_unique<VarDecl>(I64, "a", nullptr, Decl::Visibility::Default);
  auto B = make_unique<VarDecl>(I64, "b", nullptr, Decl::Visibility::Default);
  auto R = make_unique<VarDecl>(I64, "", nullptr, Decl::Visibility::Default);
  std::unique_ptr<SourceUnit> source;
  if (!strcmp(argv[1], "IfStmt")) { // IfStmt test
    source = make_unique<SourceUnit>(
        make_unique_vector<Decl>(make_unique<ContractDecl>(
            "C", make_unique_vector<InheritanceSpecifier>(),
            make_unique_vector<Decl>(make_unique<FunctionDecl>(
                "test", Decl::Visibility::Public, StateMutability::Pure, false,
                false,
                make_unique<ParamList>(
                    make_unique_vector<VarDecl>(std::move(A), std::move(B))),
                make_unique_vector<ModifierInvocation>(),
                make_unique<ParamList>(
                    make_unique_vector<VarDecl>(std::move(R))),
                make_unique<Block>(make_unique_vector<Stmt>(make_unique<IfStmt>(
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(9, I64),
                        BinaryOperatorKind::BO_GT, Bool),
                    make_unique<Block>(make_unique_vector<Stmt>()),
                    make_unique<Block>(make_unique_vector<Stmt>(
                        make_unique<IfStmt>(
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a", A.get()),
                                make_unique<NumberLiteral>(11, I64),
                                BinaryOperatorKind::BO_GT, Bool),
                            make_unique<Block>(make_unique_vector<Stmt>()),
                            make_unique<Block>(make_unique_vector<Stmt>())),
                        make_unique<BinaryOperator>(
                            make_unique<Identifier>("a", A.get()),
                            make_unique<Identifier>("b", B.get()),
                            BinaryOperatorKind::BO_Add, I64)))))))),
            nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "WhileStmt")) { // WhileStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(A))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(R))),
            make_unique<Block>(make_unique_vector<Stmt>(
                make_unique<WhileStmt>(
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(10, I64),
                        BinaryOperatorKind::BO_GT, Bool),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a", A.get()),
                            make_unique<Identifier>("a", A.get()),
                            BinaryOperatorKind::BO_Add, I64))),
                    false),
                make_unique<WhileStmt>(
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(10, I64),
                        BinaryOperatorKind::BO_GT, Bool),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a", A.get()),
                            make_unique<Identifier>("a", A.get()),
                            BinaryOperatorKind::BO_Add, I64))),
                    true),
                make_unique<BinaryOperator>(
                    make_unique<NumberLiteral>(10, I64),
                    make_unique<Identifier>("a", A.get()),
                    BinaryOperatorKind::BO_Add, I64))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ForStmt")) { // ForStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(A))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(R))),
            make_unique<Block>(make_unique_vector<Stmt>(
                make_unique<ForStmt>(
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(3, I64),
                        BinaryOperatorKind::BO_Add, I64),
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(10, I64),
                        BinaryOperatorKind::BO_LT, Bool),
                    make_unique<BinaryOperator>(
                        make_unique<Identifier>("a", A.get()),
                        make_unique<NumberLiteral>(2, I64),
                        BinaryOperatorKind::BO_Sub, I64),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a", A.get()),
                            make_unique<Identifier>("a", A.get()),
                            BinaryOperatorKind::BO_Add, I64)))),
                make_unique<BinaryOperator>(
                    make_unique<NumberLiteral>(10, I64),
                    make_unique<Identifier>("a", A.get()),
                    BinaryOperatorKind::BO_Add, I64))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ReturnStmt")) { // ReturnStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(A))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(R))),
            make_unique<Block>(make_unique_vector<Stmt>(make_unique<ReturnStmt>(
                make_unique<NumberLiteral>(7122, I64)))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "BreakStmt")) { // BreakStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(A))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(R))),
            make_unique<Block>(make_unique_vector<Stmt>(make_unique<WhileStmt>(
                make_unique<BinaryOperator>(
                    make_unique<Identifier>("a", A.get()),
                    make_unique<NumberLiteral>(1, I64),
                    BinaryOperatorKind::BO_GT, Bool),
                make_unique<Block>(make_unique_vector<Stmt>(
                    make_unique<IfStmt>(
                        make_unique<BinaryOperator>(
                            make_unique<NumberLiteral>(1, I64),
                            make_unique<Identifier>("a", A.get()),
                            BinaryOperatorKind::BO_GT, Bool),
                        make_unique<Block>(make_unique_vector<Stmt>(
                            make_unique<IfStmt>(
                                make_unique<BinaryOperator>(
                                    make_unique<NumberLiteral>(3, I64),
                                    make_unique<Identifier>("a", A.get()),
                                    BinaryOperatorKind::BO_GT, Bool),
                                make_unique<Block>(make_unique_vector<Stmt>(
                                    make_unique<BreakStmt>())),
                                nullptr),
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a", A.get()),
                                make_unique<NumberLiteral>(3, I64),
                                BinaryOperatorKind::BO_Add, I64),
                            make_unique<BreakStmt>())),
                        nullptr),
                    make_unique<BreakStmt>())),
                false))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ContinueStmt")) { // ContinueStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(A))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(std::move(R))),
            make_unique<Block>(make_unique_vector<Stmt>(make_unique<WhileStmt>(
                make_unique<BinaryOperator>(
                    make_unique<Identifier>("a", A.get()),
                    make_unique<NumberLiteral>(1, I64),
                    BinaryOperatorKind::BO_GT, Bool),
                make_unique<Block>(make_unique_vector<Stmt>(
                    make_unique<IfStmt>(
                        make_unique<BinaryOperator>(
                            make_unique<NumberLiteral>(1, I64),
                            make_unique<Identifier>("a", A.get()),
                            BinaryOperatorKind::BO_GT, Bool),
                        make_unique<Block>(make_unique_vector<Stmt>(
                            make_unique<IfStmt>(
                                make_unique<BinaryOperator>(
                                    make_unique<NumberLiteral>(3, I64),
                                    make_unique<Identifier>("a", A.get()),
                                    BinaryOperatorKind::BO_GT, Bool),
                                make_unique<Block>(make_unique_vector<Stmt>(
                                    make_unique<ContinueStmt>())),
                                nullptr),
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a", A.get()),
                                make_unique<NumberLiteral>(3, I64),
                                BinaryOperatorKind::BO_Add, I64),
                            make_unique<ContinueStmt>())),
                        nullptr),
                    make_unique<ContinueStmt>())),
                false))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else
    return 0;

  ASTContext Ctx;
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(Ctx, *source);

  llvm::LLVMContext Context;
  llvm::IntrusiveRefCntPtr<soll::DiagnosticOptions> Opts(
      new soll::DiagnosticOptions());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticIDs> DiagID(
      new soll::DiagnosticIDs());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticsEngine> Diags =
      new soll::DiagnosticsEngine(DiagID, Opts);
  soll::TargetOptions TO;
  std::unique_ptr<soll::CodeGenerator> Gen(
      soll::CreateLLVMCodeGen(*Diags, "FuncBodyCGTest", Context, TO));
  soll::ASTContext ASTC;
  Gen->Initialize(ASTC);

  Gen->HandleSourceUnit(Ctx, *source);
  Gen->getModule()->print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
