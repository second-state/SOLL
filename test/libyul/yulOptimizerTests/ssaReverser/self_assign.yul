// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := calldataload(0)
    a := a
}
// ====
// step: ssaReverser
// ----
// { let a := calldataload(0) }
