[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
# Feature Guide for Solidity

## Introduction

In this guide, we will list all Solidity language features and briefly summarize the current status and limitations about SOLL implementation.

### Supported Language Features

- Contract declaration
  - Constructor without parameters
- Type support
  - Value types: `bool`, `int`, `uint`, `address`
  - Literal types: `Address Literals`, `Rational and Integer Literals`, `String Literals`, `Hexadecimal Literals`
  - Mapping types: `mapping`
  - String types: `string`
  - Array types:
    - Support compile-time fixed size array
    - Fixed-Size *byte arrays*
    - Dynamically-Size *byte arrays*
  - Struct types
  - Support both implicit/explicit type casting
- Storage declaration
- Event declaration and emit event statements
  - Support keywords `event`, `emit`
- Function declaration
  - Visibility: `public`, `private`, `internal`, `external`
  - State Mutability `view`, `pure`
  - Support `payable`
  - Support fallback function declaration and keyword `fallback`
- Units and Globally Available Variables
  - Ether Units: `ether`, `finney`, `szabo`, `wei`
  - Time Units: `weeks`, `days`, `hours`, `minutes`, `seconds`
  - Members of Address Types:
    - `<address>.balance (uint256)`
    - `<address payable>.transfer(uint256 amount)`
    - `<address payable>.send(uint256 amount) returns (bool)`
  - Block and Transaction Properties:
    - `blockhash(uint blockNumber) returns (bytes32)`
    - `block.coinbase (address payable)`
    - `block.difficulty (uint)`
    - `block.gaslimit (uint)`
    - `block.number (uint)`
    - `block.timestamp (uint)`
    - `gasleft() returns (uint256)`
    - `msg.data (bytes calldata)`
    - `msg.sender (address payable)`
    - `msg.sig (bytes4)`
    - `msg.value (uint)`
    - `now (uint)`
    - `tx.gasprice (uint)`
    - `tx.origin (address payable)`
  - ABI Encoding and Decoding Functions(*Experimental, Unstable*):
    - `abi.encode(...) returns (bytes memory)`
    - `abi.encodePacked(...) returns (bytes memory)`
  - Contract Related: `this`
- Error handling
  - Support `require()`, `revert()`
- Grammar and statements
  - Binary operators
  - Unary operators
  - Exponentiation operators
  - Condition statements: `if`, `else`
  - Function call statements
  - Loop statements: `while`, `do..while`, `for` and nested loop statements are also supported.

### Unimplemented Language Features / Known Issues

- Library declaration
  - Keyword `library` is not yet supported.
- Types
  - Contract types
  - Function types
  - Enums
  - Fixed Point Numbers
  - Dynamic size arrays
- Units and Globally Available Variables
  - ABI Encoding and Decoding Functions:
    - `abi.decode(bytes memory encodedData, (...)) returns (...)`
    - `abi.encodeWithSelector(bytes4 selector, ...) returns (bytes memory)`
    - `abi.encodeWithSignature(string memory signature, ...) returns (bytes memory)`
  - Mathematical and Cryptographic Functions:
    - `addmod(uint x, uint y, uint k) returns (uint)`
    - `mulmod(uint x, uint y, uint k) returns (uint)`
    - `keccak256(bytes memory) returns (bytes32)`
    - `sha256(bytes memory) returns (bytes32)`
    - `ripemd160(bytes memory) returns (bytes20)`
    - `ecrecover(bytes32 hash, uint8 v, bytes32 r, bytes32 s) returns (address)`
  - Members of Address Types:
    - `<address>.call(bytes memory) returns (bool, bytes memory)`
    - `<address>.delegatecall(bytes memory) returns (bool, bytes memory)`
    - `<address>.staticcall(bytes memory) returns (bool, bytes memory)`
  - Contract related: `selfdestruct(address payable recipient)`
  - Type Information: `type(C).creationCode`, `type(C).runtimeCode`
- Modifier declaration and usage
  - Keyword `modifier` is not yet supported.
  - Developers should extend their modifier logic into function body directly
- Multiple contracts declaration.
  - Developers can declare only one contract in a single file.
- Contract inheritance
  - Keyword `is` is not yet supported.
- Constructor with parameters
  - Because Ewasm deployer does not support deployment with constructor’s parameters.
- Tuple declaration
  - Grammar `()` is not yet supported.
- Multiple function return values
  - Due to the lack support of tuple, functions cannot return multiple values.
- Inline assembly
