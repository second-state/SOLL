// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
// REQUIRES: tuple
object "tuple" {
  code {
    function tuple(x, y) -> rx, ry {
        rx := x
        ry := y
    }

    function swap(x, y) -> rx, ry {
        rx, ry := tuple(y, x)
    }

    let a, b := swap(2, 1)
    sstore(0x0a, a)
    sstore(0x0b, b)
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Tuple is identifier list in Yul."
}
