// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/TextDiagnostic.h"
#include <llvm/Support/CommandLine.h>

namespace cl = llvm::cl;

namespace soll {

static cl::opt<bool> Help("h", cl::desc("Alias for -help"), cl::Hidden);

static cl::OptionCategory SollCategory("Soll options");
static cl::list<std::string> InputFilenames(cl::Positional,
                                            cl::desc("[<file> ...]"),
                                            cl::cat(SollCategory));
static cl::opt<ActionKind> Action("action", cl::Optional, cl::ValueRequired,
                                  cl::init(EmitLLVM),
                                  cl::values(clEnumVal(ASTDump, "")),
                                  cl::values(clEnumVal(EmitLLVM, "")),
                                  cl::values(clEnumVal(EmitFuncSig, "")),
                                  cl::values(clEnumVal(EmitABI, "")),
                                  cl::cat(SollCategory));

void CompilerInvocation::ParseCommandLineOptions(int argc, const char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv);

  DiagnosticOpts = new DiagnosticOptions();
  DiagRenderer =
      std::make_unique<TextDiagnostic>(llvm::errs(), *DiagnosticOpts);
  FrontendOpts.ProgramAction = Action;
}

bool CompilerInvocation::Execute(CompilerInstance &CI) {
  llvm::LLVMContext Ctx;
  std::unique_ptr<FrontendAction> Action;
  switch (CI.getFrontendOpts().ProgramAction) {
  case ASTDump:
    Action = std::make_unique<ASTPrintAction>();
    break;
  case EmitLLVM:
    Action = std::make_unique<EmitLLVMAction>(&Ctx);
    break;
  case EmitFuncSig:
    Action = std::make_unique<EmitFuncSigAction>();
    break;
  case EmitABI:
    Action = std::make_unique<EmitABIAction>();
    break;
  }
  return std::all_of(std::begin(InputFilenames), std::end(InputFilenames),
                     [&Action, &CI](const auto &filename) {
                       return CI.ExecuteAction(*Action, filename);
                     });
}

DiagnosticOptions &CompilerInvocation::GetDiagnosticOptions() {
  return *DiagnosticOpts;
}

DiagnosticRenderer &CompilerInvocation::GetDiagnosticRenderer() {
  return *DiagRenderer;
}

} // namespace soll
