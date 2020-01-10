// RUN: %soll -lang=Yul %s
{
    function a() { a() }
}
// ----
// : movable, sideEffectFree, sideEffectFreeIfNoMSize
// a:
