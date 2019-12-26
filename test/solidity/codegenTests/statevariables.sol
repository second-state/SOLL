// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// RUN: %soll %s
pragma solidity ^0.5.0;

contract StateVariables {
    address msgSender;
    uint msgValue;
    bytes msgData;
    address bkCoinbase;
    uint bkDifficulty;
    uint bkGasLimit;
    uint bkBlockNumber;
    uint bkBlockTimestamp;
    uint bkHash;
    uint gasLeft;
    uint txGasPrice;
    address txOrigin;
    uint txOriginBalance;
    address _this;
    uint otherBalance;

    function Jinkela() public payable returns(uint){
        return 12341234;
    }

    constructor() public payable {
        msgSender = msg.sender;
        msgValue = msg.value;
        msgData = msg.data;
        bkCoinbase = block.coinbase;
        bkDifficulty = block.difficulty;
        bkGasLimit = block.gaslimit;
        bkBlockNumber = block.number;
        bkBlockTimestamp = block.timestamp;
        bkHash = blockhash(1234);
        gasLeft = gasleft();
        txGasPrice = tx.gasprice;
        txOrigin = tx.origin;
        txOrigin.staticcall(msgData);
        txOrigin.delegatecall(msgData);
        txOrigin.call(msgData);
        txOrigin.send(2217);
        txOriginBalance = txOrigin.balance;
        bkDifficulty = this.Jinkela();
        _this = address(this);
        address(this).send(2345);
        otherBalance = address(111).balance;
        address(address(this)).send(123123);
    }
}