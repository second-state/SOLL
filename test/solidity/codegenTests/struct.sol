// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract Struct {
    struct A{
        int a;
        string b;
        int32[10] c;
        int32[] d;
    }
    struct B{
        A a;
        //mapping(int=>int) m;
    }
    struct C{
      B b;
    }
    B b;
    A a;
    C c;
    int32[] d;
    /*
    function Jinkela() public payable returns(A memory){
        A newA;
        newA.c[2] = 10;
        newA.b = "1bd2";
        return newA;
    }
    */
    function encode() public payable returns(int32){
        c.b.a.a = 6;
        //b.m[71] = 22;
        //a.d[2] = 8;
        //a.c[2] = 8;
        //return a.c[2];
    }
}