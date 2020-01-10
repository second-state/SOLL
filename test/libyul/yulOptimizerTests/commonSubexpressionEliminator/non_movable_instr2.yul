// RUN: %soll -lang=Yul %s
// REQUIRES: gas
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
