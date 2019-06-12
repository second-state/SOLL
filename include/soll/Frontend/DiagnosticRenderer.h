#pragma once
#include <string_view>
#include "soll/Basic/SourceLocation.h"
#include "soll/Basic/Diagnostic.h"

namespace soll {

class DiagnosticRenderer {
public:
    virtual ~DiagnosticRenderer() = default;
    virtual void emitDiagnosticMessage(
        SourceLocation Loc,
        DiagnosticsEngine::Level Level,
        std::string_view Message
    );
};

} // namespace soll
