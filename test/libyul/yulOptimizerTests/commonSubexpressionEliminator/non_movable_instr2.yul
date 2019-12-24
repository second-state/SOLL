// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := gas()
    let b := gas()
}
// ====
// step: commonSubexpressionEliminator
// ----
// {
//     let a := gas()
//     let b := gas()
// }
