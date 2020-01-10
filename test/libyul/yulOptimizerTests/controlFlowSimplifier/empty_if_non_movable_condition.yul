// RUN: %soll -lang=Yul %s
// REQUIRES: pop
// REQUIRES: mload
{ if mload(0) {} }
// ====
// step: controlFlowSimplifier
// ----
// { pop(mload(0)) }
