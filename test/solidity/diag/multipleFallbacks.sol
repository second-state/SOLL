// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
// The shell prevents SIGABRT from propagating to `not`

pragma solidity ^0.5.0;

contract C {
    function() public {
    }
// CHECK: multiple fallback defined
// CHECK: previous definition is here
    function() public {
    }
}
