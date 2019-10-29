// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/TextDiagnostic.h"
#include "soll/Frontend/TextDiagnosticPrinter.h"
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
static cl::opt<TargetKind>
    Target("target", cl::Optional, cl::ValueRequired, cl::init(EWASM),
           cl::values(clEnumVal(EWASM, "Generate LLVM IR for eWASM backend")),
           cl::values(clEnumVal(EVM, "Generate LLVM IR for EVM backend")),
           cl::cat(SollCategory));

bool CompilerInvocation::ParseCommandLineOptions(
    llvm::ArrayRef<const char *> Arg, DiagnosticsEngine &Diags) {
  llvm::cl::ParseCommandLineOptions(Arg.size(), Arg.data());

  DiagnosticOpts = new DiagnosticOptions();
  DiagRenderer =
      std::make_unique<TextDiagnostic>(llvm::errs(), *DiagnosticOpts);

  for (auto &Filename : InputFilenames) {
    FrontendOpts.Inputs.emplace_back(Filename);
  }
  FrontendOpts.ProgramAction = Action;

  TargetOpts.BackendTarget = Target;
  return true;
}

DiagnosticOptions &CompilerInvocation::GetDiagnosticOptions() {
  return *DiagnosticOpts;
}

DiagnosticRenderer &CompilerInvocation::GetDiagnosticRenderer() {
  return *DiagRenderer;
}

} // namespace soll
