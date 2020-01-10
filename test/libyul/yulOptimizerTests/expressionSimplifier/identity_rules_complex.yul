// RUN: %soll -lang=Yul %s
// REQUIRES: calldataload
{ let a := sub(calldataload(0), calldataload(0)) }
// ====
// step: expressionSimplifier
// ----
// { let a := 0 }
