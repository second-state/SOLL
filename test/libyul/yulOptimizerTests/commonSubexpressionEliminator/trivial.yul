// RUN: %soll -lang=Yul %s
// REQUIRES: codesize
{
    let a := mul(1, codesize())
    let b := mul(1, codesize())
}
// ====
// step: commonSubexpressionEliminator
// ----
// {
//     let a := mul(1, codesize())
//     let b := a
// }
