// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/Diagnostic.h"
#include <memory>

namespace soll {
class DiagnosticOptions;
class TextDiagnostic;

class TextDiagnosticPrinter : public DiagnosticConsumer {
  llvm::raw_ostream &OS;
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
  std::unique_ptr<TextDiagnostic> TextDiag;
  bool OwnsOutputStream;

public:
  TextDiagnosticPrinter(llvm::raw_ostream &os, DiagnosticOptions *diags,
                        bool OwnsOutputStream = false);
  virtual ~TextDiagnosticPrinter() override;

  virtual void BeginSourceFile() override;

  virtual void EndSourceFile() override;

  virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                const Diagnostic &Info) override;
};

} // namespace soll
