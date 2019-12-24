// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{ if mload(0) {} }
// ====
// step: controlFlowSimplifier
// ----
// { pop(mload(0)) }
