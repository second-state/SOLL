// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include <cassert>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/DerivedTypes.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace soll {

class ContractDecl;

enum class DataLocation { Storage, CallData, Memory };

class Type {
public:
  enum class Category {
    Address,
    Integer,
    RationalNumber,
    String,
    Bytes,
    Bool,
    Array,
    FixedBytes,
    Contract,
    Struct,
    Function,
    Enum,
    Tuple,
    Mapping,
    Unknow,
  };
  virtual ~Type() noexcept {}

  // TODO: change the following following function to virtual pure function
  // return nonsense value due CE in tools/
  virtual unsigned int getBitNum() const { return 0; }
  virtual bool isImplicitlyConvertibleTo(Type const &_other) const {
    return _other.getCategory() == getCategory();
  }
  virtual bool isExplicitlyConvertibleTo(Type const &_convertTo) const {
    return _convertTo.getCategory() == getCategory();
  }
  virtual Category getCategory() const = 0;
  virtual std::string getName() const = 0;
  virtual std::string getSignatureEncoding() const { return getName(); };
  virtual bool isDynamic() const = 0;
  virtual bool shouldEndianLess() const = 0;
  virtual unsigned getABIStaticSize() const = 0;
  virtual unsigned getStorageSize() const { return 32; }
  virtual bool isEqual(Type const &Ty) const {
    return Ty.getCategory() == getCategory();
  }
};

class AddressType : public Type {
  StateMutability SM;

public:
  AddressType(StateMutability SM) : SM(SM) {}
  StateMutability getStateMutability() const { return SM; }
  Category getCategory() const override { return Category::Address; }
  unsigned int getBitNum() const override { return 160; }
  std::string getName() const override { return "address"; }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return true; }
  unsigned getABIStaticSize() const override { return 32; }
};

class BooleanType : public Type {
public:
  Category getCategory() const override { return Category::Bool; }
  std::string getName() const override { return "bool"; }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return true; }
  unsigned getABIStaticSize() const override { return 32; }
  unsigned int getBitNum() const override { return 1; }
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
  static IntegerType getIntN(unsigned int BitNum) {
    return IntegerType(static_cast<IntKind>(BitNum / 8 - 1 + 32));
  }
  static IntegerType getUIntN(unsigned int BitNum) {
    return IntegerType(static_cast<IntKind>(BitNum / 8 - 1));
  }
  IntKind getKind() const { return _intKind; }
  bool isSigned() const {
    return static_cast<int>(getKind()) >= static_cast<int>(IntKind::I8);
  }

  unsigned int getBitNum() const override {
    return 8 * (static_cast<int>(getKind()) % 32 + 1);
  }
  bool isImplicitlyConvertibleTo(Type const &_other) const override;
  bool isExplicitlyConvertibleTo(Type const &_convertTo) const override;
  Category getCategory() const override { return Category::Integer; }
  std::string getName() const override {
    std::ostringstream oss;
    if (isSigned()) {
      oss << "int";
    } else {
      oss << "uint";
    }
    oss << getBitNum();
    return oss.str();
  }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return true; }
  unsigned getABIStaticSize() const override { return 32; }
  bool isEqual(Type const &Ty) const override {
    return Type::isEqual(Ty) &&
           static_cast<IntegerType const &>(Ty).getKind() == getKind();
  }

private:
  IntKind _intKind;
};

class FixedBytesType : public Type {
public:
  enum class ByteKind {
    B1,
    B2,
    B3,
    B4,
    B5,
    B6,
    B7,
    B8,
    B9,
    B10,
    B11,
    B12,
    B13,
    B14,
    B15,
    B16,
    B17,
    B18,
    B19,
    B20,
    B21,
    B22,
    B23,
    B24,
    B25,
    B26,
    B27,
    B28,
    B29,
    B30,
    B31,
    B32
  };
  FixedBytesType(ByteKind bk) : _byteKind(bk) {}
  ByteKind getKind() const { return _byteKind; }
  unsigned int getBitNum() const override {
    return 8 * (static_cast<int>(getKind()) + 1);
  }
  Category getCategory() const override { return Category::FixedBytes; }
  std::string getName() const override {
    std::ostringstream oss;
    oss << "bytes";
    oss << getBitNum() / 8;
    return oss.str();
  }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override { return 32; }
  bool isEqual(Type const &Ty) const override {
    return Type::isEqual(Ty) &&
           static_cast<FixedBytesType const &>(Ty).getKind() == getKind();
  }

private:
  ByteKind _byteKind;
};

class StringType : public Type {
  Category getCategory() const override { return Category::String; }
  std::string getName() const override { return "string"; }
  bool isDynamic() const override { return true; }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override { return 32; }
};

class BytesType : public Type {
  Category getCategory() const override { return Category::Bytes; }
  std::string getName() const override { return "bytes"; }
  bool isDynamic() const override { return true; }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override { return 32; }
};

