// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
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
