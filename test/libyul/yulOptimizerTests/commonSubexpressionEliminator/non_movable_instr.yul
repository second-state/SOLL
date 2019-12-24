// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := mload(1)
    let b := mload(1)
}
// ====
// step: commonSubexpressionEliminator
// ----
// {
//     let a := mload(1)
//     let b := mload(1)
// }
