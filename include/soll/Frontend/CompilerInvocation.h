#pragma once
#include <memory>

namespace soll {

class CompilerInstance;
class DiagnosticOptions;
class DiagnosticRenderer;

class CompilerInvocation {
public:
    explicit CompilerInvocation() {}
    void ParseCommandLineOptions(int argc, const char **argv);
    bool Execute(CompilerInstance& CI);
    DiagnosticOptions& GetDiagnosticOptions();
    DiagnosticRenderer& GetDiagnosticRenderer();
private:
    std::unique_ptr<DiagnosticOptions> m_DiagOpts;
    std::unique_ptr<DiagnosticRenderer> m_DiagRenderer;
};

} // namespace soll
