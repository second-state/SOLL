// RUN: %soll -lang=Yul %s
// REQUIRES: revert
// REQUIRES: mstore
{
    {
      revert(0, 0)
    }
    mstore(0, 0)
}
// ====
// step: deadCodeEliminator
// ----
// {
//     { revert(0, 0) }
//     mstore(0, 0)
// }
