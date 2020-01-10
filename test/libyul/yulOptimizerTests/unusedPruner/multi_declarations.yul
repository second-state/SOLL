// RUN: %soll -lang=Yul %s
// REQUIRES: declareMulti
{
    let x, y
}
// ====
// step: unusedPruner
// ----
// { }
