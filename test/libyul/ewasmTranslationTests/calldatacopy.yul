// RUN: %soll -lang=Yul %s
// REQUIRES: calldatacopy
// REQUIRES: mload
{
  calldatacopy(0, 0, 32)
  sstore(0, mload(0))
}
// ----
// Trace:
// Memory dump:
// Storage dump:
