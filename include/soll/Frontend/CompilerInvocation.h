// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/TargetOptions.h"
#include "soll/Basic/FileSystemOptions.h"
#include "soll/Frontend/FrontendOptions.h"
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
  FrontendOptions FrontendOpts;
  TargetOptions TargetOpts;

public:
  explicit CompilerInvocation() {}
  void ParseCommandLineOptions(int argc, const char **argv);
  bool Execute(CompilerInstance &CI);
  DiagnosticOptions &GetDiagnosticOptions();
  DiagnosticRenderer &GetDiagnosticRenderer();

  FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }
  const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }

  FrontendOptions &getFrontendOpts() { return FrontendOpts; }
  const FrontendOptions &getFrontendOpts() const { return FrontendOpts; }

  TargetOptions &getTargetOpts() { return TargetOpts; }
  const TargetOptions &getTargetOpts() const { return TargetOpts; }
};

} // namespace soll
