// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "Alias" {
  code {
    let base := 1
    base := addu256(base, 1) // base += 1. Now base = 2
    base := add(base, 1) // base += 1. Now base = 3
    base := subu256(base, 1) // base -= 1. Now base = 2
    base := sub(base, 1) // base -= 1. Now base = 1
    base := mulu256(base, 2) // base *= 2. Now base = 2
    base := mul(base, 2) // base *= 2. Now base = 4
    base := divu256(base, 2) // base /= 2. Now base = 2
    base := div(base, 1) // base /= 1. Now base = 2
    base := divs256(base, 1) // base /= 1. Now base = 2
    base := sdiv(base, 2) // base /= 2. Now base = 1
    sstore(0, base)
    let a := 10
    if lts256(a, 100) {
        sstore(4, 1)
    }
    if slt(a, 100) {
        sstore(8, 1)
    }
    if gts256(a, 1) {
        sstore(12, 1)
    }
    if sgt(a, 1) {
        sstore(16, 1)
    }
  }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Yul Alias"
}
