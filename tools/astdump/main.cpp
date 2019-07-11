#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>

using namespace soll;

int main(int argc, const char **argv) {
  SourceUnit source(std::move(container<
                              std::vector<std::unique_ptr<Decl>>>::init({
      std::make_unique<ContractDecl>(
          "C", std::vector<std::unique_ptr<InheritanceSpecifier>>(),
          std::move(container<std::vector<std::unique_ptr<Decl>>>::init(
              {std::make_unique<FunctionDecl>(
                  "add", Decl::Visibility::Public, StateMutability::Pure, false,
                  std::make_unique<ParamList>(std::move(
                      container<std::vector<std::unique_ptr<VarDecl>>>::init(
                          {std::make_unique<VarDecl>(std::make_unique<Type>(),
                                                     "a", nullptr,
                                                     Decl::Visibility::Default),
                           std::make_unique<VarDecl>(
                               std::make_unique<Type>(), "b", nullptr,
                               Decl::Visibility::Default)}))),
                  std::vector<std::unique_ptr<ModifierInvocation>>(),
                  std::make_unique<ParamList>(std::move(
                      container<std::vector<std::unique_ptr<VarDecl>>>::init(
                          {std::make_unique<VarDecl>(
                              std::make_unique<Type>(), "", nullptr,
                              Decl::Visibility::Default)}))),
                  std::make_unique<Block>(std::move(
                      container<std::vector<std::unique_ptr<Stmt>>>::init(
                          {std::make_unique<BinaryOperator>(
                               std::make_unique<Identifier>("c"),
                               std::make_unique<BinaryOperator>(
                                   std::make_unique<Identifier>("a"),
                                   std::make_unique<Identifier>("b"),
                                   BinaryOperatorKind::BO_Add),
                               BinaryOperatorKind::BO_Assign),
                           std::make_unique<CallExpr>(
                               std::make_unique<Identifier>("require"),
                               container<std::vector<std::unique_ptr<Expr>>>::
                                   init({std::make_unique<BinaryOperator>(
                                             std::make_unique<Identifier>("c"),
                                             std::make_unique<Identifier>("a"),
                                             BinaryOperatorKind::BO_GE),
                                         std::make_unique<StringLiteral>(
                                             "SafeMath: addition "
                                             "overflow")})),
                           std::make_unique<ReturnStmt>(
                               std::make_unique<Identifier>("c"))}))))})),
          ContractDecl::ContractKind::Contract),
  })));

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, source);

  return EXIT_SUCCESS;
}
