// RUN: %soll %s
pragma solidity >0.4.0 <=0.7.0;

contract C {
  function BreakStmt() public {
    int a;
    while(a > 1) {
      if (1 > a) {
        if (3 > a) {
          break;
        }
        a + 3;
        break;
      }
      break;
    }
  }
}