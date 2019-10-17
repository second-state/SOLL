pragma solidity ^0.5.0;

contract EEI_Test {
  address sender;
  uint128 value;
  uint128 txGasPrice;
  address txOrigin;
  address bkCoinbase;
  uint256 bkDifficulty;
  uint64 bkGasLimit;
  uint64 bkBlockNumber;
  uint64 bkBlockTimestamp;
  mapping (address => uint256) private hash;
  event Test(address indexed addr, uint256 indexed value, uint256 value2);

  constructor() public {
    sender = msg.sender;
    // integer assignment with ImplicitCast has issue
    msg.value;
    tx.gasprice;
    txOrigin = tx.origin;
    bkCoinbase = block.coinbase;
    bkDifficulty = block.difficulty;
    block.gaslimit;
    block.number;
    block.timestamp;
    hash[sender] = 54309531;
    emit Test(sender, int(123), int(456));
  }
}
