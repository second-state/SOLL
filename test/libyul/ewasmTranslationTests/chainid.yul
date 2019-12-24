// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  sstore(0, chainid())
}
// ====
// EVMVersion: >=istanbul
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
