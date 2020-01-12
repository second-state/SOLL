// RUN: %soll -lang=Yul %s
{
    let x
    if x { mstore(0, 0) }
}
// ====
// step: structuralSimplifier
// ----
// { let x }
