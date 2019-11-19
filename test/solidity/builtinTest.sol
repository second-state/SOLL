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
  }

  function () external payable {
  }
}
