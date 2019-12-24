// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{ let a := add(7, sub(mload(0), 7)) }
// ====
// step: expressionSimplifier
// ----
// { let a := mload(0) }
