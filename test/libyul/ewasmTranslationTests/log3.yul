// RUN: %soll -lang=Yul %s
// REQUIRES: log3
{
  log3(0x20, 0x40, 0x60, 0x80, 0xa0)
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
