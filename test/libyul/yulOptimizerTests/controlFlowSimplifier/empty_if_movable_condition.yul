// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{ let a := mload(0) if a {} }
// ====
// step: controlFlowSimplifier
// ----
// {
//     let a := mload(0)
//     pop(a)
// }
