// RUN: %soll -lang=Yul %s
{
    let a
    a := 1
    a := 2
}
// ====
// step: redundantAssignEliminator
// ----
// { let a }
