// RUN: %soll -lang=Yul %s
// REQUIRES: mstore
{
    let x
    if x { mstore(0, 0) }
}
// ====
// step: structuralSimplifier
// ----
// { let x }
