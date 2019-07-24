#include "soll/AST/ASTPrinter.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include <iostream>

using namespace soll;

int main(int argc, const char **argv) {
  FunctionDecl func(
      "add", Decl::Visibility::Public, StateMutability::Pure, false,
      std::make_unique<ParamList>(
          std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init(
              {std::make_unique<VarDecl>(std::make_unique<Type>(), "a", nullptr,
                                         Decl::Visibility::Default),
               std::make_unique<VarDecl>(std::make_unique<Type>(), "b", nullptr,
                                         Decl::Visibility::Default)}))),
      std::vector<std::unique_ptr<ModifierInvocation>>(),
      std::make_unique<ParamList>(
          std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init(
              {std::make_unique<VarDecl>(std::make_unique<Type>(), "", nullptr,
                                         Decl::Visibility::Default)}))),
      std::make_unique<Block>(
          std::move(container<std::vector<std::unique_ptr<Stmt>>>::init(
              {std::make_unique<BinaryOperator>(
                  std::make_unique<Identifier>("c"),
                  std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<Identifier>("b"),
                      BinaryOperatorKind::BO_Add),
                  BinaryOperatorKind::BO_Assign)}))));

  ASTPrinter p(std::cout);
  func.accept(p);

  return EXIT_SUCCESS;
}
