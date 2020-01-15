// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;
contract Service {
    mapping(string => string) accounts;
    function createAccount(string memory acc, string memory pw) public {
        accounts[acc] = pw;
    }
    function checkLogin(string memory acc, string memory pw) public view returns (bool) {
        if (keccak256(bytes(accounts[acc])) == keccak256(bytes(pw)))
            return true;
        return false;
    }
}