// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Config/Config.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/TextDiagnostic.h"
#include "soll/Frontend/TextDiagnosticPrinter.h"
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/raw_ostream.h>

namespace cl = llvm::cl;

namespace soll {

static cl::opt<bool> Help("h", cl::desc("Alias for -help"), cl::Hidden);

static cl::OptionCategory SollCategory("SOLL options");

static cl::list<std::string> InputFilenames(cl::Positional,
                                            cl::desc("[<file> ...]"),
                                            cl::cat(SollCategory));

static cl::opt<InputKind> Language("lang", cl::Optional, cl::ValueRequired,
                                   cl::init(Sol),
                                   cl::values(clEnumVal(Sol, "")),
                                   cl::values(clEnumVal(Yul, "")),
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
           cl::values(clEnumVal(EWASM, "Generate LLVM IR for Ewasm backend")),
           cl::values(clEnumVal(EVM, "Generate LLVM IR for EVM backend")),
           cl::cat(SollCategory));

static void printSOLLVersion(llvm::raw_ostream &OS) {
  OS << "SOLL version " << SOLL_VERSION_STRING << "\n";
}

bool CompilerInvocation::ParseCommandLineOptions(
    llvm::ArrayRef<const char *> Arg, DiagnosticsEngine &Diags) {
  llvm::cl::SetVersionPrinter(printSOLLVersion);
  llvm::cl::ParseCommandLineOptions(Arg.size(), Arg.data());

  DiagnosticOpts = new DiagnosticOptions();
  DiagnosticOpts->ShowColors = llvm::sys::Process::StandardErrHasColors();
  DiagRenderer =
      std::make_unique<TextDiagnostic>(llvm::errs(), *DiagnosticOpts);

  for (auto &Filename : InputFilenames) {
    FrontendOpts.Inputs.emplace_back(Filename);
  }
  FrontendOpts.ProgramAction = Action;
  FrontendOpts.Language = Language;

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
