// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

using namespace soll;

int main(int argc, const char **argv) {
  using std::make_shared;
  using std::make_unique;

  auto I64 = make_shared<IntegerType>(IntegerType::IntKind::I64);
  auto I256 = make_shared<IntegerType>(IntegerType::IntKind::I256);
  auto U256 = make_shared<IntegerType>(IntegerType::IntKind::U256);

  // mapping
  /*
  auto MU256I256 = make_shared<MappingType>(U256, I256, DataLocation::Storage);
  auto Mapping = make_unique<VarDecl>(MU256I256, "map", nullptr,
                                      Decl::Visibility::Public, true);
  auto mapping = make_unique<Identifier>("map", Mapping.get());
  auto key = make_unique<NumberLiteral>(87, U256);
  auto val = make_unique<NumberLiteral>(7122, I256);
  */

  // fixed size memory array
  /*
  auto AI64_20 = make_shared<ArrayType>(I64, 20, DataLocation::Memory);
  auto Arr = make_unique<VarDecl>(AI64_20, "arr", nullptr,
                                  Decl::Visibility::Public, true);
  auto arr = make_unique<Identifier>("arr", Arr.get());
  auto idx = make_unique<NumberLiteral>(3, U256);
  auto val = make_unique<NumberLiteral>(3, I64);
  */

  // fixed size storage array
  /*
  auto AI64_20S = make_shared<ArrayType>(I64, 20, DataLocation::Storage);
  auto Arr = make_unique<VarDecl>(AI64_20S, "arr", nullptr,
                                  Decl::Visibility::Public, true);
  auto arr = make_unique<Identifier>("arr", Arr.get());
  auto idx = make_unique<NumberLiteral>(3, U256);
  */

  // dynamic storage array
  auto AI64S = make_shared<ArrayType>(I64, DataLocation::Storage);
  auto Arr = make_unique<VarDecl>(AI64S, "arr", nullptr,
                                  Decl::Visibility::Public, true);
  auto idx = make_unique<NumberLiteral>(3, U256);
  auto val = make_unique<NumberLiteral>(7122, I64);

  SourceUnit source(make_unique_vector<Decl>(make_unique<ContractDecl>(
      "IndexAccess", make_unique_vector<InheritanceSpecifier>(),
      make_unique_vector<Decl>(
          std::move(Arr),
          make_unique<FunctionDecl>(
              "main", Decl::Visibility::Public, StateMutability::Pure, false,
              false, make_unique<ParamList>(make_unique_vector<VarDecl>()),
              make_unique_vector<ModifierInvocation>(),
              make_unique<ParamList>(make_unique_vector<VarDecl>()),
              make_unique<Block>(
                  make_unique_vector<Stmt>(make_unique<BinaryOperator>(
                      make_unique<IndexAccess>(
                          make_unique<Identifier>("arr", Arr.get()),
                          std::move(idx), I64),
                      std::move(val), BO_Assign, I64))))),
      nullptr, nullptr, ContractDecl::ContractKind::Contract)));

  ASTContext Ctx;
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(Ctx, source);

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

  Gen->HandleSourceUnit(Ctx, source);
  Gen->getModule()->print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
