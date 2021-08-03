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
      if (Buff.empty())
        continue;
      size_t Pivot = Buff.find_last_of(":");
      auto LibName = Buff.substr(0, Pivot);
      auto Address = Buff.substr(Pivot + 1);
      Map[LibName] = addressParse(Address);
    }
  }
  return Map;
}
ASTContext::ASTContext(InputKind Language,
                       const std::vector<std::string> &LibrariesAddressInfo)
    : Language(Language),
      LibrariesAddressMap(extractLibraries(LibrariesAddressInfo)),
      IntegerTypeU256Ptr(
          std::make_shared<IntegerType>(IntegerType::IntKind::U256)),
      IntegerTypeI256Ptr(
          std::make_shared<IntegerType>(IntegerType::IntKind::I256)),
      ContractTypePtr(std::make_shared<ContractType>()),
      FixedBytesTypeB32Ptr(
          std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32)),
      FixedBytesTypeB20Ptr(
          std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B20)),
      FixedBytesTypeB4Ptr(
          std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B4)),
      FixedBytesTypeB1Ptr(
          std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B1)),
      BooleanTypePtr(std::make_shared<BooleanType>()),
      StringTypePtr(std::make_shared<StringType>()),
      BytesTypePtr(std::make_shared<BytesType>()),
      AddressTypeNonPayablePtr(
          std::make_shared<AddressType>(StateMutability::NonPayable)),
      AddressTypePayablePtr(
          std::make_shared<AddressType>(StateMutability::Payable)),
      NullPtr(nullptr) {}
} // namespace soll