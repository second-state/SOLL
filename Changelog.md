[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
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
* Add command-line options for generating metadata of Solidity contracts.
    * `EmitABI`: Generate ABI
    * `EmitFuncSig`: Generate function signatures
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
* Add command-line option. (Emit LLVM IR, Dump AST).

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
