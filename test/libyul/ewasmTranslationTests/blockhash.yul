// RUN: %soll -lang=Yul %s
// REQUIRES: blockhash
{
  sstore(0, blockhash(1))
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
