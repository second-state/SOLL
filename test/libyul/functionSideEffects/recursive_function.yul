// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
    function a() { a() }
}
// ----
// : movable, sideEffectFree, sideEffectFreeIfNoMSize
// a:
