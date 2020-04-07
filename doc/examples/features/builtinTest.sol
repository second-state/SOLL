// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract EEI_Test {
  address msgSender;
  uint msgValue;
  bytes msgData;
  uint txGasPrice;
  address txOrigin;
  address bkCoinbase;
  uint256 bkDifficulty;
  uint bkGasLimit;
  uint bkBlockNumber;
  uint bkBlockTimestamp;
  mapping (address => uint256) private hash;
  event Test(address indexed addr, uint256 indexed value, uint256 value2);

  constructor() public payable {
    msgSender = msg.sender;
    msgValue = msg.value;
    msgData = msg.data;
    txGasPrice = tx.gasprice;
    txOrigin = tx.origin;
    bkCoinbase = block.coinbase;
    bkDifficulty = block.difficulty;
    bkGasLimit = block.gaslimit;
    bkBlockNumber = block.number;
    bkBlockTimestamp = block.timestamp;
    hash[msgSender] = 54309531;
    emit Test(msgSender, uint(123), uint(456));

    uint256 bal = address(0xbf4ed7b27f1d666546e30d74d50d173d20bca754).balance;
    address(0xbf4ed7b27f1d666546e30d74d50d173d20bca754).balance < 1;
  }

  // for abi.encode, abi.encodePacked
  uint a;
  int16 b;
  address c;
  bool d;
  string s1;
  string s2;
  bytes b1;
  bytes b2;
  bytes8 e;

  function Jinkela() public payable returns(uint){
      return 12341234;
  }

  function encode() public payable returns(bytes memory){
      return abi.encode(a,s1,b,abi.encode(a,b,c,d,e),Jinkela(),b1,d,s2,e);
  }

  function encodePacked() public payable returns(bytes memory){
      return abi.encodePacked(a,s1,b,abi.encodePacked(a,b,c,d,e),Jinkela(),b1,d,s2,e);
  }

  function require_sample(uint64 a, uint64 b) public pure returns (uint64) {
        uint64 c = a + b;
        require(c >= a, "SafeMath: addition overflow");
        assert(c >= a);
        revert("Revert Msg");
    }
}

contract CastExpr {
    address a;
    address b;
    uint e;
    uint f;
    uint256 g;
    uint256 h;
    uint32 i;
    uint32 j;
    function castExpr() public {
        b = address(this);
        a = address(b);
        e = 7189324;
        //i = 209528;
        i = uint32(209528);
        j = uint32(i);
        g = uint256(i);
        //h = 115792089237316195423570985008687907853269984665640564039457584007913129639935;
        g = uint256(h);
        f = uint(h);
    }
}