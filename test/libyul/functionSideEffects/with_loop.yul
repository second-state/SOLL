// RUN: %soll -lang=Yul %s
// REQUIRES: pop
{
  function f() -> x { x := g() }
  function g() -> x { for {} 1 {} {} }
  pop(f())
}
// ----
// :
// f:
// g:
