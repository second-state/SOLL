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

* Generate legal and can be deployed on testnet Ewasm file.  
* Add diagnostic message support. 
* Add command-line options. (Generate ABI, function signature and evm bytecode)

Bugfixes:

* Fix endian issue when call EEI function from wasm.

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

* Test suite migrate into utils as submodule.
* Add command line option. (Emit LLVM IR, Dump AST).

Bugfixes:

* Provide correct binary operator precedence. 


### 0.0.1 (2019-07-31)

Language Features:

* Type
    * 64bit integer type support only.
* Basic grammar support.
    * Binary operator, Unary Operator, If statement, Function call.
* Builtin function
    * require(), revert()


Compiler Features:

* Two phases compile flow to generate wasm file.

