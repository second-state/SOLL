// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let a := 1
    pop(a)
}
// ====
// step: unusedPruner
// ----
// { }
