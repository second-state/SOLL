// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract CallExpr {
    function redleaf(uint64 a, uint64 b) public pure {
        uint64 x = pikachu(a, a, b);
    }
    function pikachu(uint64 a, uint64 b, uint64 c) public pure returns (uint64) {
        uint64 d = a + b + c;
        return d;
    }
    function get() public pure returns(int,string memory) {
        int a = 2;
        return (a,"XXXXX");
    }
    function Jinkela() public pure{
        int a;
        string memory b;
        (a, b) = get();
    }
}
