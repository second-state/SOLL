// RUN: %soll -lang=Yul %s
// REQUIRES: coinbase
{
  sstore(0, coinbase())
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
