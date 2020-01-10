// RUN: %soll -lang=Yul %s
// REQUIRES: calldataload
{
    let a := calldataload(0)
    a := a
}
// ====
// step: ssaReverser
// ----
// { let a := calldataload(0) }
