// RUN: %soll -lang=Yul %s
// REQUIRES: datasize
// REQUIRES: datasizeSelf
// REQUIRES: objectInObject
object "main"
{
    code {
        sstore(0, datasize("main"))
        sstore(1, datasize("sub"))
    }
    object "sub" { code { sstore(0, 1) } }
}
// ----
// Trace:
// Memory dump:
// Storage dump:
//   0000000000000000000000000000000000000000000000000000000000000000: 0000000000000000000000000000000000000000000000000000000000000b64
//   0000000000000000000000000000000000000000000000000000000000000001: 0000000000000000000000000000000000000000000000000000000000000109
