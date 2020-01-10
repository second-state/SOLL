// RUN: %soll -lang=Yul %s
// REQUIRES: codesize
// REQUIRES: codecopy
// REQUIRES: create
{
  let size := codesize()
  codecopy(0, 0, size)
  sstore(0, create(42, 0, size))
}
// ----
// Trace:
//   INVALID()
// Memory dump:
//      0: 0000000000000000000000000000000000000000000000000000000000000014
//     40: 636f6465636f6465636f6465636f6465636f6465000000000000000000000000
// Storage dump:
