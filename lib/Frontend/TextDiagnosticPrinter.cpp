#include "soll/Frontend/TextDiagnosticPrinter.h"

namespace soll {

TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &os,
                                             DiagnosticOptions *diags,
                                             bool _OwnsOutputStream)
    : OS(os), DiagOpts(diags), OwnsOutputStream(_OwnsOutputStream) {}

void TextDiagnosticPrinter::HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                             const Diagnostic &Info) {
  DiagnosticConsumer::HandleDiagnostic(DiagLevel, Info);
}

} // namespace soll
