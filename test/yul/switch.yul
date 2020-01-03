// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "switch" {
  code {
    switch 1
    case 1 {
      sstore(0x11, 1)
    }
    case 2 {
      sstore(0x12, 2)
    }
    default {
      sstore(0x13, 0xff)
    }

    switch 2
    case 1 {
      sstore(0x21, 1)
    }

    switch 3
    default {
      sstore(0x33, 0xff)
    }
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Switch is under implementation."
}
