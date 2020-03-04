// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/CodeGenOptions.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/FileSystemOptions.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/Frontend/DiagnosticRenderer.h"
#include "soll/Frontend/FrontendOptions.h"
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <memory>

namespace soll {

class CompilerInstance;
class DiagnosticOptions;

class CompilerInvocation {
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagnosticOpts;
  std::unique_ptr<DiagnosticRenderer> DiagRenderer;
  CodeGenOptions CodeGenOpts;
  FileSystemOptions FileSystemOpts;
  FrontendOptions FrontendOpts;
  TargetOptions TargetOpts;

public:
  explicit CompilerInvocation() {}
  bool ParseCommandLineOptions(llvm::ArrayRef<const char *> Arg,
                               DiagnosticsEngine &Diags);
  DiagnosticOptions &GetDiagnosticOptions();
  DiagnosticRenderer &GetDiagnosticRenderer();

  CodeGenOptions &getCodeGenOpts() { return CodeGenOpts; }
  const CodeGenOptions &getCodeGenOpts() const { return CodeGenOpts; }

  FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }
  const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }

  FrontendOptions &getFrontendOpts() { return FrontendOpts; }
  const FrontendOptions &getFrontendOpts() const { return FrontendOpts; }

  TargetOptions &getTargetOpts() { return TargetOpts; }
  const TargetOptions &getTargetOpts() const { return TargetOpts; }
};

} // namespace soll
