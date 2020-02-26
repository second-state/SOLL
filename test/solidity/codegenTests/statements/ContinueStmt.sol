// SPDX-License-Identifier: GPL-3.0-or-later
// REQUIRES: implicitCast
// RUN: %soll %s
pragma solidity >0.4.0 <=0.7.0;

contract C {
  function ContinueStmt() public {
    int a;
    while(a > 1) {
      if (1 > a) {
        if (3 > a) {
          continue;
        }
        a + 3;
        continue;
      }
      continue;
    }
  }
}
