// RUN: %soll -lang=Yul %s
{ if 0 { mstore(0, 0) } }
// ====
// step: structuralSimplifier
// ----
// { }
