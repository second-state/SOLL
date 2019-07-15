#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticOptions.h"

namespace soll {

class TextDiagnosticPrinter : public DiagnosticConsumer {
  llvm::raw_ostream &OS;
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
  bool OwnsOutputStream;

public:
  TextDiagnosticPrinter(llvm::raw_ostream &os, DiagnosticOptions *diags,
                        bool OwnsOutputStream = false);
  virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                const Diagnostic &Info) override;
};

} // namespace soll
