// RUN: %soll %s
pragma solidity > 0.4.0 <= 0.7.0;

contract EXP {
  function exp(int base, uint exp) public pure returns(int) {
    return base ** exp;
  }
  function exp64(int64 base, uint64 exp) public pure returns(int64) {
    return base ** exp;
  }
}
