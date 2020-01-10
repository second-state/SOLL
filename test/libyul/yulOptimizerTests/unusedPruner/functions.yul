// RUN: %soll -lang=Yul %s
{
    function f() { let a := 1 }
    function g() { f() }
}
// ====
// step: unusedPruner
// ----
// { }
