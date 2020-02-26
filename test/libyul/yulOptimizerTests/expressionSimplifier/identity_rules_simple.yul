// RUN: %soll -lang=Yul %s
// REQUIRES: mload
{
    let a := mload(0)
    let b := sub(a, a)
}
// ====
// step: expressionSimplifier
// ----
// {
//     let a := mload(0)
//     let b := 0
// }
