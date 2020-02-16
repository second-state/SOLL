// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
// The shell prevents SIGABRT from propogating to `not`

pragma solidity ^0.5.0;

contract C {
// CHECK: 'enum' is not yet supported.
    enum Alphabet { A, B }
    function test() public returns (Alphabet) {
        return Alphabet.A;
    }
}
