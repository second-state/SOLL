// RUN: %soll -lang=Yul %s
// REQUIRES: mstore
// REQUIRES: revert
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
