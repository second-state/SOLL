// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s

pragma solidity >=0.6.0 <0.9.0;

contract A { function f() public pure{} }
contract B is A {}
contract C is A {}
// No explicit override required
contract D is B, C {}
