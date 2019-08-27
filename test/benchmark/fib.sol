pragma solidity >0.4.0 <=0.7.0;

contract FIB {
	// evm takes seconds to run fib(10000)
	function fib(uint x) public pure returns(uint) {
		uint MOD = 1000000007;
		uint f = uint(1);
		uint s = uint(1);
		uint tmp;
	    for(uint i = uint(3); i<=x; i += 1) {
	        f = f + s;
			f %= MOD;
	        tmp = f;
	        f = s;
	        s = tmp;
	    }
	    return s;
	}
	function fib64(uint64 x) public pure returns(uint64) {
		uint64 MOD = 1000000007;
		uint64 f = uint64(1);
		uint64 s = uint64(1);
		uint64 tmp;
	    for(uint64 i = uint64(3); i<=x; i += 1) {
	        f = f + s;
			f %= MOD;
	        tmp = f;
	        f = s;
	        s = tmp;
	    }
	    return s;
	}

	function fib_recursive(uint x) public pure returns(uint) {
		if (x == 1 || x == 2) return 1;
		else return fib_recursive(x-1) + fib_recursive(x-2);
	}
	function fib64_recursive(uint64 x) public pure returns(uint64) {
		if (x == 1 || x == 2) return 1;
		else return fib64_recursive(x-1) + fib64_recursive(x-2);
	}
}
