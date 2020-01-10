// RUN: %soll -lang=Yul %s
// REQUIRES: log1
{
  log1(0x20, 0x40, 0x60)
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
