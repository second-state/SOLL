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

#include <sstream>

namespace cl = llvm::cl;

namespace soll {

static cl::opt<bool> Help("h", cl::desc("Alias for -help"), cl::Hidden);

static cl::OptionCategory SollCategory("SOLL options");

static cl::list<std::string> InputFilenames(cl::Positional,
                                            cl::desc("[<file> ...]"),
                                            cl::cat(SollCategory));

static cl::list<std::string> Libraries(
    "libraries", cl::Optional,
    cl::desc(
        "Direct string containing library addresses. "
        "Syntax: <libraryName>:<address> [whitespace] ...\n"
        "Address is interpreted as a hex string optionally prefixed by 0x."),
    cl::cat(SollCategory));

static cl::opt<InputKind> Language("lang", cl::Optional, cl::ValueRequired,
                                   cl::init(Sol),
                                   cl::values(clEnumVal(Sol, "")),
                                   cl::values(clEnumVal(Yul, "")),
                                   cl::cat(SollCategory));

static cl::opt<DeployPlatformKind> DeployPlatform(
    "deploy", cl::Optional, cl::ValueRequired, cl::init(Chain),
    cl::values(clEnumVal(Normal, "Normal case")),
    cl::values(clEnumVal(Chain, "EWASM vm not support keccak256 yet, so for "
                                "workaround use sha256 instead")),
    cl::cat(SollCategory));

static cl::opt<ActionKind> Action(
    "action", cl::Optional, cl::ValueRequired, cl::init(EmitWasm),
    cl::values(clEnumVal(ASTDump, "")), cl::values(clEnumVal(EmitAssembly, "")),
    cl::values(clEnumVal(EmitBC, "")), cl::values(clEnumVal(EmitLLVM, "")),
    cl::values(clEnumVal(EmitLLVMOnly, "")),
    cl::values(clEnumVal(EmitCodeGenOnly, "")),
    cl::values(clEnumVal(EmitObj, "")), cl::values(clEnumVal(EmitWasm, "")),
    cl::values(clEnumVal(EmitFuncSig, "")), cl::values(clEnumVal(EmitABI, "")),
    cl::values(clEnumVal(InitOnly, "")),
    cl::values(clEnumVal(ParseSyntaxOnly, "")), cl::cat(SollCategory));

static cl::opt<OptLevel> OptimizationLevel(
    cl::Optional, cl::init(O0), cl::desc("Optimization level"),
    cl::cat(SollCategory),
    cl::values(clEnumVal(O0, "No optimizations"),
               clEnumVal(O1, "Enable trivial optimizations"),
               clEnumVal(O2, "Enable default optimizations"),
               clEnumVal(O3, "Enable expensive optimizations"),
               clEnumVal(Os, "Enable default optimizations for size"),
               clEnumVal(Oz, "Enable expensive optimizations for size")));

static cl::opt<bool> Runtime("runtime", cl::desc("Generate for runtime code"),
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

  DiagnosticOpts->ShowColors = llvm::sys::Process::StandardErrHasColors();
  DiagRenderer =
      std::make_unique<TextDiagnostic>(llvm::errs(), *DiagnosticOpts);

  for (auto &Filename : InputFilenames) {
    FrontendOpts.Inputs.emplace_back(Filename);
  }
  for (auto &Libs : Libraries) {
    FrontendOpts.LibrariesAddressMaps.emplace_back(Libs);
  }
  FrontendOpts.ProgramAction = Action;
  FrontendOpts.Language = Language;
  if (Target == EWASM) {
    TargetOpts.DeployPlatform = DeployPlatform;
  }
  TargetOpts.BackendTarget = Target;

  CodeGenOpts.OptimizationLevel = OptimizationLevel;
  CodeGenOpts.Runtime = Runtime;
  return true;
}

DiagnosticOptions &CompilerInvocation::GetDiagnosticOptions() {
  return *DiagnosticOpts;
}

DiagnosticRenderer &CompilerInvocation::GetDiagnosticRenderer() {
  return *DiagRenderer;
}

} // namespace soll
