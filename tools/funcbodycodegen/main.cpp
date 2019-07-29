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

  std::vector<std::unique_ptr<Stmt>> stmts;

  std::vector<VarDeclPtr> VarDecs;
  // VarDecl VarDec(nullptr, "c", nullptr, Decl::Visibility::Default);
  VarDecs.emplace_back(std::make_unique<VarDecl>(nullptr, "c", nullptr,
                                                 Decl::Visibility::Default));
  stmts.emplace_back(std::make_unique<DeclStmt>(std::move(VarDecs), nullptr));

  stmts.emplace_back(std::make_unique<BinaryOperator>(
      std::make_unique<Identifier>("c"),
      std::make_unique<BinaryOperator>(std::make_unique<Identifier>("a"),
                                       std::make_unique<Identifier>("b"),
                                       BinaryOperatorKind::BO_Add),
      BinaryOperatorKind::BO_Assign));

  stmts.emplace_back(std::make_unique<CallExpr>(std::make_unique<Identifier>("require"),
                      container<std::vector<std::unique_ptr<Expr>>>::init(
                       {std::make_unique<BinaryOperator>(
                            std::make_unique<Identifier>("c"),
                            std::make_unique<Identifier>("a"),
                            BinaryOperatorKind::BO_GE),
                        std::make_unique<StringLiteral>(
                            "SafeMath: addition overflow")})));
  
  std::vector<std::unique_ptr<VarDecl>> params;

  params.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "a", nullptr, Decl::Visibility::Default));
  params.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "b", nullptr, Decl::Visibility::Default));

  std::vector<std::unique_ptr<VarDecl>> retval;

  retval.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "", nullptr, Decl::Visibility::Default));

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, source);

  LLVMContext Context;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module);

  FBCG.compile(*static_cast<const FunctionDecl *>(
      static_cast<const ContractDecl *>(source.getNodes().front())
          ->getSubNodes()
          .front()));
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
