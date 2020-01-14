// SPDX-License-Identifier: GPL-3.0-or-later
// RUN: %soll -action=ASTDump %s | FileCheck %s --match-full-lines
contract C {
    function f() {
// CHECK: NumberLiteral 2
        2 wei;
// CHECK: NumberLiteral 3000000000000
        3 szabo;
// CHECK: NumberLiteral 4000000000000000
        4 finney;
// CHECK: NumberLiteral 5000000000000000000
        5 ether;
// CHECK: NumberLiteral 6
        6 seconds;
// CHECK: NumberLiteral 420
        7 minutes;
// CHECK: NumberLiteral 28800
        8 hours;
// CHECK: NumberLiteral 777600
        9 days;
// CHECK: NumberLiteral 6048000
        0xa weeks;
    }
}
