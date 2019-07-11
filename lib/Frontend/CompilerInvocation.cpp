#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/TextDiagnostic.h"
#include <llvm/Support/CommandLine.h>

namespace cl = llvm::cl;

namespace soll {

static cl::opt<bool> Help("h", cl::desc("Alias for -help"), cl::Hidden);

static cl::OptionCategory SollCategory("Soll options");
static cl::list<std::string> InputFilenames(cl::Positional,
                                            cl::desc("[<file> ...]"),
                                            cl::cat(SollCategory));

void CompilerInvocation::ParseCommandLineOptions(int argc, const char **argv) {
  llvm::cl::ParseCommandLineOptions(argc, argv);

  DiagnosticOpts = new DiagnosticOptions();
  DiagRenderer = std::make_unique<TextDiagnostic>(llvm::errs(), *DiagnosticOpts);
}

bool CompilerInvocation::Execute(CompilerInstance &CI) {
  return std::all_of(
      std::begin(InputFilenames), std::end(InputFilenames),
      [&CI](const auto &filename) { return CI.Execute(filename); });
}

DiagnosticOptions &CompilerInvocation::GetDiagnosticOptions() {
  return *DiagnosticOpts;
}

DiagnosticRenderer &CompilerInvocation::GetDiagnosticRenderer() {
  return *DiagRenderer;
}

} // namespace soll
