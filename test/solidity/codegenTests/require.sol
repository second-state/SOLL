pragma solidity ^0.5.0;

contract CallExpr {
    function redleaf(uint64 a, uint64 b) public pure returns (uint64) {
        uint64 c = a + b;
        require(c >= a, "SafeMath: addition overflow");
        assert(c >= a);
        revert("Revert Msg");
    }
}