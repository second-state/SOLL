// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Type.h"

namespace soll {

bool IntegerType::isImplicitlyConvertibleTo(Type const &_other) const {
  if (_other.getCategory() == Category::Integer) {
    IntegerType const &convertTo = dynamic_cast<IntegerType const &>(_other);
    if (this->getBitNum() > convertTo.getBitNum())
      return false;
    else if (this->isSigned())
      return convertTo.isSigned();
    else
      return !convertTo.isSigned() || convertTo.getBitNum() > this->getBitNum();
  }
  return false;
}

bool IntegerType::isExplicitlyConvertibleTo(Type const &_convertTo) const {
  switch (_convertTo.getCategory()) {
  case Category::Integer:
  case Category::Address:
  case Category::Contract:
  case Category::Enum:
    return true;
  case Category::FixedBytes:
    // TODO: FixedBytes not implemented yet
    // return numBits() == dynamic_cast<FixedBytesType
    // const&>(_convertTo).numBytes() * 8)
  default:
    return false;
  }
}

std::shared_ptr<IntegerType> IntegerType::common(const IntegerType &A,
                                                 const IntegerType &B) {
  // TODO: replace this
  // Solidity allows int and uint to have common type
  // but this temp. impl. does not deal with it
  // for example;
  //  common(uint64, int72) ->  int72
  //  common(uint72, int64) -> uint72
  //  common(uint64, int64) -> uint72
  assert(A.isSigned() == B.isSigned());
  return std::make_shared<IntegerType>(std::max(A.getKind(), B.getKind()));
}
} // namespace soll