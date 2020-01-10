// RUN: %soll -lang=Yul %s
// REQUIRES: returnTuple
{
    let a
    let b
    function f() -> x, y { }
    a, b := f()
}
// ====
// step: unusedPruner
// ----
// {
//     let a
//     let b
//     function f() -> x, y
//     { }
//     a, b := f()
// }
