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
      : CodeGenAction(BackendAction::EmitMCNull, _VMContext) {}

  void ExecuteAction() override {
    CompilerInstance &CI = getCompilerInstance();
    if (!CI.hasLexer())
      return;
    if (!CI.hasSema())
      CI.createSema();
  }
};

TEST_CASE("TestNullCodeGen", "[CodeGenTest]") {
  auto TempFile = llvm::sys::fs::TempFile::create("test-%%%%%%.sol");
  REQUIRE(bool(TempFile));
  {
    llvm::raw_fd_ostream OS(TempFile->FD, /*shouldClose*/ true);
    OS << "contract C {}";
    OS.flush();
  }

  CompilerInstance Compiler;
  auto &Invocation = Compiler.getInvocation();
  Invocation.getFrontendOpts().Inputs.push_back(
      FrontendInputFile(TempFile->TmpName));
  Invocation.getFrontendOpts().ProgramAction = EmitLLVM;
  Invocation.getTargetOpts().BackendTarget = EWASM;
  Compiler.createDiagnostics();
  REQUIRE(Compiler.hasDiagnostics());

  auto Act = std::make_unique<NullCodeGenAction>();
  bool Success = Compiler.ExecuteAction(*Act);
  REQUIRE(Success);
}

} // namespace
