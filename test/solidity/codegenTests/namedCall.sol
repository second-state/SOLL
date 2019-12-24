// REQUIRES: namedcall
// RUN: %soll %s
pragma solidity ^0.5.0;

contract C {

    function f() public {
        g({value: 2, key: 3});
    }

    function g(uint key, uint value) public {
        data = data + value;
    }

}
