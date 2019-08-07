// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Type.h"

namespace soll {

bool IntegerType::isImplicitlyConvertibleTo(Type const &_other) const {
  if (_other.category() == Category::Integer) {
    IntegerType const &convertTo = dynamic_cast<IntegerType const &>(_other);
    if (this->numBits() > convertTo.numBits())
      return false;
    else if (this->isSigned())
      return convertTo.isSigned();
    else
      return !convertTo.isSigned() || convertTo.numBits() > this->numBits();
  }
  return false;
}

bool IntegerType::isExplicitlyConvertibleTo(Type const &_convertTo) const {
  switch (_convertTo.category()) {
  case Category::Integer:
  case Category::Address:
  case Category::Contract:
  case Category::Enum:
  case Category::FixedPoint:
    return true;
  case Category::FixedBytes:
    // TODO: FixedBytes not implemented yet
    // return numBits() == dynamic_cast<FixedBytesType
    // const&>(_convertTo).numBytes() * 8)
  default:
    return false;
  }
}

} // namespace soll