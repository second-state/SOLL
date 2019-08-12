pragma solidity ^0.5.0;

contract CallExpr {
    function redleaf(uint64 a, uint64 b) public pure {
        uint64 x = pikachu(a, a, b);
    }
    function pikachu(uint64 a, uint64 b, uint64 c) public pure returns (uint64) {
        uint64 d = a + b + c;
        return d;
    }
}