class ReferenceType : public Type {
protected:
  DataLocation Loc;
  explicit ReferenceType(DataLocation Loc) : Loc(Loc) {}

public:
  DataLocation location() const { return Loc; }
  // mapping and dynamic storage use a 256-bit integer as numbering
  unsigned int getBitNum() const override { return 256; }
};

class MappingType : public ReferenceType {
  TypePtr KeyType;
  TypePtr ValueType;

public:
  MappingType(TypePtr &&KT, TypePtr &&VT)
      : ReferenceType(DataLocation::Storage), KeyType(std::move(KT)),
        ValueType(std::move(VT)) {}

  TypePtr getKeyType() const { return KeyType; }
  TypePtr getValueType() const { return ValueType; }

  Category getCategory() const override { return Category::Mapping; }
  std::string getName() const override { return "mapping"; }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override {
    assert(false && "mapping is not allowed here");
    __builtin_unreachable();
  }
  unsigned getStorageSize() const override { return 32; }
};

class ArrayType : public ReferenceType {
  TypePtr ElementType;
  std::optional<llvm::APInt> Length; ///< Length of the array
public:
  // dynamic-sized array
  ArrayType(TypePtr ET, DataLocation Loc)
      : ReferenceType(Loc), ElementType(ET) {}
  // fix-sized array
  ArrayType(TypePtr ET, llvm::APInt L, DataLocation Loc)
      : ReferenceType(Loc), ElementType(ET), Length(L) {}

  TypePtr getElementType() const { return ElementType; }

  bool isDynamicSized() const { return !Length.has_value(); }
  const llvm::APInt &getLength() const {
    assert(!isDynamicSized());
    return *Length;
  }
  Category getCategory() const override { return Category::Array; }
  std::string getName() const override {
    std::string ArrLength;
    if (!isDynamicSized())
      ArrLength = std::to_string(getLength().getZExtValue());
    return ElementType->getName() + "[" + ArrLength + "]";
  }
  bool isDynamic() const override {
    if (isDynamicSized()) {
      return true;
    } else {
      return getElementType()->isDynamic();
    }
  }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override {
    if (isDynamicSized()) {
      return 32;
    } else {
      return getElementType()->getABIStaticSize() *
             getLength().getLimitedValue();
    }
  }
  unsigned getStorageSize() const override {
    if (isDynamicSized()) {
      return 32;
    } else {
      unsigned BitPerElement = getElementType()->getBitNum();
      unsigned ElementPerSlot = 256 / BitPerElement;
      unsigned Length = getLength().getLimitedValue();
      return (Length / ElementPerSlot + (Length % ElementPerSlot != 0)) * 32;
    }
  }
};

class FunctionType : public Type {
  std::vector<TypePtr> ParamTypes;
  std::vector<TypePtr> ReturnTypes;
  std::shared_ptr<const std::vector<std::string>> ParamNames;

public:
  FunctionType(std::vector<TypePtr> &&PTys, std::vector<TypePtr> &&RTys,
               std::shared_ptr<std::vector<std::string>> PNames = nullptr)
      : ParamTypes(std::move(PTys)), ReturnTypes(std::move(RTys)),
        ParamNames(PNames) {}

  const std::vector<TypePtr> &getParamTypes() const { return ParamTypes; }
  const std::vector<TypePtr> &getReturnTypes() const { return ReturnTypes; }
  std::shared_ptr<const std::vector<std::string>> getParamNames() const {
    return ParamNames;
  }

  Category getCategory() const override { return Category::Function; }
  std::string getName() const override { return "function"; }
  bool isDynamic() const override { return false; }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override { return 32; }
  bool isEqual(Type const &Ty) const override {
    if (!Type::isEqual(Ty)) {
      return false;
    }
    auto Compare = [](auto const &A, auto const &B) { return A->isEqual(*B); };
    auto const &T = static_cast<FunctionType const &>(Ty);
    if (!std::equal(getParamTypes().cbegin(), getParamTypes().cend(),
                    T.getParamTypes().cbegin(), T.getParamTypes().cend(),
                    Compare)) {
      return false;
    }
    if (!std::equal(getReturnTypes().cbegin(), getReturnTypes().cend(),
                    T.getReturnTypes().cbegin(), T.getReturnTypes().cend(),
                    Compare)) {
      return false;
    }
    return true;
  }
};

class TupleType : public Type {
protected:
  std::vector<TypePtr> ElementTypes;

public:
  TupleType(std::vector<TypePtr> &&ETys) : ElementTypes(std::move(ETys)) {}

  std::vector<TypePtr> &getElementTypes() { return ElementTypes; }
  const std::vector<TypePtr> &getElementTypes() const { return ElementTypes; }
  bool isImplicitlyConvertibleTo(Type const &_other) const override;
  bool isExplicitlyConvertibleTo(Type const &_convertTo) const override;

