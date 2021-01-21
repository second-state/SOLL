// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;
pragma experimental ABIEncoderV2;

contract ABI_encoding {
    uint a;
    int16 b;
    address c;
    bool d;
    string s1;
    string s2;
    bytes b1;
    bytes b2;
    bytes4 e;
    int16[7][] Arr1;
    int32[][2] Arr2;
    int[][2][] Arr3;
    int[3][2][] Arr4;
    int64[] Arr5;
    function Jinkela() public payable returns(uint){
        return 12341234;
    }

    function encode() public payable returns(bytes memory){
        return abi.encode(a,s1,b,abi.encode(a,b,c,d,e),Jinkela(),b1,d,s2,e);
    }

    function encodePacked() public payable returns(bytes memory){
        return abi.encodePacked(a,s1,b,abi.encodePacked(a,b,c,d,e),Jinkela(),b1,d,s2,e);
    }
    function encodeArray() public payable returns(bytes){
        return abi.encode(Arr1, Arr2, Arr3, Arr5);
    }

    function encodePackedArray() public payable returns(bytes){
        return abi.encodePacked(Arr1, Arr4, Arr5);
    }

    struct ST{
        uint a;
        int16 b;
        address c;
        bool d;
        string s1;
        string s2;
        bytes b1;
        bytes b2;
        bytes8 e;
        int16[7][] Arr1;
        int32[][2] Arr2;
        int[][2][] Arr3;
        int[3][2][] Arr4;
        int64[] Arr5;
    }
    ST st;
    function encodeStruct() public payable returns(bytes){
        return abi.encode(st);
    }
    function encodeWithSelector() public payable returns(bytes){
        return abi.encodeWithSelector(e, st);
    }
    function encodeWithSignature() public payable returns(bytes){
        return abi.encodeWithSignature("Jinkela", st);
    }
}