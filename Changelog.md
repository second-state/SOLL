[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)

### 0.0.6 (2020-03-xx)

Language Features:

* Solidity:
    * Support abi.encode and abi.encodePacked. (excluding array type)
    * Support fallback keyword. (solidity spec >= 0.6.0)
* Yul:
    * Support tuple usage.
        * return, declaration, assignment
    * Support built-in functions.
        * `return`, `revert`
        * `log0`, `log1`, `log2`, `log3`, `log4`
        * `blockcoinbase`, `blockdifficulty`, `blockgaslimit`
        * `blocknumber`, `blocktimestamp`, `txorigin`, `txgasprice`, `gasleft`
        * `caller`, `callvalue`, `calldatasize`
        * `codecopy`, `datacopy`
        * `datasize`, `dataoffset` (limitation: couldn't be used apply on object itself)
    * Support leave statement. (Yul spec >= 0.6.0)
    * Support generate and deploy ewasm from Yul file. (Yul file was compiled from solc)

Compiler Features:

* Diagnostic Tool:
    * Throw error messages for unimplemented features:
        * Redefine constructor or fallback function.
    * Add color on/off option.
* Add command-line action options for distinguish generate ewasm could be deployed on chain or normal case.
    * `-deploy=Chain`: Ewasm vm (Hera) not support keccak256 yet, so for workaround use sha256 instead. (Default)
    * `-deploy=Normal`: Normal behavior.
* Add command-line action options for generating object files.
    * `-action=EmitAssembly`: Emit a .s file.
    * `-action=EmitBC`: Emit a .bc file.
    * `-action=EmitLLVM`: Emit a .bc file.
    * `-action=EmitLLVMOnly`: Generate LLVM IR, but do not emit anything.
    * `-action=EmitCodeGenOnly`: Generate machine code, but don't emit anything.
    * `-action=EmitObj`: Emit a .o file.
    * `-action=EmitWasm`: Emit a .wasm file.
* Add command-line action options for misc.
    * `-action=ParseSyntaxOnly`: Parse and perform semantic analysis.

Bugfixes:

* Add ImplicitCastExpr on return statement.
* Fix mismatch EEI functions `blockhash`.
* Keep full event signature as topic 0 field when emit event.
* Fix built-in function minor issues:
    * `iszero`, `shl`, `shr`, `sar`
* Remove unexpected message when `soll -action=EmitFuncSig`.
* Remove redundant slot access when load/store storage with bytes and string.

### 0.0.5 (2020-01-18)

Language Features:

* Common:
    * Support big number literal (`> uint64`)
* Solidity:
    * Support built-in units:
        * `ether`, `finney`, `szabo`, `wei`
        * `weeks`, `days`, `hours`, `minutes`, `seconds`
    * Replace `keccak256` and `sha256` with `sha3` for EVM backend.
    * Implement exponentiation operator.
    * Support `this` to access member functions in the same contract.
* Yul:
    * Support `switch` statement.
    * Support function definition.
    * Implement scoping logic and delay identifier lookup
    * Support alias operators:
        * `slt` -> `lts256`
        * `sgt` -> `gts256`
        * `sdiv` -> `divs256`
    * Handle memory relative mechanism:
        * Use global variable to record memory size.
        * Use `__heap_size` as memory offset.
        * Add helper functions to increase memory usage.
    * Implement built-in functions:
        * `calldataload`
        * `mload`, `mstore`, `mstore8`, `msize`
        * `exp`
* Diagnostic Tool:
    * Throw error messages for unimplemented features:
        * Constructor doesn’t support parameters. SOLL only accept constructor without any parameters.
        * SOLL doesn’t support `array.push, array.pop`
    * Terminate codegen when error occurs

Bugfixes:

* Hex number literal:
    * Support hex number literal (`0x...`)
* `<address>.balance` should return uint256 instead of uint128.
* Reduce some useless endian transformation.
* Fix mismatch EEI functions:
    * `<address>.balance`, `<address>.send`, `blockhash`


### 0.0.4 (2019-12-25)

Language Features:

* Support limited Yul language. [See details here.](doc/guides/FeatureGuideForYul.md)
    - Basic arithmetical, compare, sstore and sload built-in functions.
    - For loop, If, Var declare and Assignment statements.

Compiler Features:

* Add command-line version option for checking current SOLL version.
    * `-version`
* Add command-line language options for supporting Yul input file.
    * `-lang=Sol`: Input file is written in Solidity. (Default)
    * `-lang=Yul`: Input file is written in Yul.
* Add testing framework for unittest and regrassion test.
* Integrate Solidity compilation test and libyul test from ethereum/solidity.


### 0.0.3 (2019-11-19)

Language Features:

* Support partial builtin global variable
    * msg.sender, msg.value, msg.data.
    * tx.gasprice, tx.origin.
    * block.coinbase, block.difficulty, block.gaslimit, block.number, block.timestamp.
* Support Constructor, Fallback function.
* Support Event mechanism.
* Support Payable mechanism.
* Support dynamic type. (Bytes/String)

Compiler Features:

* Be able to generate deployable Ewasm files.
* Support diagnostic message.
* Add command-line action options for generating metadata of Solidity contracts.
    * `-action=EmitABI`: Generate ABI
    * `-action=EmitFuncSig`: Generate function signatures
* Support EVM1 bytecode target. (Depends on [EVM_LLVM](https://github.com/etclabscore/evm_llvm))

Bugfixes:

* Fix an endian issue that happened when someone calls the EEI function from wasm.
* Fix a parsing issue on array type.


### 0.0.2 (2019-09-03)

Language Features:

* Type
    * bool, integers, address.
    * fixed-size array, mapping.
* Basic grammar support
    * Support parentheses.
    * Loop (while, do...while, for)
    * Implicit/Explicit typecasting.
* Support state variable.

Compiler Features:

* Migrate Test suite to utils as a submodule.
* Add command-line action option.
    * `-action=EmitLLVM`: Emit LLVM IR. (Default)
    * `-action=ASTDump`: Dump AST.

Bugfixes:

* Fix a precedence issue related to binary operator.


### 0.0.1 (2019-07-31)

Language Features:

* Type
    * Support 64bit integer type only.
* Basic grammar support.
    * Binary operator, Unary Operator, If statement, Function call.
* Builtin function
    * require(), revert()


Compiler Features:

* Provide two compiling phases for generating wasm files.
