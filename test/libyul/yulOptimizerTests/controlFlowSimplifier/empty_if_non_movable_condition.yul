// RUN: %soll -lang=Yul %s
// REQUIRES: mload
// REQUIRES: pop
{ if mload(0) {} }
// ====
// step: controlFlowSimplifier
// ----
// { pop(mload(0)) }
