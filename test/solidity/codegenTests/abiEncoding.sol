// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract ABI_encoding {
    uint a;
    int16 b;
    address c;
    bool d;
    string s1;
    string s2;
    bytes b1;
    bytes b2;
    bytes8 e;
    function Jinkela() public payable returns(uint){
        return 12341234;
    }

    function encode() public payable returns(bytes){
        return abi.encode(a,s1,b,abi.encode(a,b,c,d,e),Jinkela(),b1,d,s2,e);
    }

    function encodePacked() public payable returns(bytes){
        return abi.encodePacked(a,s1,b,abi.encodePacked(a,b,c,d,e),Jinkela(),b1,d,s2,e);
    }
}