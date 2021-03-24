// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s

contract Base {
// CHECK: functionOverridingFailTest.sol:7:5
// CHECK: virtual need be marked for zz
    function zz() {}
    function pp() virtual {}
}

contract A is Base {
    function zz() override {}
// CHECK: functionOverridingFailTest.sol:15:5
// CHECK: override need be marked for pp
    function pp() {}
}
