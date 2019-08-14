// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/Frontend/ASTConsumers.h"
#include "soll/Sema/BreakableVisitor.h"
#include <cstring>
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
                make_unique<ParamList>(make_unique_vector<VarDecl>(
                    make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                         Decl::Visibility::Default),
                    make_unique<VarDecl>(make_unique<Type>(), "b", nullptr,
                                         Decl::Visibility::Default))),
                make_unique_vector<ModifierInvocation>(),
                make_unique<ParamList>(make_unique_vector<VarDecl>(
                    make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                         Decl::Visibility::Default))),
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
            ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "WhileStmt")) { // WhileStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                     Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                     Decl::Visibility::Default))),
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
        ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ForStmt")) { // ForStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                     Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                     Decl::Visibility::Default))),
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
        ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ReturnStmt")) { // ReturnStmt test
    source = make_unique<SourceUnit>(
        make_unique_vector<Decl>(make_unique<ContractDecl>(
            "C", make_unique_vector<InheritanceSpecifier>(),
            make_unique_vector<Decl>(make_unique<FunctionDecl>(
                "test", Decl::Visibility::Public, StateMutability::Pure, false,
                make_unique<ParamList>(make_unique_vector<VarDecl>(
                    make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                         Decl::Visibility::Default))),
                make_unique_vector<ModifierInvocation>(),
                make_unique<ParamList>(make_unique_vector<VarDecl>(
                    make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                         Decl::Visibility::Default))),
                make_unique<Block>(
                    make_unique_vector<Stmt>(make_unique<ReturnStmt>(
                        make_unique<NumberLiteral>(7122)))))),
            ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "BreakStmt")) { // BreakStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                     Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                     Decl::Visibility::Default))),
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
        ContractDecl::ContractKind::Contract)));
  } else if (!strcmp(argv[1], "ContinueStmt")) { // ContinueStmt test
    source = make_unique<
        SourceUnit>(make_unique_vector<Decl>(make_unique<ContractDecl>(
        "C", make_unique_vector<InheritanceSpecifier>(),
        make_unique_vector<Decl>(make_unique<FunctionDecl>(
            "test", Decl::Visibility::Public, StateMutability::Pure, false,
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "a", nullptr,
                                     Decl::Visibility::Default))),
            make_unique_vector<ModifierInvocation>(),
            make_unique<ParamList>(make_unique_vector<VarDecl>(
                make_unique<VarDecl>(make_unique<Type>(), "", nullptr,
                                     Decl::Visibility::Default))),
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
        ContractDecl::ContractKind::Contract)));
  } else
    return 0;

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, *source);

  LLVMContext Context;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module);
  BreakableVisitor BV;

  BV.check(*static_cast<FunctionDecl *>(
      static_cast<ContractDecl *>(source->getNodes().front())
          ->getSubNodes()
          .front()));
  FBCG.compile(*static_cast<const FunctionDecl *>(
      static_cast<const ContractDecl *>(source->getNodes().front())
          ->getSubNodes()
          .front()));
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
