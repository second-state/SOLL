pragma solidity >0.4.0 <=0.7.0;

contract WARSHALL {
	function warshall() public pure returns(uint) {
		uint a = uint(3);
		uint b = uint(6);
		uint[10][10] memory G;
		for(uint i = uint(0); i < 10; i += 1) {
			for(uint j = uint(0); j < 10; j += 1) {
				G[i][j] = 999999999;
			}
		}
		for(uint i = uint(0); i < 10; i += 1) {
			G[i][i] = 0;
			if (i != 9) {
				G[i][i+1] = i * 2;
				G[i+1][i] = i * 2;
			}
		}

		for(uint k = uint(0); k < 10; k += 1) {
			for(uint j = uint(0); j < 10; j += 1) {
				for(uint i = uint(0); i < 10; i += 1) {
					if ( G[i][j] > G[i][k] + G[k][j]) {
						G[i][j] = G[i][k] + G[k][j];
					}
				}
			}
		}
		return G[a][b];
	}
	function warshall64() public pure returns(uint64) {
		uint a = uint(3);
		uint b = uint(6);
		uint64[10][10] memory G;
		for(uint i = uint(0); i < 10; i += 1) {
			for(uint j = uint(0); j < 10; j += 1) {
				G[i][j] = uint64(999999999);
			}
		}
		for(uint i = uint(0); i < 10; i += 1) {
			G[i][i] = uint64(0);
			if (i != 9) {
				G[i][i+1] = uint64(i * 2);
				G[i+1][i] = uint64(i * 2);
			}
		}

		for(uint k = uint(0); k < 10; k += 1) {
			for(uint j = uint(0); j < 10; j += 1) {
				for(uint i = uint(0); i < 10; i += 1) {
					if ( G[i][j] > G[i][k] + G[k][j]) {
						G[i][j] = G[i][k] + G[k][j];
					}
				}
			}
		}
		return G[a][b];
	}
}
