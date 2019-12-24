// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{ if 0 { mstore(0, 0) } }
// ====
// step: structuralSimplifier
// ----
// { }
