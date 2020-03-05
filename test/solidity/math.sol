// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll --runtime --action=EmitLLVM %s
pragma solidity ^0.5.0;

contract Math {
    function add(uint256 a, uint256 b) public pure returns (uint256) {
        return a + b;
    }

    function add(int256 a, int256 b) public pure returns (int256) {
        return a + b;
    }

    function sub(uint256 a, uint256 b) public pure returns (uint256) {
        return a - b;
    }

    function sub(int256 a, int256 b) public pure returns (int256) {
        return a - b;
    }

    function mul(uint256 a, uint256 b) public pure returns (uint256) {
        return a * b;
    }

    function mul(int256 a, int256 b) public pure returns (int256) {
        return a * b;
    }

    function div(uint256 a, uint256 b) public pure returns (uint256) {
        return a / b;
    }

    function div(int256 a, int256 b) public pure returns (int256) {
        return a / b;
    }

    function mod(uint256 a, uint256 b) public pure returns (uint256) {
        return a % b;
    }

    function mod(int256 a, int256 b) public pure returns (int256) {
        return a % b;
    }

    function shl(uint256 a, uint256 b) public pure returns (uint256) {
        return a << b;
    }

    function shl(int256 a, int256 b) public pure returns (int256) {
        return a << b;
    }

    function shr(uint256 a, uint256 b) public pure returns (uint256) {
        return a >> b;
    }

    function shr(int256 a, int256 b) public pure returns (int256) {
        return a >> b;
    }
}
