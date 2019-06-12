#pragma once

namespace soll {

class DiagnosticOptions {
public:
    enum class Format: bool { Soll, Vi };
    DiagnosticOptions() {}
private:
    Format m_Format : 1;
    bool m_WError : 1;
};

} // namespace soll
