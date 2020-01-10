// RUN: %soll -lang=Yul %s
// REQUIRES: calldataload
{
  sstore(0, calldataload(0))
}
// ----
// Trace:
// Memory dump:
// Storage dump:
