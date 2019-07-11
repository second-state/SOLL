#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

namespace soll {

class ASTConsumer;
std::unique_ptr<ASTConsumer>
CreateASTPrinter(std::unique_ptr<llvm::raw_ostream> OS,
                 llvm::StringRef FilterString);

} // namespace soll
