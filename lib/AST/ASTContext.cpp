// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ASTContext.h"
#include <sstream>
namespace soll {
llvm::APInt addressParse(llvm::StringRef Literal) {
  llvm::APInt Result = llvm::APInt::getNullValue(1);
  if (!Literal.empty()) {
    assert(!Literal.getAsInteger(0, Result) && "Invalid library address");
    unsigned BitWidth = Result.getActiveBits();
    unsigned TruncWidth = (BitWidth + 7) & ~7;
    if (TruncWidth < Result.getBitWidth()) {
      Result = Result.trunc(TruncWidth);
    }
  }
  return Result;
}
llvm::StringMap<llvm::APInt>
extractLibraries(const std::vector<std::string> &LibrariesAddressInfo) {
  llvm::StringMap<llvm::APInt> Map;
  for (auto &Libs : LibrariesAddressInfo) {
    std::stringstream SS(Libs);
    std::string Buff;
    while (SS >> Buff) {
      std::string LibName, Address;
      size_t I = 0;
      for (; Buff.at(I) != ':'; ++I)
        LibName += Buff.at(I);
      for (++I; I < Buff.size(); ++I)
        Address += Buff.at(I);
      Map[LibName] = addressParse(Address);
    }
  }
  return Map;
}
ASTContext::ASTContext(InputKind Language,
                       const std::vector<std::string> &LibrariesAddressInfo)
    : Language(Language),
      LibrariesAddressMap(extractLibraries(LibrariesAddressInfo)) {}
} // namespace soll