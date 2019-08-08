// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
namespace soll {

class Type {
public:
  // copied from Solitity Types.h
  enum class Category {
    Address,
    Integer,
    RationalNumber,
    StringLiteral,
    Bool,
    FixedPoint,
    Array,
    FixedBytes,
    Contract,
    Struct,
    Function,
    Enum,
    Tuple,
    Mapping,
    TypeType,
    Modifier,
    Magic,
    Module,
    InaccessibleDynamic
  };

  // TODO: change the following following function to virtual pure function
  // return nonsense value due CE in some files
  virtual bool isImplicitlyConvertibleTo(Type const &_other) const {
    return false;
  }
  virtual bool isExplicitlyConvertibleTo(Type const &_convertTo) const {
    return false;
  }
  virtual Category category() const { return Category::Integer; }
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
  bool isSigned() const { return static_cast<int>(getKind()) >= static_cast<int>(IntKind::I8); }
  unsigned int numBits() const { return 8 * (static_cast<int>(getKind()) % 32 + 1); }

  bool isImplicitlyConvertibleTo(Type const &_other) const override;
  bool isExplicitlyConvertibleTo(Type const &_convertTo) const override;
  Category category() const override { return Category::Integer; }

private:
  IntKind _intKind;
};

} // namespace soll
