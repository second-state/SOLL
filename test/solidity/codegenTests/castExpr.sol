// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract CastExpr {
    address a;
    address b;
    uint e;
    uint f;
    uint256 g;
    uint256 h;
    uint32 i;
    uint32 j;
    function castExpr() public {
        b = address(this);
        a = address(b);
        e = 7189324;
        //i = 209528;
        i = uint32(209528);
        j = uint32(i);
        g = uint256(i);
        //h = 115792089237316195423570985008687907853269984665640564039457584007913129639935;
        g = uint256(h);
        f = uint(h);
    }
}