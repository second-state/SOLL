// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --runtime --action=EmitLLVM %s
pragma solidity ^0.5.0;

contract A {
    int gggg;
    constructor() public payable {
        g({str: "XXXXXDDDDDD", value: 2, key: 3});
        gggg = 7221;
    }
    function g(uint key, uint value, string str) public {
        key = key + value;
    }
    function f() public returns(int){
        return 2712;
    }

}

contract B {
    A contract_A;

    function f() public returns(int){
        contract_A.g({str: "strstr", value: 2, key: 3});
        return 2217;
    }

}