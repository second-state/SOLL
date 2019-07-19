#include "soll/AST/Expr.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Frontend/CompilerInvocation.h"
#include <llvm/Support/Process.h>
#include <llvm/Support/raw_ostream.h>

using namespace soll;

int main(int argc, const char **argv) {
  if (llvm::sys::Process::FixupStandardFileDescriptors()) {
    return EXIT_FAILURE;
  }

  CompilerInstance Soll;
  Soll.GetInvocation().ParseCommandLineOptions(argc, argv);

  if (!Soll.Execute()) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
