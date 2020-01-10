// RUN: %soll -lang=Yul %s
// REQUIRES: mload
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
