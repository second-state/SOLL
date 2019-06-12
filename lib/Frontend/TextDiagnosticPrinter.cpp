#include "soll/Frontend/TextDiagnosticPrinter.h"

namespace soll {

void TextDiagnosticPrinter::HandleDiagnostic(
    DiagnosticsEngine::Level DiagLevel,
    const DiagnosticEntry &Entry
) {
    DiagnosticConsumer::HandleDiagnostic(DiagLevel, Entry);
}

} // namespace soll
