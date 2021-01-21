// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

enum UnaryOperatorKind {
  UO_PostInc,
  UO_PostDec,

  UO_PreInc,
  UO_PreDec,
  UO_AddrOf,
  UO_Deref,
  UO_Plus,
  UO_Minus,
  UO_Not,
  UO_LNot,
  UO_IsZero,
  UO_Undefined,
};

// binary operators
// exclude []
enum BinaryOperatorKind {
  // Exponential
  BO_Exp,
  // Multiplicative operators.
  BO_Mul,
  BO_Div,
  BO_Rem,
  // Additive operators.
  BO_Add,
  BO_Sub,
  // Bitwise shift operators.
  BO_Shl,
  BO_Shr,
  // Bitwise AND operator.
  BO_And,
  // Bitwise XOR operator.
  BO_Xor,
  // Bitwise OR operator.
  BO_Or,
  // Asm Bitwise AND operator.
  BO_AsmAnd,
  // Asm  Bitwise XOR operator.
  BO_AsmXor,
  // Asm Bitwise OR operator.
  BO_AsmOr,
  // Relational operators.
  BO_LT,
  BO_GT,
  BO_LE,
  BO_GE,
  // Equality operators.
  BO_EQ,
  BO_NE,
  // Logical AND operator.
  BO_LAnd,
  // Logical OR operator.
  BO_LOr,
  // Logical XOR operator.
  BO_LXor,
  // Assignment operators.
  BO_Assign,
  BO_MulAssign,
  BO_DivAssign,
  BO_RemAssign,
  BO_AddAssign,
  BO_SubAssign,
  BO_ShlAssign,
  BO_ShrAssign,
  BO_AndAssign,
  BO_XorAssign,
  BO_OrAssign,
  BO_Comma,
  BO_Undefined,
};

enum CastKind {
  None,
  IntegralCast,
  FixedBytesCast,
  // only for ImplicitCast
  LValueToRValue,
  // [TODO] Rename
  TypeCast
};

} // namespace soll
