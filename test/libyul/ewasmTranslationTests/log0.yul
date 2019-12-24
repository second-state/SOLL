// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  log0(0x20, 0x40)
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