  Category getCategory() const override { return Category::Tuple; }
  std::string getName() const override { return "tuple"; }
  std::string getSignatureEncoding() const override {
    std::string Signature = "(";
    bool first = true;
    for (auto ET : ElementTypes) {
      if (!first)
        Signature += ",";
      first = false;
      Signature += ET->getSignatureEncoding();
    }
    Signature += ")";
    return Signature;
  };
  bool isDynamic() const override {
    for (const auto &ETys : ElementTypes) {
      if (ETys == nullptr || ETys->isDynamic()) {
        return true;
      }
    }
    return false;
  }
  virtual bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override {
    unsigned Size = 0;
    for (const auto &ETys : ElementTypes) {
      if (!ETys) {
        // make as a empty string
        // head(x) = enc( uint256(...) )
        Size += 32;
        // tail(x) = enc("")
        Size += 32;
      } else if (ETys->isDynamic()) {
        // head(x) = enc( uint256(...) )
        Size += 32;
        // tail(x) = enc(x)
        Size += ETys->getABIStaticSize();
      } else {
        // head(x) = enc(x)
        Size += ETys->getABIStaticSize();
        // tail(x) = enc("") #empty string
        Size += 32;
      }
    }
    return Size;
  }
  virtual unsigned getStorageSize() const override { return 32; }
  bool isEqual(Type const &Ty) const override {
    if (!Type::isEqual(Ty)) {
      return false;
    }
    auto const &T = static_cast<TupleType const &>(Ty);

    if (getElementTypes().size() != T.getElementTypes().size()) {
      return false;
    }

    bool Result = true;
    size_t EleNum = getElementTypes().size();

    for (size_t idx = 0; idx < EleNum; ++idx) {
      if (getElementTypes()[idx]) {
        if (!T.getElementTypes()[idx]) {
          Result = false;
          break;
        }
        Result &= getElementTypes()[idx]->isEqual(*T.getElementTypes()[idx]);
      } else {
        Result &= T.getElementTypes()[idx] == nullptr;
      }
    }
    return Result;
  }
};

class StructType : public TupleType {
  std::vector<std::string> ElementNames;
  llvm::StructType *Tp = nullptr;

public:
  StructType(std::vector<TypePtr> &&ET, std::vector<std::string> &&EN)
      : TupleType(std::move(ET)), ElementNames(std::move(EN)) {}
  Category getCategory() const override { return Category::Struct; }
  std::string getName() const override { return "struct"; }

  unsigned getStorageSize() const override {
    return getStoragePos(getElementSize());
  }
  void setLLVMType(llvm::StructType *T) {
    if (!Tp)
      Tp = T;
  }
  llvm::Type *getLLVMType() const { return Tp; }
  size_t getElementIndex(std::string Name) const {
    size_t i = 0;
    for (; i < ElementNames.size(); ++i) {
      if (ElementNames[i] == Name) {
        break;
      }
    }
    return i;
  }

  bool hasElement(std::string Name) const  {
    return getElementIndex(Name) < ElementNames.size();
  }
  
  size_t getStoragePos(size_t ElementIndex) const {
    size_t Pos = 0;
    for (size_t i = 0; i < ElementIndex; ++i) {
      Pos += ElementTypes[i]->getStorageSize() / 32;
    }
    return Pos;
  }
  size_t getElementSize() const { return ElementTypes.size(); }
  const std::vector<std::string> &getElementNames() const {
    return ElementNames;
  }
};

class ContractType : public Type {
  ContractDecl *D;

public:
  ContractType(ContractDecl *D = nullptr) : D(D) {}
  ContractDecl *getDecl() { return D; }
  const ContractDecl *getDecl() const { return D; }
  Category getCategory() const override { return Category::Contract; }
  unsigned int getBitNum() const override { return 160; }
  std::string getName() const override { return "contract"; }
  std::string getSignatureEncoding() const override { return "address"; }
  bool isDynamic() const override {
    assert(false && "contract is not allowed here");
    __builtin_unreachable();
  }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override {
    assert(false && "contract is not allowed here");
    __builtin_unreachable();
  }
};

class UnresolveType : public Type {
  llvm::StringRef IdentifierName;

public:
  UnresolveType(llvm::StringRef IdentifierName)
      : IdentifierName(IdentifierName) {}
  Category getCategory() const override { return Category::Unknow; }
  std::string getName() const override { return "Unknow"; }
  bool isDynamic() const override {
    assert(false && "UnresolveType is not allowed here");
    __builtin_unreachable();
  }
  bool shouldEndianLess() const override { return false; }
  unsigned getABIStaticSize() const override {
    assert(false && "UnresolveType is not allowed here");
    __builtin_unreachable();
  }
  llvm::StringRef getIdentifierName() const { return IdentifierName; }
};

} // namespace soll
