// RUN: %soll -lang=Yul %s
// REQUIRES: mload
// REQUIRES: mstore
{
  {
     let a := 0x20
     mstore(a, 2)
  }
  let a
  mstore(a, 3)
}
// ----
// Trace:
// Memory dump:
//      0: 0000000000000000000000000000000000000000000000000000000000000003
//     20: 0000000000000000000000000000000000000000000000000000000000000002
// Storage dump:
