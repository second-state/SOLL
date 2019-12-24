// RUN: %soll -lang=Yul %s
// REQUIRES: YulFull
{
  sstore(0, returndatasize())
}
// ====
// EVMVersion: >=byzantium
// ----
// Trace:
// Memory dump:
// Storage dump:
