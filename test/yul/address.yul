// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "address" {
    code {
        mstore(0x0, address())
        mstore(0x20, balance(address()))
        mstore(0x40, selfbalance())
        return(0x0, 0x40)
    }
  // Unreferenced data is not added to the assembled bytecode.
  data "str" "Basic functions of Yul address"
}
