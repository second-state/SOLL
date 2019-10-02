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
  std::unique_ptr<SourceUnit> source;
  if (!strcmp(argv[1], "IfStmt")) { // IfStmt test
    source = make_unique<SourceUnit>(
        make_unique_vector<Decl>(make_unique<ContractDecl>(
            "C", make_unique_vector<InheritanceSpecifier>(),
            make_unique_vector<Decl>(make_unique<FunctionDecl>(
                "test", Decl::Visibility::Public, StateMutability::Pure, false,
                false,
                make_unique<ParamList>(make_unique_vector<VarDecl>(
                    make_unique<VarDecl>(
                        make_unique<IntegerType>(IntegerType::IntKind::I64),
                        "a", nullptr, Decl::Visibility::Default),
                    make_unique<VarDecl>(
                        make_unique<IntegerType>(IntegerType::IntKind::I64),
                        "b", nullptr, Decl::Visibility::Default))),
                make_unique_vector<ModifierInvocation>(),
                make_unique<ParamList>(
                    make_unique_vector<VarDecl>(make_unique<VarDecl>(
                        make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                        nullptr, Decl::Visibility::Default))),
                make_unique<Block>(make_unique_vector<Stmt>(make_unique<IfStmt>(
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(9),
                                                BinaryOperatorKind::BO_GT),
                    make_unique<Block>(make_unique_vector<Stmt>()),
                    make_unique<Block>(make_unique_vector<Stmt>(
                        make_unique<IfStmt>(
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a"),
                                make_unique<NumberLiteral>(11),
                                BinaryOperatorKind::BO_GT),
                            make_unique<Block>(make_unique_vector<Stmt>()),
                            make_unique<Block>(make_unique_vector<Stmt>())),
                        make_unique<BinaryOperator>(
                            make_unique<Identifier>("a"),
                            make_unique<Identifier>("b"),
                            BinaryOperatorKind::BO_Add)))))))),
            nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "WhileStmt")) { // WhileStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "a",
                    nullptr, Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                    nullptr, Decl::Visibility::Default))),
            make_unique<Block>(make_unique_vector<Stmt>(
                make_unique<WhileStmt>(
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(10),
                                                BinaryOperatorKind::BO_GT),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a"),
                            make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_Add))),
                    false),
                make_unique<WhileStmt>(
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(10),
                                                BinaryOperatorKind::BO_GT),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a"),
                            make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_Add))),
                    true),
                make_unique<BinaryOperator>(make_unique<NumberLiteral>(10),
                                            make_unique<Identifier>("a"),
                                            BinaryOperatorKind::BO_Add))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ForStmt")) { // ForStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "a",
                    nullptr, Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                    nullptr, Decl::Visibility::Default))),
            make_unique<Block>(make_unique_vector<Stmt>(
                make_unique<ForStmt>(
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(3),
                                                BinaryOperatorKind::BO_Add),
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(10),
                                                BinaryOperatorKind::BO_LT),
                    make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                                make_unique<NumberLiteral>(2),
                                                BinaryOperatorKind::BO_Sub),
                    make_unique<Block>(
                        make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                            make_unique<Identifier>("a"),
                            make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_Add)))),
                make_unique<BinaryOperator>(make_unique<NumberLiteral>(10),
                                            make_unique<Identifier>("a"),
                                            BinaryOperatorKind::BO_Add))))),
        nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ReturnStmt")) { // ReturnStmt test
    source = make_unique<SourceUnit>(
        make_unique_vector<Decl>(make_unique<ContractDecl>(
            "C", make_unique_vector<InheritanceSpecifier>(),
            make_unique_vector<Decl>(make_unique<FunctionDecl>(
                "test", Decl::Visibility::Public, StateMutability::Pure, false,
                false,
                make_unique<ParamList>(
                    make_unique_vector<VarDecl>(make_unique<VarDecl>(
                        make_unique<IntegerType>(IntegerType::IntKind::I64),
                        "a", nullptr, Decl::Visibility::Default))),
                make_unique_vector<ModifierInvocation>(),
                make_unique<ParamList>(
                    make_unique_vector<VarDecl>(make_unique<VarDecl>(
                        make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                        nullptr, Decl::Visibility::Default))),
                make_unique<Block>(
                    make_unique_vector<Stmt>(make_unique<ReturnStmt>(
                        make_unique<NumberLiteral>(7122)))))),
            nullptr, nullptr, ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "BreakStmt")) { // BreakStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            false,
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "a",
                    nullptr, Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                    nullptr, Decl::Visibility::Default))),
            make_unique<Block>(make_unique_vector<Stmt>(make_unique<WhileStmt>(
                make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                            make_unique<NumberLiteral>(1),
                                            BinaryOperatorKind::BO_GT),
                make_unique<Block>(make_unique_vector<Stmt>(
                    make_unique<IfStmt>(
                        make_unique<BinaryOperator>(
                            make_unique<NumberLiteral>(1),
                            make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_GT),
                        make_unique<Block>(make_unique_vector<Stmt>(
                            make_unique<IfStmt>(
                                make_unique<BinaryOperator>(
                                    make_unique<NumberLiteral>(3),
                                    make_unique<Identifier>("a"),
                                    BinaryOperatorKind::BO_GT),
                                make_unique<Block>(make_unique_vector<Stmt>(
                                    make_unique<BreakStmt>())),
                                nullptr),
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a"),
                                make_unique<NumberLiteral>(3),
                                BinaryOperatorKind::BO_Add),
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
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "a",
                    nullptr, Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(
                make_unique_vector<VarDecl>(make_unique<VarDecl>(
                    make_unique<IntegerType>(IntegerType::IntKind::I64), "",
                    nullptr, Decl::Visibility::Default))),
            make_unique<Block>(make_unique_vector<Stmt>(make_unique<WhileStmt>(
                make_unique<BinaryOperator>(make_unique<Identifier>("a"),
                                            make_unique<NumberLiteral>(1),
                                            BinaryOperatorKind::BO_GT),
                make_unique<Block>(make_unique_vector<Stmt>(
                    make_unique<IfStmt>(
                        make_unique<BinaryOperator>(
                            make_unique<NumberLiteral>(1),
                            make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_GT),
                        make_unique<Block>(make_unique_vector<Stmt>(
                            make_unique<IfStmt>(
                                make_unique<BinaryOperator>(
                                    make_unique<NumberLiteral>(3),
                                    make_unique<Identifier>("a"),
                                    BinaryOperatorKind::BO_GT),
                                make_unique<Block>(make_unique_vector<Stmt>(
                                    make_unique<ContinueStmt>())),
                                nullptr),
                            make_unique<BinaryOperator>(
                                make_unique<Identifier>("a"),
                                make_unique<NumberLiteral>(3),
                                BinaryOperatorKind::BO_Add),
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
  llvm::IntrusiveRefCntPtr<soll::DiagnosticOptions> Opts(new soll::DiagnosticOptions());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticIDs> DiagID(new soll::DiagnosticIDs());
  llvm::IntrusiveRefCntPtr<soll::DiagnosticsEngine> Diags = new soll::DiagnosticsEngine(DiagID, Opts);
  soll::TargetOptions TO;
  soll::CodeGenerator *Gen = soll::CreateLLVMCodeGen(*Diags, "FuncBodyCGTest", Context, TO);

  Gen->HandleSourceUnit(Ctx, *source);
  Gen->getModule()->print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
