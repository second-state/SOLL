// RUN: %soll -lang=Yul %s
// REQUIRES: pc
{
  sstore(0, pc())
}
// ----
// Trace:
//   INVALID()
// Memory dump:
// Storage dump:
