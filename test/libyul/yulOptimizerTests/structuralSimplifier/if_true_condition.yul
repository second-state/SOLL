// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{ if 1 { mstore(0, 0) } }
// ====
// step: structuralSimplifier
// ----
// { mstore(0, 0) }
