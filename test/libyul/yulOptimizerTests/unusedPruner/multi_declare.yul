// RUN: %soll -lang=Yul %s
// REQUIRES: returnTuple
{
    function f() -> x, y { }
    let a, b := f()
}
// ====
// step: unusedPruner
// ----
// { }
