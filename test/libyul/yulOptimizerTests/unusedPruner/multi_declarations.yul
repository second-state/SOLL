// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    let x, y
}
// ====
// step: unusedPruner
// ----
// { }
