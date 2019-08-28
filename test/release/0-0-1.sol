pragma solidity ^0.5.0;

contract SafeMath {
    function add(uint64 a, uint64 b) public pure returns (uint64) {
        uint64 c = a + b;
        require(c >= a, "SafeMath: addition overflow");

        return c;
    }

    function sub(uint64 a, uint64 b) public pure returns (uint64) {
        require(b <= a, "SafeMath: subtraction overflow");
        uint64 c = a - b;

        return c;
    }

    function mul(uint64 a, uint64 b) public pure returns (uint64) {
        if (a == 0) {
            return 0;
        }

        uint64 c = a * b;
        require(c / a == b, "SafeMath: multiplication overflow");

        return c;
    }

    function div(uint64 a, uint64 b) public pure returns (uint64) {
        require(b > 0, "SafeMath: division by zero");
        uint64 c = a / b;
        return c;
    }

    function mod(uint64 a, uint64 b) public pure returns (uint64) {
        require(b != 0, "SafeMath: modulo by zero");
        return a % b;
    }
}
