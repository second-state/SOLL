#pragma once
#include <llvm/Support/raw_ostream.h>
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Frontend/DiagnosticRenderer.h"

namespace soll {

class TextDiagnostic: public DiagnosticRenderer {
public:
    TextDiagnostic(llvm::raw_ostream& OS, DiagnosticOptions& DiagOpts)
        : DiagnosticRenderer(), m_OS(OS), m_DiagOpts(DiagOpts) {}
private:
    llvm::raw_ostream &m_OS;
    DiagnosticOptions& m_DiagOpts;
};

} // namespace soll
