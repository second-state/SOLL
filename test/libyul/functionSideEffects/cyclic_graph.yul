// RUN: %soll -lang=Yul %s
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
