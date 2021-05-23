// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/TypePtr.h"

namespace soll {
TypePtr IntegerTypeU256Ptr =
    std::make_shared<IntegerType>(IntegerType::IntKind::U256);
TypePtr IntegerTypeI256Ptr =
    std::make_shared<IntegerType>(IntegerType::IntKind::I256);
TypePtr ContractTypePtr = std::make_shared<ContractType>();
TypePtr FixedBytesTypeB32Ptr =
    std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32);
TypePtr FixedBytesTypeB20Ptr =
    std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B20);
TypePtr FixedBytesTypeB4Ptr =
    std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B4);
TypePtr FixedBytesTypeB1Ptr =
    std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B1);
TypePtr BooleanTypePtr = std::make_shared<BooleanType>();
TypePtr StringTypePtr = std::make_shared<StringType>();
TypePtr BytesTypePtr = std::make_shared<BytesType>();
TypePtr AddressTypeNonPayablePtr =
    std::make_shared<AddressType>(StateMutability::NonPayable);
TypePtr AddressTypePayablePtr =
    std::make_shared<AddressType>(StateMutability::Payable);
} // namespace soll