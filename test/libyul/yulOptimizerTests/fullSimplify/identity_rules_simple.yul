// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := mload(0)
    mstore(0, sub(a, a))
}
// ====
// step: fullSimplify
// ----
// { mstore(0, 0) }
