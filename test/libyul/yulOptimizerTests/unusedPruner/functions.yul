// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    function f() { let a := 1 }
    function g() { f() }
}
// ====
// step: unusedPruner
// ----
// { }
