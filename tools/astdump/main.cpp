#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include <iostream>

using namespace soll;

int main(int argc, const char **argv) {
  std::vector<std::unique_ptr<Stmt>> stmts;
  stmts.emplace_back(std::make_unique<BinaryOperator>(
      std::make_unique<Identifier>("c"),
      std::make_unique<BinaryOperator>(std::make_unique<Identifier>("a"),
                                       std::make_unique<Identifier>("b"),
                                       BinaryOperatorKind::BO_Add),
      BinaryOperatorKind::BO_Assign));

  std::vector<std::unique_ptr<VarDecl>> params;
  params.emplace_back(std::make_unique<VarDecl>(
      "uint64", "a", std::make_unique<Identifier>("a")));
  params.emplace_back(std::make_unique<VarDecl>(
      "uint64", "b", std::make_unique<Identifier>("b")));

  std::vector<std::unique_ptr<VarDecl>> retval;
  retval.emplace_back(std::make_unique<VarDecl>(
      "uint64", "", std::make_unique<Identifier>("")));

  FunctionDecl func("add", Decl::Visibility::Public, StateMutability::Pure,
                    false, std::make_unique<ParamList>(std::move(params)),
                    std::vector<std::unique_ptr<ModifierInvocation>>(),
                    std::make_unique<ParamList>(std::move(retval)),
                    std::make_unique<Block>(std::move(stmts)));

  ASTPrinter p(std::cout);
  func.accept(p);

  return EXIT_SUCCESS;
}
