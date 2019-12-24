// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := 1
    let b := 1
    mstore(0, 1)
}
// ====
// step: unusedPruner
// ----
// { mstore(0, 1) }
