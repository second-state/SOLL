// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/CodeGenAction.h"
#include "catch.hpp"
#include "soll/Basic/TargetOptions.h"
#include "soll/Frontend/CompilerInstance.h"

using namespace soll;

namespace {

class NullCodeGenAction : public CodeGenAction {
public:
  NullCodeGenAction(llvm::LLVMContext *_VMContext = nullptr)
      : CodeGenAction(_VMContext) {}

  void ExecuteAction() override {
    CompilerInstance &CI = getCompilerInstance();
    if (!CI.hasLexer())
      return;
    if (!CI.hasSema())
      CI.createSema();
  }
};

TEST_CASE("TestNullCodeGen", "[CodeGenTest]") {
  CompilerInstance Compiler;
  auto &Invocation = Compiler.getInvocation();
  Invocation.getFrontendOpts().Inputs.push_back(FrontendInputFile("test.soll"));
  Invocation.getFrontendOpts().ProgramAction = EmitLLVM;
  Invocation.getTargetOpts().BackendTarget = EWASM;
  Compiler.createDiagnostics();
  REQUIRE(Compiler.hasDiagnostics());

  auto Act = std::make_unique<NullCodeGenAction>();
  bool Success = Compiler.ExecuteAction(*Act);
  REQUIRE(Success);
}

} // namespace
