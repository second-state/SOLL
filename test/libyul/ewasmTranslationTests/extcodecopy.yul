// RUN: %soll -lang=Yul %s
// REQUIRES: extcodecopy
// REQUIRES: address
// REQUIRES: extcodesize
// REQUIRES: mload
{
  extcodecopy(address(), 0x100, 0, extcodesize(address()))
  sstore(0, mload(0x100))
}
// ----
// Trace:
//   INVALID()
// Memory dump:
//      0: 0000000000000000000000000000000011111111000000000000000000000000
// Storage dump:
