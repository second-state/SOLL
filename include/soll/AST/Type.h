// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include <cassert>
#include <memory>
#include <optional>
#include <vector>

namespace soll {

enum class DataLocation { Storage, CallData, Memory };

class Type {
public:
  enum class Category {
    Address,
    Integer,
    RationalNumber,
    String,
    Bool,
    Array,
    FixedBytes,
    Contract,
    Struct,
    Function,
    Enum,
    Tuple,
    Mapping,
  };
  virtual ~Type() noexcept {}

  // TODO: change the following following function to virtual pure function
  // return nonsense value due CE in tools/
  virtual void setBitNum(unsigned) {}
  virtual unsigned int getBitNum() const { return 0; }
  virtual bool isImplicitlyConvertibleTo(Type const &_other) const {
    return false;
  }
  virtual bool isExplicitlyConvertibleTo(Type const &_convertTo) const {
    return false;
  }
  virtual Category getCategory() const = 0;
};

class AddressType : public Type {
  Category getCategory() const override { return Category::Address; }
};

class BooleanType : public Type {
  Category getCategory() const override { return Category::Bool; }
};

class IntegerType : public Type {
public:
  enum class IntKind {
    // unsigned Integer
    U8,
    U16,
    U24,
    U32,
    U40,
    U48,
    U56,
    U64,
    U72,
    U80,
    U88,
    U96,
    U104,
    U112,
    U120,
    U128,
    U136,
    U144,
    U152,
    U160,
    U168,
    U176,
    U184,
    U192,
    U200,
    U208,
    U216,
    U224,
    U232,
    U240,
    U248,
    U256,
    // signed Integer
    I8,
    I16,
    I24,
    I32,
    I40,
    I48,
    I56,
    I64,
    I72,
    I80,
    I88,
    I96,
    I104,
    I112,
    I120,
    I128,
    I136,
    I144,
    I152,
    I160,
    I168,
    I176,
    I184,
    I192,
    I200,
    I208,
    I216,
    I224,
    I232,
    I240,
    I248,
    I256
  };

  IntegerType(IntKind ik) : _intKind(ik) {}
  IntKind getKind() const { return _intKind; }
  bool isSigned() const {
    return static_cast<int>(getKind()) >= static_cast<int>(IntKind::I8);
  }

  void setBitNum() = delete;
  unsigned int getBitNum() const override {
    return 8 * (static_cast<int>(getKind()) % 32 + 1);
  }
  bool isImplicitlyConvertibleTo(Type const &_other) const override;
  bool isExplicitlyConvertibleTo(Type const &_convertTo) const override;
  Category getCategory() const override { return Category::Integer; }

private:
  IntKind _intKind;
};

class StringType : public Type {
  Category getCategory() const override { return Category::String; }
};

class ReferenceType : public Type {
protected:
  DataLocation Loc;
  explicit ReferenceType(DataLocation Loc) : Loc(Loc) {}

public:
  DataLocation location() const { return Loc; }
};

class MappingType : public ReferenceType {
  TypePtr KeyType;
  TypePtr ValueType;

public:
  MappingType(TypePtr &&KT, TypePtr &&VT)
      : KeyType(std::move(KT)), ValueType(std::move(VT)),
        ReferenceType(DataLocation::Storage) {}

  TypePtr getKeyType() const { return KeyType; }
  TypePtr getValueType() const { return ValueType; }

  Category getCategory() const override { return Category::Mapping; }
};

class ArrayType : public ReferenceType {
  TypePtr ElementType;
  std::optional<uint32_t> Length; ///< Length of the array
public:
  // dynamic-sized array
  ArrayType(TypePtr ET, DataLocation Loc)
      : ElementType(ET), ReferenceType(Loc) {}
  // fix-sized array
  ArrayType(TypePtr ET, uint32_t L, DataLocation Loc)
      : ElementType(ET), Length(L), ReferenceType(Loc) {}

  TypePtr getElementType() const { return ElementType; }

  bool isDynamicSized() const { return !Length.has_value(); }
  uint32_t getLength() const {
    assert(!isDynamicSized());
    return *Length;
  }
  Category getCategory() const override { return Category::Array; }
};

class FunctionType : public Type {
  std::vector<TypePtr> ParamTypes;
  std::vector<TypePtr> ReturnTypes;

public:
  const std::vector<TypePtr> &getParamTypes() const { return ParamTypes; }
  const std::vector<TypePtr> &getReturnTypes() const { return ReturnTypes; }
};

class StructType : public Type {
  // TODO
};

} // namespace soll
