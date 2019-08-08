pragma solidity ^ 0.5.0;

contract Expr_LRValue {
  function binary_operator(uint64 a, uint64 b) public pure returns(uint64) {
    a = b + 1;
    a = 2 - b;
    a = b * 3;
    a = b / 4;
    a = b % 5;
    a += b;
    a -= b;
    a *= 3;
    a /= b;
    a %= b;

    1 && 1;
    a && 1;
    a || a;
    1 || 1;

    a << 2;
    2 >> a;

    2 | a;
    2 & 2;

    return a;
  }
  function unary_operator() public pure returns(uint64) {
    uint64 a = 1;
    +a;
    +1;
    -a;
    -1;
    !a;
    !1;
    return a;
  }
}