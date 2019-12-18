// RUN: %soll %s
pragma solidity ^0.5.0;

contract SafeMath {
    event Transfer(address indexed _from, address indexed _to, uint256 _value);
    function add(uint256 a, uint256 b) public returns (uint256) {
        uint256 c = a + b;
        require(c >= a, "SafeMath: addition overflow");
        emit Transfer(address(123), address(456), c);
        return c;
    }
}
