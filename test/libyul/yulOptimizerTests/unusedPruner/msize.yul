// RUN: %soll -lang=Yul %s
// REQUIRES: pop
// REQUIRES: mload
{
    let a := 1
    let b := mload(10)
    sstore(0, msize())
}
// ====
// step: unusedPruner
// ----
// {
//     pop(mload(10))
//     sstore(0, msize())
// }
