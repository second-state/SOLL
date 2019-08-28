pragma solidity ^ 0.5.0;

contract Expr_LRValue {
  function binary_operator_assignment() public pure {
    // =, +=, -=, *=, /=, %=, <<=, >>=
    uint64 a;
    uint64 b;
    a = b;
    a += b;
    a -= b;
    a *= 3;
    a /= b;
    a %= b;
    b <<= 10;
    a >>= b;
    a += b = a;
  }
  function binary_operator_arithmetic() public pure {
    // +, -, *, /, %, <<, >>, ~
    uint64 a;
    uint64 b;
    a + b;
    a - b;
    a * b;
    a / b;
    a % b;
    a << b;
    a >> b;
    ~a;
  }
  function binary_operator_comparison() public pure {
    // >, <, >=, <=, ==, !=
    uint64 a;
    uint64 b;
    bool result;
    result = a > b;
    result = a < b;
    result = a >= b;
    result = a <= b;
    result = a == b;
    result = a != b;
  }
  function binary_operator_logical() public pure {
    // ||, &&
    bool a;
    bool b;
    a = a || b;
    b = a && b;
  }
  function binary_operator_bitwise() public pure {
    // &, |, ^
    uint64 a;
    uint64 b;
    a = a & b;
    b = a | b;
    a = a ^ b;
  }
  function unary_operator() public pure {
    // ++a, --a, a++, a--, -a, !a
    uint64 a;
    bool b;
    a = ++a;
    a = --a;
    a = a++;
    a = a--;
    a = -a;
    b = !b;
  }
}