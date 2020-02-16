// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
// The shell prevents SIGABRT from propragating to `not`

pragma solidity ^0.5.0;

contract C {
    constructor() public {
    }
// CHECK: multiple constructor defined
// CHECK: previous definition is here
    constructor() public {
    }
}
