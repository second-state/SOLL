// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "memory" {
  code {
    let temp := 0

    mstore8(0x90, 0xff)
    temp := mload(0x80)
    sstore(0, temp)
    sstore(1, msize())

    mstore(0x90, 0xff)
    temp := mload(0xa0)
    sstore(2, temp)
    sstore(3, msize())
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Memory relative built-in functions"
}
