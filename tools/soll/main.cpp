// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/TextDiagnosticPrinter.h"
#include "soll/FrontendTool/Utils.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/TargetSelect.h>

using namespace soll;

int main(int argc, const char **argv) {
  if (llvm::sys::Process::FixupStandardFileDescriptors()) {
    return EXIT_FAILURE;
  }

  llvm::SmallVector<const char *, 256> Args(argv, argv + argc);
  std::unique_ptr Soll = std::make_unique<CompilerInstance>();
  llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts =
      new DiagnosticOptions();
  DiagnosticsEngine Diags(DiagID, &*DiagOpts);

  if (!Soll->GetInvocation().ParseCommandLineOptions(Args, Diags)) {
    return EXIT_FAILURE;
  }

  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  if (!ExecuteCompilerInvocation(Soll.get())) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
