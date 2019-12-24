// RUN: %soll -lang=Yul %s
{
  let a
}
// ====
// step: varDeclInitializer
// ----
// { let a := 0 }
