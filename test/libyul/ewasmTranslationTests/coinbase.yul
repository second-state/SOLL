// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  sstore(0, coinbase())
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
