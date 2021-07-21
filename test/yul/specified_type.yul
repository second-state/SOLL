// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul %s
{
    let x := and("abc":uint32, add(3:uint256, true:uint256))
    let zero:uint32 := 0:uint32
    let Jinkela:bool
    Jinkela := true
}
