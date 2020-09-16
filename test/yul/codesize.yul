// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
object "codesize" {
    code {
        mstore(0x20, extcodesize(address()))
        mstore(0x40, codesize())
        return(0x0, 0x60)
    }
    // Unreferenced data is not added to the assembled bytecode.
    data "str" "Basic functions of Yul codesize"
}

