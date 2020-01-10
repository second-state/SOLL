// RUN: %soll -lang=Yul %s
// REQUIRES: calldatasize
{
  sstore(0, calldatasize())
}
// ----
// Trace:
// Memory dump:
// Storage dump:
