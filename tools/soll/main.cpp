// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "soll/AST/Expr.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/CompilerInvocation.h"
#include <llvm/Support/Process.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

using namespace soll;

int main(int argc, const char **argv) {
  if (llvm::sys::Process::FixupStandardFileDescriptors()) {
    return EXIT_FAILURE;
  }

  std::unique_ptr Soll = std::make_unique<CompilerInstance>();
  llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());

  Soll->GetInvocation().ParseCommandLineOptions(argc, argv);

  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  if (!Soll->Execute()) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
