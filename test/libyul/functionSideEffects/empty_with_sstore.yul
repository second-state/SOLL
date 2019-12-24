// RUN: %soll -lang=Yul %s
{
    sstore(0, 1)
}
// ----
// : invalidatesStorage
