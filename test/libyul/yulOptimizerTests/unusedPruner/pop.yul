// RUN: %soll -lang=Yul %s
// REQUIRES: pop
{
    let a := 1
    pop(a)
}
// ====
// step: unusedPruner
// ----
// { }
