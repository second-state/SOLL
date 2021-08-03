// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --lang=Yul --libraries "file.sol:Math:0x111122223333444455556666777788889999aAaa" %s
{
    let x := linkersymbol("file.sol:Math")
}