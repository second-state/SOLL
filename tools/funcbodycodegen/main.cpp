#include "soll/AST/ASTPrinter.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include <iostream>

using namespace soll;

using llvm::LLVMContext;
using llvm::IRBuilder;
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

  std::vector<std::unique_ptr<VarDecl>> params;

  params.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "a", nullptr, Decl::Visibility::Default));
  params.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "b", nullptr, Decl::Visibility::Default));

  std::vector<std::unique_ptr<VarDecl>> retval;

  retval.emplace_back(std::make_unique<VarDecl>(
      std::make_unique<Type>(), "", nullptr, Decl::Visibility::Default));

  FunctionDecl func("add", Decl::Visibility::Public, StateMutability::Pure,
                    false, std::make_unique<ParamList>(std::move(params)),
                    std::vector<std::unique_ptr<ModifierInvocation>>(),
                    std::make_unique<ParamList>(std::move(retval)),
                    std::make_unique<Block>(std::move(stmts)));

  ASTPrinter p(std::cout);
  func.accept(p);

  LLVMContext Context;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module);

  FBCG.compile(func);
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
