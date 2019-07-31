// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/FileSystemOptions.h"
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <memory>

namespace soll {

class CompilerInstance;
class DiagnosticOptions;
class DiagnosticRenderer;

class CompilerInvocation {
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagnosticOpts;
  std::unique_ptr<DiagnosticRenderer> DiagRenderer;
  FileSystemOptions FileSystemOpts;

public:
  explicit CompilerInvocation() {}
  void ParseCommandLineOptions(int argc, const char **argv);
  bool Execute(CompilerInstance &CI);
  DiagnosticOptions &GetDiagnosticOptions();
  DiagnosticRenderer &GetDiagnosticRenderer();

  FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }
  const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }
};

} // namespace soll
