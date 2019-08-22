// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>
#include <memory>

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
  using std::make_shared;

  // should be shared ptr, waiting for merge request
  
  // mapping
  /*
  auto mapping = make_unique<Identifier>("map");
  auto key = make_unique<NumberLiteral>(87);
  auto val = make_unique<NumberLiteral>(7122);
  mapping->setType(make_shared<MappingType>(
      make_shared<IntegerType>(IntegerType::IntKind::U256),
      make_shared<IntegerType>(IntegerType::IntKind::I256)));
  key->setType(make_shared<IntegerType>(IntegerType::IntKind::U256));
  val->setType(make_shared<IntegerType>(IntegerType::IntKind::I256));
  */

  // fixed size memory array
  /*
  auto arr = make_unique<Identifier>("arr");
  auto idx = make_unique<NumberLiteral>(3);
  auto val = make_unique<NumberLiteral>(3);
  arr->setType(make_shared<ArrayType>(
    make_shared<IntegerType>(IntegerType::IntKind::I64),
    20,
    DataLocation::Memory
  ));
  idx->setType(make_shared<IntegerType>(IntegerType::IntKind::U256));
  val->setType(make_shared<IntegerType>(IntegerType::IntKind::I64));
  */

  // fixed size storage array
  /*
  auto arr = make_unique<Identifier>("arr");
  auto idx = make_unique<NumberLiteral>(3);
  arr->setType(make_shared<ArrayType>(
    make_shared<IntegerType>(IntegerType::IntKind::I64),
    20,
    DataLocation::Storage
  ));
  idx->setType(make_shared<IntegerType>(IntegerType::IntKind::U256));
  */

  // dynamic storage array
  auto arr = make_unique<Identifier>("arr");
  auto idx = make_unique<NumberLiteral>(3);
  auto val = make_unique<NumberLiteral>(7122);
  arr->setType(make_shared<ArrayType>(
      make_shared<IntegerType>(IntegerType::IntKind::I64),
      DataLocation::Storage));
  idx->setType(make_shared<IntegerType>(IntegerType::IntKind::U256));
  val->setType(make_shared<IntegerType>(IntegerType::IntKind::I64));

  SourceUnit source(make_unique_vector<Decl>(make_unique<ContractDecl>(
    "IndexAccess",
    make_unique_vector<InheritanceSpecifier>(),
    make_unique_vector<Decl>(
      make_unique<FunctionDecl>(
        "main",
        Decl::Visibility::Public,
        StateMutability::Pure,
        false,
        make_unique<ParamList>(make_unique_vector<VarDecl>()),
        make_unique_vector<ModifierInvocation>(),
        make_unique<ParamList>(make_unique_vector<VarDecl>()),
        make_unique<Block>(make_unique_vector<Stmt>(
          make_unique<BinaryOperator>(
            make_unique<IndexAccess>(
              std::move(arr),
              std::move(idx)
            ),
            std::move(val),
            BO_Assign
          )
        ))
      )
    ),
    ContractDecl::ContractKind::Contract
  )));

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, source);

  LLVMContext Context;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module);

  // revert
  llvm::FunctionType *FT = llvm::FunctionType::get(
    Builder.getVoidTy(),
    {Builder.getInt8PtrTy(), Builder.getInt32Ty()}, 
    false
  );
  llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "revert", Module);

  // keccak
  FT = llvm::FunctionType::get(
    Builder.getIntNTy(256),
    {Builder.getInt8PtrTy(), Builder.getIntNTy(256)},
    false
  );
  llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "keccak", Module);

  // storage store
  FT = llvm::FunctionType::get(
    Builder.getVoidTy(),
    {Builder.getIntNTy(256), Builder.getIntNTy(256)},
    false
  );
  llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "storageStore", Module);

  FBCG.compile(*static_cast<const FunctionDecl *>(
      static_cast<const ContractDecl *>(source.getNodes().front())
          ->getSubNodes()
          .front()));
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
