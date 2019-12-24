// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    function a() { b() }
    function b() { a() }
}
// ----
// : movable, sideEffectFree, sideEffectFreeIfNoMSize
// a:
// b:
