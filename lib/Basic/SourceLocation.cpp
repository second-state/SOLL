#include "soll/Basic/SourceManager.h"
#include "soll/Basic/SourceLocation.h"

namespace soll {

void SourceLocation::print(llvm::raw_ostream &OS, const SourceManager& SM) {
    auto pos = SM.getPosition(m_FileID, m_Offset);
    OS << SM.getFilename(m_FileID).data() << ':' << pos.first << ':' << pos.second;
}

} // namespace soll
