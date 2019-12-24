// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    function f() -> x, y { }
    let a, b := f()
}
// ====
// step: unusedPruner
// ----
// { }
