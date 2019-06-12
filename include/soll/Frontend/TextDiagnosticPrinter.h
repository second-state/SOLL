#pragma once
#include <soll/Basic/Diagnostic.h>

namespace soll {

class TextDiagnosticPrinter: public DiagnosticConsumer {
public:
    virtual void HandleDiagnostic(
        DiagnosticsEngine::Level DiagLevel,
        const DiagnosticEntry &Entry
        ) override;
};

} // namespace soll
