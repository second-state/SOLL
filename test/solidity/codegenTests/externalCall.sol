// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// REQUIRES: namedcall
// RUN: %soll %s
pragma solidity ^0.5.0;

contract A {
    function g(uint key, uint value, string str) public returns (uint, uint) {
        return (key + value, key * value);
    }

}

contract B {
    A contract_A;

    function f() public {
        uint aaa;
        uint bbb;
        (aaa, bbb) = contract_A.g({str: "strstr", value: 2, key: 3});
    }

}