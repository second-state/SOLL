pragma solidity >0.4.0 <=0.7.0;

contract MAT {
	// fibonacci matrix multiply
	function mat_mul(uint t) public pure returns(uint) {
		uint a = uint(0);
		uint b = uint(1);
		uint[2][2] memory base;
		base[a][a] = 1;
		base[a][b] = 1;
		base[b][a] = 1;
		base[b][b] = 0;
		uint[2][2] memory rtn;
		rtn[a][a] = 1;
		rtn[b][b] = 1;
		rtn[b][a] = 0;
		rtn[a][b] = 0;
		uint[2][2] memory tmp;
		for(uint cnt = uint(0); cnt < t; cnt += 1) {
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					tmp[i][j] = 0;
				}
			}
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					for(uint k = uint(0); k < 2; k += 1) {
						tmp[i][j] += rtn[i][k] * base[k][j];
					}
				}
			}
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					rtn[i][j] = tmp[i][j];
				}
			}
		}
		return rtn[a][a];
	}
	function mat64_mul(uint64 t) public pure returns(uint64) {
		uint a = uint(0);
		uint b = uint(1);
		uint64[2][2] memory base;
		base[a][a] = 1;
		base[a][b] = 1;
		base[b][a] = 1;
		base[b][b] = 0;
		uint64[2][2] memory rtn;
		rtn[a][a] = 1;
		rtn[b][b] = 1;
		rtn[b][a] = 0;
		rtn[a][b] = 0;
		uint64[2][2] memory tmp;
		for(uint cnt = uint(0); cnt < t; cnt += 1) {
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					tmp[i][j] = 0;
				}
			}
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					for(uint k = uint(0); k < 2; k += 1) {
						tmp[i][j] += rtn[i][k] * base[k][j];
					}
				}
			}
			for(uint i = uint(0); i < 2; i += 1) {
				for(uint j = uint(0); j < 2; j += 1) {
					rtn[i][j] = tmp[i][j];
				}
			}
		}
		return rtn[a][a];
	}
}
