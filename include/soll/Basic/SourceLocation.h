#pragma once
#include <cstdint>
#include <llvm/Support/raw_ostream.h>
#include "soll/Basic/SourceManager.h"

namespace soll {

class SourceLocation {
public:
    virtual ~SourceLocation() = default;
    virtual void print(llvm::raw_ostream &OS, const SourceManager& SM);
protected:
    SourceManager::FileID m_FileID;
    SourceManager::FileOffset m_Offset;
};

} // namespace soll
