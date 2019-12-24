// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  log1(0x20, 0x40, 0x60)
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
