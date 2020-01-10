// RUN: %soll -lang=Yul %s
// REQUIRES: pop
{
    let a := 1
    let b := keccak256(1, 1)
    sstore(0, msize())
}
// ====
// step: unusedPruner
// ----
// {
//     pop(keccak256(1, 1))
//     sstore(0, msize())
// }
