// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "soll/Frontend/ASTConsumers.h"
#include <iostream>

using namespace soll;

using llvm::IRBuilder;
using llvm::LLVMContext;

int main(int argc, const char **argv) {
  using std::make_unique;
  SourceUnit source(                                      // SourceUnit
      make_unique_vector<Decl>(                           // list of Contracts
          make_unique<ContractDecl>(                      // ContractDecl
              "C",                                        // Contract name
              make_unique_vector<InheritanceSpecifier>(), // Contract base
              make_unique_vector<Decl>(                   // list
                  make_unique<FunctionDecl>(
                      "print",                  // Function name
                      Decl::Visibility::Public, // Function visibility
                      StateMutability::Pure,    // Function mutability
                      false,                    // Function is constructor
                      make_unique<ParamList>(   // Function parameter
                          make_unique_vector<VarDecl>( // list of Params
                              make_unique<VarDecl>(    // string a;
                                  make_unique<StringType>(), "a", nullptr,
                                  Decl::Visibility::Default),
                              make_unique<VarDecl>( // uint122 b;
                                  make_unique<IntegerType>(
                                      IntegerType::IntKind::U112),
                                  "b", nullptr,
                                  Decl::Visibility::Default)) // end of
                                                              // vector<VarDecl>
                          ), // end of ParamList
                      make_unique_vector<ModifierInvocation>(),
                      make_unique<ParamList>(          // Function return value
                          make_unique_vector<VarDecl>( // list of Params
                              make_unique<VarDecl>(    // returns (int8)
                                  make_unique<IntegerType>(
                                      IntegerType::IntKind::I8),
                                  "", nullptr,
                                  Decl::Visibility::Default)) // end of
                                                              // vector<VarDecl>
                          ),                           // end of ParamList
                      make_unique<Block>(              // Function body
                          make_unique_vector<Stmt>())) // end of FunctionDecl
                  ),                                   // end of vector<Decl>
              ContractDecl::ContractKind::Contract)    // end of ContractDecl
          )                                            // end of vector<Decl>
  );                                                   // end of SourceUnit

  ASTContext *Ctx = new ASTContext();
  auto p = CreateASTPrinter();
  p->HandleSourceUnit(*Ctx, source);
  return EXIT_SUCCESS;
}
