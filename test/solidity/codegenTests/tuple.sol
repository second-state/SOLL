// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract Tuple {
    function test1() public payable {
        uint a;
        uint b;
        string c;

        int8 tmp = 20;
        (a,) = (123,);
        (a,b,c) = (1,tmp, "PPAP");
        (a, ,c) = (1,tmp, "PPAP");
        (a, ,c) = (1,   , "PPAP");
        ( , , ) = (a,"C","A");
        ( , , ) = (a,c  , );

        (a,b) = (b,a);
    }
}