pragma solidity ^0.5.0;

contract C {
    uint c;
    function foo() public {
        c = 3;
        uint b = c;
        c = b;
    }
}

