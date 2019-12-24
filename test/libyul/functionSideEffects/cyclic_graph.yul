// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    function a() { b() }
    function b() { c() }
    function c() { b() }
}
// ----
// : movable, sideEffectFree, sideEffectFreeIfNoMSize
// a:
// b:
// c:
