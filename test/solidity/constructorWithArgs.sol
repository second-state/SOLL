// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: not %soll %s |& FileCheck %s
// https://github.com/second-state/SOLL/blob/655b71a8f26b6b11b00d4cffd0f2e55e3c948323/lib/CodeGen/CodeGenModule.cpp#L969
// This checks constructor with arguments emit a proper not implemented diagnostic
pragma solidity ^0.5.0;

contract test {
    uint256 s;
    constructor(uint256 value) public {
// CHECK: Constructor with parameter is not yet supported
        s = value;
    }
    function() public {
    }
}
