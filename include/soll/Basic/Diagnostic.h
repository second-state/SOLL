#pragma once

namespace soll {

class DiagnosticEntry {
public:
};

class DiagnosticsEngine {
    DiagnosticsEngine(const DiagnosticsEngine&) = delete;
    DiagnosticsEngine& operator=(const DiagnosticsEngine&) = delete;
public:
    enum class Level {
        Ignored,
        Note,
        Remark,
        Warning,
        Error,
        Fatal,
    };
};

class DiagnosticConsumer {
public:
    DiagnosticConsumer() = default;
    virtual ~DiagnosticConsumer() = default;
    virtual void clear() {}
    virtual void BeginSourceFile() {}
    virtual void EndSourceFile() {}
    virtual void finish() {}
    virtual void HandleDiagnostic(
        DiagnosticsEngine::Level DiagLevel,
        const DiagnosticEntry &Entry
        ) {}
};

} // namespace soll
