#pragma once
#include <cstdint>
#include <string_view>
#include <utility>

namespace soll {

class SourceManager {
public:
    using FileID = std::uint32_t;
    using FileOffset = std::uint32_t;
    std::string_view getFilename(FileID id) const;
    std::pair<std::uint32_t, std::uint32_t> getPosition(FileID id, FileOffset offset) const;
};

} // namespace soll
