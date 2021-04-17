// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Type.h"
#include "soll/AST/Decl.h"

namespace soll {

bool IntegerType::isImplicitlyConvertibleTo(Type const &_other) const {
  if (_other.getCategory() == Category::Integer) {
    IntegerType const &ConvertTo = dynamic_cast<IntegerType const &>(_other);
    if (this->getBitNum() > ConvertTo.getBitNum())
      return false;
    else if (this->isSigned())
      return ConvertTo.isSigned();
    else
      return !ConvertTo.isSigned() || ConvertTo.getBitNum() > this->getBitNum();
  }
  if (_other.getCategory() == Category::Bool) {
    return true;
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

bool TupleType::isImplicitlyConvertibleTo(Type const &_other) const {
  if (_other.getCategory() != Category::Integer) {
    return false;
  }
  TupleType const &convertTo = dynamic_cast<TupleType const &>(_other);
  bool result = true;
  std::size_t num = ElementTypes.size();

  for (std::size_t idx = 0; idx < num; ++idx)
    result &= ElementTypes[idx]->isImplicitlyConvertibleTo(
        *convertTo.getElementTypes()[idx]);
  return result;
}

bool TupleType::isExplicitlyConvertibleTo(Type const &_convertTo) const {
  return false;
}

std::string StructType::getUniqueName() const {
  return D->getUniqueName().str();
};

std::string ContractType::getUniqueName() const {
  return D->getUniqueName().str();
};

} // namespace soll
