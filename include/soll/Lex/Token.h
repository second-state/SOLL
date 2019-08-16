// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/SourceLocation.h"
#include "soll/Basic/TokenKinds.h"

namespace soll {

class IdentifierInfo;

class Token {
  unsigned Loc;
  tok::TokenKind Kind;
  unsigned Length;

  /// PtrData - This is a union of four different pointer types, which depends
  /// on what type of token this is:
  ///  Identifiers, keywords, etc:
  ///    This is an IdentifierInfo*, which contains the uniqued identifier
  ///    spelling.
  ///  Literals:  isLiteral() returns true.
  ///    This is a pointer to the start of the token in a text buffer, which
  ///    may be dirty (have trigraphs / escaped newlines).
  ///  Eof:
  //     This is a pointer to a Decl.
  ///  Other:
  ///    This is null.
  void *PtrData;

public:
  tok::TokenKind getKind() const { return Kind; }
  void setKind(tok::TokenKind K) { Kind = K; }

  bool is(tok::TokenKind K) const { return Kind == K; }
  bool isNot(tok::TokenKind K) const { return Kind != K; }
  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template <typename... Ts>
  bool isOneOf(tok::TokenKind K1, tok::TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  bool isAnyIdentifier() const { return tok::isAnyIdentifier(getKind()); }

  bool isLiteral() const { return tok::isLiteral(getKind()); }

  // [TODO] Need refine follow clang architecture, reference BuiltinTypes.def
  // and OperationKinds.def
  bool isElementaryTypeName() const { return tok::kw_address <= getKind(); }
  bool isUnaryOp() const {
    return (tok::exclaim <= getKind() && getKind() <= tok::minusminus) ||
           isOneOf(tok::kw_delete, tok::plus, tok::minus);
  }
  bool isCountOp() const { return isOneOf(tok::plusplus, tok::minusminus); }

  SourceLocation getLocation() const {
    return SourceLocation::getFromRawEncoding(Loc);
  }
  void setLocation(SourceLocation L) { Loc = L.getRawEncoding(); }

  unsigned getLength() const { return Length; }
  void setLength(unsigned Len) { Length = Len; }

  SourceLocation getLastLoc() const { return getLocation(); }

  SourceLocation getEndLoc() const {
    return getLocation().getLocWithOffset(getLength());
  }

  const char *getName() const { return tok::getTokenName(Kind); }

  IdentifierInfo *getIdentifierInfo() const {
    assert(isNot(tok::raw_identifier) &&
           "getIdentifierInfo() on a tok::raw_identifier token!");
    if (isLiteral())
      return nullptr;
    if (is(tok::eof))
      return nullptr;
    return reinterpret_cast<IdentifierInfo *>(PtrData);
  }
  void setIdentifierInfo(IdentifierInfo *II) {
    PtrData = reinterpret_cast<void *>(II);
  }

  llvm::StringRef getRawIdentifier() const {
    assert(is(tok::raw_identifier));
    return llvm::StringRef(reinterpret_cast<const char *>(PtrData),
                           getLength());
  }
  void setRawIdentifierData(const char *Ptr) {
    assert(is(tok::raw_identifier));
    PtrData = const_cast<char *>(Ptr);
  }

  const char *getLiteralData() const {
    assert(isLiteral() && "Cannot get literal data of non-literal");
    return reinterpret_cast<const char *>(PtrData);
  }
  void setLiteralData(const char *Ptr) {
    assert(isLiteral() && "Cannot set literal data of non-literal");
    PtrData = const_cast<char *>(Ptr);
  }
};

} // namespace soll

namespace llvm {
template <> struct isPodLike<soll::Token> { static const bool value = true; };
} // end namespace llvm
