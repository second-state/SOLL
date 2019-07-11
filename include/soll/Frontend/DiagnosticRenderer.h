#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/SourceLocation.h"
#include <string_view>

namespace soll {

class DiagnosticRenderer {
public:
  virtual ~DiagnosticRenderer() = default;
  virtual void emitDiagnosticMessage(SourceLocation Loc,
                                     DiagnosticsEngine::Level Level,
                                     std::string_view Message);
};

} // namespace soll
