// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  sstore(0, calldatasize())
}
// ----
// Trace:
// Memory dump:
// Storage dump:
