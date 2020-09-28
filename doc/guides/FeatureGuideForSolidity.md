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

## Comparison table with Solidity 0.7.1

### Layout
|Layout|support|
|-|-|
|pragma solidity | Yes |
|pragma experimental | No |
|interface | No |
|library | Yes |
|abstract | Yes |
|using A for B; | No |
|Inline Assembly | No |

### Global Variables
|Global Variables|support|
|-|-|
|abi.decode | No |
|abi.encode | Yes |
|abi.encodePacked | Yes |
|abi.encodeWithSelector | No |
|abi.encodeWithSignature | No |
|block.coinbase | Yes |
|block.difficulty | Yes |
|block.gaslimit | Yes |
|block.number | Yes |
|block.timestamp | Yes |
|gasleft | Yes |
|msg.data | Yes |
|msg.sender | Yes |
|msg.value | Yes |
|tx.gasprice | Yes |
|tx.origin | Yes |
|assert | Yes |
|require(bool condition) | Yes |
|require(bool condition, string memory message) | Yes |
|revert() | Yes |
|revert(string memory message) | Yes |
|blockhash | Yes |
|keccak256 | Yes |
|sha256 | Yes |
|ripemd160 | Yes |
|ecrecover | Yes |
|addmod | Yes |
|mulmod | Yes |
|this | Yes |
|super | No |
|selfdestruct | No |
|&lt;address&gt;.balance | Yes |
|&lt;address payable&gt;.send | Yes |
|&lt;address payable&gt;.transfer | Yes |
|&lt;address&gt;.call | Yes |
|&lt;address&gt;.delegatecall | Yes |
|&lt;address&gt;.staticcall | Yes |
|type(C).name | No |
|type(C).creationCode | No |
|type(C).runtimeCode | No |
|type(I).interfaceId | No |
|type(T).min | No |
|type(T).max | No |

### Opcode
|Opcode|support|
|-|-|
|stop() | Yes |
|add(x, y) | Yes |
|sub(x, y) | Yes |
|mul(x, y) | Yes |
|div(x, y) | Yes |
|sdiv(x, y) | Yes |
|mod(x, y) | Yes |
|smod(x, y) | No |
|exp(x, y) | Yes |
|not(x) | Yes |
|lt(x, y) | Yes |
|gt(x, y) | Yes |
|slt(x, y) | Yes |
|sgt(x, y) | Yes |
|eq(x, y) | Yes |
|iszero(x) | Yes |
|and(x, y) | Yes |
|or(x, y) | Yes |
|xor(x, y) | Yes |
|byte(n, x) | No |
|shl(x, y) | Yes |
|shr(x, y) | Yes |
|sar(x, y) | Yes |
|addmod(x, y, m) | Yes |
|mulmod(x, y, m) | Yes |
|signextend(i, x) | Yes |
|keccak256(p, n) | Yes |
|pc() | No |
|pop(x) | No |
|mload(p) | Yes |
|mstore(p, v) | Yes |
|mstore8(p, v) | Yes |
|sload(p) | Yes |
|sstore(p, v) | Yes |
|msize() | Yes |
|gas() | Yes |
|address() | No |
|balance(a) | No |
|selfbalance() | No |
|caller() | Yes |
|callvalue() | Yes |
|calldataload(p) | Yes |
|calldatasize() | Yes |
|calldatacopy(t, f, s) | No |
|codesize() | No |
|codecopy(t, f, s) | Yes |
|extcodesize(a) | No |
|extcodecopy(a, t, f, s) | No |
|returndatasize() | No |
|returndatacopy(t, f, s) | No |
|extcodehash(a) | No |
|create(v, p, n) | No |
|create2(v, p, n, s) | No |
|call(g, a, v, in, insize, out, outsize) | No |
|callcode(g, a, v, in, insize, out, outsize) | No |
|delegatecall(g, a, in, insize, out, outsize) | No |
|staticcall(g, a, in, insize, out, outsize) | No |
|return(p, s) | Yes |
|revert(p, s) | Yes |
|selfdestruct(a) | No |
|invalid() | No |
|log0(p, s) | Yes |
|log1(p, s, t1) | Yes |
|log2(p, s, t1, t2) | Yes |
|log3(p, s, t1, t2, t3) | Yes |
|log4(p, s, t1, t2, t3, t4) | Yes |
|chainid() | No |
|origin() | Yes |
|gasprice() | Yes |
|blockhash(b) | No |
|coinbase() | Yes |
|timestamp() | Yes |
|number() | Yes |
|difficulty() | Yes |
|gaslimit() | Yes |
|datasize(x) | Yes |
|dataoffset(x) | Yes |
|datacopy(t, f, l) | Yes |
|setimmutable | No |
|loadimmutable | No |
|linkersymbol | No |

### Types
|Value Types|support|
|-|-|
|Booleans | Yes |
|Booleans operator | Yes |
|Integers | Yes |
|Integers shift(<<, >>) | No |
|type() | No |
|Fixed Point Numbers | No |
|address | Yes |
|address payable | Yes |
|Enums | No |
|Structs | Yes (cannot assignment) |

|Literals|support|
|-|-|
|Address Literals | Yes |
|Rational Literals | No |
|Scientific notation | No |
|Underscores (123_456) | No |
|escape characters | No |
|Unicode Literals | No |
|Hexadecimal Literals | Yes (separated by space not support) |

|Reference Types|support|
|-|-|
|storage | Yes |
|memory | Yes |
|calldata | Yes |

|Mapping Types|support|
|-|-|
|mapping of mapping | Yes |
|mapping of string/bytes | Yes |
|mapping of struct | No |

|Ether Units|support|
|-|-|
|wei | Yes |
|ether | Yes |
|gwei | No |

|Time Units|support|
|-|-|
seconds | Yes |
|minutes | Yes |
|hours | Yes |
|days | Yes |
|weeks | Yes |

|Other Types|support|
|-|-|
|delete | No |
|Fixed-Size Byte Array String literals Assign | No |
|Function multi return | No |

### Structure of a Contract

|State Variables Types|support|
|-|-|
|Value Types(int, uint, bool, ...) | Yes |
|Address | Yes |
|Contract Types | No |
|Fixed-size byte arrays | Yes |
|Dynamically-sized byte array | Yes |
|Arrays | No |
|Structs | Yes |
|Mapping | Yes |
|constant | Yes |
|immutable | No |

|Functions Types|support|
|-|-|
|Functions | Yes |
|receive | No |
|constructor | Yes |
|Modifiers | No |
|fallback | Yes |
|Function Overloading | No |
|Constructor with parameter | No |
|virtual | No |
|override | No |

|Others|support|
|-|-|
|Events | Yes |
|Inheritance | No |

### Expressions and Control Structures

|Control Structures|support|
|-|-|
|if else | Yes |
|while | Yes |
|do | No |
|for | Yes |
|break | Yes |
|continue | Yes |
|return | Yes |

|Function Calls|support|
|-|-|
|Internal Function Calls | Yes |
|this.func() | Yes |
|External Function Calls | No |
|Named Calls | No |
|Omitted Function Parameter Names | Yes |

|Others|support|
|-|-|
|new | No |
|salt | No |
|try catch | No |
|tuple assign | No |
|Complications for Arrays  | Yes |
|Complications for Structs | Yes |
