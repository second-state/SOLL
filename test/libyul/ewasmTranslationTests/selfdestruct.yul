// RUN: %soll -lang=Yul %s
// REQUIRES: selfdestruct
{
  selfdestruct(not(0))
  invalid()
}
// ----
// Trace:
//   SELFDESTRUCT()
// Memory dump:
//     40: ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
// Storage dump:
