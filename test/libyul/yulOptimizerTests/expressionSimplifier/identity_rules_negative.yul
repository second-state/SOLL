// RUN: %soll -lang=Yul %s
// REQUIRES: calldataload
{ let a := sub(calldataload(1), calldataload(0)) }
// ====
// step: expressionSimplifier
// ----
// {
//     let a := sub(calldataload(1), calldataload(0))
// }
