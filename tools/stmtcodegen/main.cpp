#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>
#include <cstring>

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
  if(argc==1) return 0;
  std::unique_ptr<SourceUnit> source;
  if(!strcmp(argv[1], "IfStmt")) { // IfStmt test
    source = std::move(std::make_unique<SourceUnit>(
      std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
        std::make_unique<ContractDecl>(
          "C", std::vector<std::unique_ptr<InheritanceSpecifier>>(),
          std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
            std::make_unique<FunctionDecl>(
              "test", Decl::Visibility::Public, StateMutability::Pure,
              false,
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(),"a", nullptr,
                    Decl::Visibility::Default),
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(),"b", nullptr,
                    Decl::Visibility::Default)
              }))),
              std::vector<std::unique_ptr<ModifierInvocation>>(),
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(), "", nullptr,
                    Decl::Visibility::Default
              )}))),
              std::make_unique<Block>(
                std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                  std::make_unique<IfStmt>(
                    std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<NumberLiteral>(9),
                      BinaryOperatorKind::BO_GT
                    ),
                    std::make_unique<Block>(
                      std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                    }))
                    ),
                    std::make_unique<Block>(
                      std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                        std::make_unique<IfStmt>(
                          std::make_unique<BinaryOperator>(
                            std::make_unique<Identifier>("a"),
                            std::make_unique<NumberLiteral>(11),
                            BinaryOperatorKind::BO_GT
                          ),
                          std::make_unique<Block>(
                            std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                          }))),
                          std::make_unique<Block>(
                            std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                        })))),
                        std::make_unique<BinaryOperator>(
                          std::make_unique<Identifier>("a"),
                          std::make_unique<Identifier>("b"),
                          BinaryOperatorKind::BO_Add
                        )
          }))))}))))})),
          ContractDecl::ContractKind::Contract),
    }))));
  }
  else if(!strcmp(argv[1], "WhileStmt")) { // WhileStmt test
    source = std::move(std::make_unique<SourceUnit>(
      std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
        std::make_unique<ContractDecl>(
          "C", std::vector<std::unique_ptr<InheritanceSpecifier>>(),
          std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
            std::make_unique<FunctionDecl>(
              "test", Decl::Visibility::Public, StateMutability::Pure,
              false,
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(),"a", nullptr,
                    Decl::Visibility::Default)
              }))),
              std::vector<std::unique_ptr<ModifierInvocation>>(),
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(), "", nullptr,
                    Decl::Visibility::Default
              )}))),
              std::make_unique<Block>(
                std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                  std::make_unique<WhileStmt>(
                    std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<NumberLiteral>(10),
                      BinaryOperatorKind::BO_GT
                    ),
                    std::make_unique<Block>(
                      std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                        std::make_unique<BinaryOperator>(
                          std::make_unique<Identifier>("a"),
                          std::make_unique<Identifier>("a"),
                          BinaryOperatorKind::BO_Add
                        )
                      }))
                    ),
                    false
                  ),
                  std::make_unique<BinaryOperator>(
                    std::make_unique<NumberLiteral>(10),
                    std::make_unique<Identifier>("a"),
                    BinaryOperatorKind::BO_Add
                  )
                }))))})),
          ContractDecl::ContractKind::Contract),
    }))));
  }
  else if(!strcmp(argv[1], "ForStmt")) { // ForStmt test
    source = std::move(std::make_unique<SourceUnit>(
      std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
        std::make_unique<ContractDecl>(
          "C", std::vector<std::unique_ptr<InheritanceSpecifier>>(),
          std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
            std::make_unique<FunctionDecl>(
              "test", Decl::Visibility::Public, StateMutability::Pure,
              false,
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(),"a", nullptr,
                    Decl::Visibility::Default)
              }))),
              std::vector<std::unique_ptr<ModifierInvocation>>(),
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(), "", nullptr,
                    Decl::Visibility::Default
              )}))),
              std::make_unique<Block>(
                std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                  std::make_unique<ForStmt>(
                    std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<NumberLiteral>(3),
                      BinaryOperatorKind::BO_Add
                    ),
                    std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<NumberLiteral>(10),
                      BinaryOperatorKind::BO_LT
                    ),
                    std::make_unique<BinaryOperator>(
                      std::make_unique<Identifier>("a"),
                      std::make_unique<BinaryOperator>(
                        std::make_unique<Identifier>("a"),
                        std::make_unique<NumberLiteral>(2),
                        BinaryOperatorKind::BO_Sub
                      ),
                      BinaryOperatorKind::BO_Assign
                    ),
                    std::make_unique<Block>(
                      std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                        std::make_unique<BinaryOperator>(
                          std::make_unique<Identifier>("a"),
                          std::make_unique<Identifier>("a"),
                          BinaryOperatorKind::BO_Add
                        )
                    })))
                  ),
                  std::make_unique<BinaryOperator>(
                    std::make_unique<NumberLiteral>(10),
                    std::make_unique<Identifier>("a"),
                    BinaryOperatorKind::BO_Add
                  )
          }))))})),
          ContractDecl::ContractKind::Contract),
    }))));
  }
  
  else if(!strcmp(argv[1], "ReturnStmt")) { // ReturnStmt test
    source = std::move(std::make_unique<SourceUnit>(
      std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
        std::make_unique<ContractDecl>(
          "C", std::vector<std::unique_ptr<InheritanceSpecifier>>(),
          std::move(container<std::vector<std::unique_ptr<Decl>>>::init({
            std::make_unique<FunctionDecl>(
              "test", Decl::Visibility::Public, StateMutability::Pure,
              false,
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(),"a", nullptr,
                    Decl::Visibility::Default)
              }))),
              std::vector<std::unique_ptr<ModifierInvocation>>(),
              std::make_unique<ParamList>(
                std::move(container<std::vector<std::unique_ptr<VarDecl>>>::init({
                  std::make_unique<VarDecl>(
                    std::make_unique<Type>(), "", nullptr,
                    Decl::Visibility::Default
              )}))),
              std::make_unique<Block>(
                std::move(container<std::vector<std::unique_ptr<Stmt>>>::init({
                  std::make_unique<ReturnStmt>(
                    std::make_unique<NumberLiteral>(7122)
                  )
                }))))})),
          ContractDecl::ContractKind::Contract),
    }))));
  }
  else return 0;

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, *source);

  LLVMContext Context;
  IRBuilder<llvm::NoFolder> Builder(Context);
  llvm::Module Module("FuncBodyCGTest", Context);
  FuncBodyCodeGen FBCG(Context, Builder, Module);

  FBCG.compile(*static_cast<const FunctionDecl *>(
      static_cast<const ContractDecl *>(source->getNodes().front())
          ->getSubNodes()
          .front()));
  Module.print(llvm::errs(), nullptr);
  return EXIT_SUCCESS;
}
