// RUN: %soll -lang=Yul %s
{
    function a() { b() }
    function b() { a() }
}
// ----
// : movable, sideEffectFree, sideEffectFreeIfNoMSize
// a:
// b:
