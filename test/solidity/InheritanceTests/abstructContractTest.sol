// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
// CHECK: The contract with implemented part can not be emited

pragma solidity >=0.6.0 <0.9.0;

abstract contract Test {
    function f() public virtual returns (bytes32);
}
