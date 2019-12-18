// RUN: %soll %s
pragma solidity >0.4.0 <=0.7.0;

contract POWER {
	// evm takes seconds to run power(3,10000)
	function power(uint x, uint y) public pure returns(uint) {
		uint rtn = uint(1);
		for(uint i = uint(0); i < y; i += 1) {
			rtn = rtn * x;
		}
		return rtn;
	}
	function power64(uint64 x, uint64 y) public pure returns(uint64) {
		uint64 rtn = uint64(1);
		for(uint64 i = uint64(0); i < y; i += uint64(1)) {
			rtn = rtn * x;
		}
		return rtn;
	}
}
