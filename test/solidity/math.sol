pragma solidity ^0.5.0;

contract Math {
    function add(uint64 a, uint64 b) public pure returns (uint64) {
        return a + b;
    }

    function sub(uint64 a, uint64 b) public pure returns (uint64) {
        return a - b;
    }

    function mul(uint64 a, uint64 b) public pure returns (uint64) {
        return a * b;
    }

    function div(uint64 a, uint64 b) public pure returns (uint64) {
        return a / b;
    }

    function mod(uint64 a, uint64 b) public pure returns (uint64) {
        return a % b;
    }

    function shl(uint64 a, uint64 b) public pure returns (uint64) {
        return a << b;
    }

    function shr(uint64 a, uint64 b) public pure returns (uint64) {
        return a >> b;
    }
}
