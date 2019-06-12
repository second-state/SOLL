#include <memory>
#include "soll/Parse/Parser.h"
#include "soll/Parse/ParseAST.h"
#include "soll/Sema/Sema.h"

namespace soll {

void ParseAST(Sema& S, bool PrintStats) {
    auto P = std::make_unique<Parser>();
}

} // namespace soll
