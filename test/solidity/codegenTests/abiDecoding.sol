// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;
pragma experimental ABIEncoderV2;

contract ABI_decoding {
    function Jinkela() public payable returns(uint){
        int a;
        int16 b;
        address c;
        bool d;
        string s1;
        bytes b1;
        bytes BBBB;
        (a,b,c,d,s1,b1) = abi.decode(BBBB, (int,int16,address,bool,string,bytes));
    }
}