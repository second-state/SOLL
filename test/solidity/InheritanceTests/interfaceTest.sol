// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: not sh -c '%soll %s; exit $?' |& FileCheck %s
// CHECK: Interface can not be emited
interface if1 {
    function todo1() external;
    function todo2(int) virtual external;
}

interface if2 is if1 {
    function todo1() override external;
    function todo3() external;
}

contract cont1 is if2 {
    function todo1() override external {}
    function todo2(int) override external {}
    function todo3() override external {}
}
