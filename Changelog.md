### 0.0.3 (unreleased)

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
* Add command-line options. (Generate ABI, function signature and evm bytecode)

Bugfixes:

* Fix an endian issue that happened when someone calls the EEI function from wasm.

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

* Provide correct binary operator precedence. 


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

