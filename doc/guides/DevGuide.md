[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)
# Developer Guide

## Introduction

In this guide, we will introduce how to use SOLL to compile smart contracts and execute these contracts by our benchmark.

In following sections, we provide four parts to show how to compile and execute smart contracts by yourself.

1. Prepare required environment.
2. Build SOLL compiler from source code.
3. Generate Ewasm bytecodes from our demo contracts and execute them.
    - [0-0-1.sol](../../doc/examples/0-0-1.sol) - A basic smart contract example (SaftMath)
    - [0-0-2.sol](../../doc/examples/0-0-2.sol) - A partial function example of [ERC20](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-20.md) contract.
    - [0-0-3.sol](../../doc/examples/0-0-3.sol) - A full function example of [ERC20](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-20.md) contract.
4. Generate Ewasm bytecodes from our Yul demo contract and execute them. **[(0.0.4 limited version)](FeatureGuideForYul.md)**
    - [0-0-4.yul](../../doc/examples/0-0-4.yul) - A Fibonacci sequence example written in Yul.

## Working Tree
```
soll
├── (...)
├── build                 // Build code path, manually create it
├── doc
│   ├── (...)
│   └── example           // Examples of every release
│       ├── 0-0-1.sol     // A basic smart contract example (SafeMath)
|       ├── 0-0-2.sol     // A partial version of ERC20 contract
|       ├── 0-0-3.sol     // A full function example of ERC20 contract
│       └── 0-0-4.yul     // A fibonacci sequence example written in Yul 
├── (...)
└── utils
    ├── (...)
    └── ewasm-testbench
        ├── (...)
        ├── safeMath.js   // 0-0-1.sol Test Environment
        └── erc20.js      // 0-0-2.sol Test Environment
```

## Build SOLL

### Preparation

- Pull official docker image to get an already established build/execute environment.
```bash
> docker pull secondstate/soll
```

- Get Source Code from Github and checkout to the latest version, 0.0.4.
```bash
> git clone --recursive https://github.com/second-state/soll.git
> cd soll
> git checkout 0.0.4
```

### Launch Docker Environment
Attach shell to container and bind volume with repositories' path.
```bash
> docker run -it --rm \
      -v $(pwd)/soll:/root/soll \
      secondstate/soll
```

### Cmake and Build
Build SOLL without enabling our built-in test suite.
```bash
(docker) $ cd ~/soll && mkdir -p build && cd build
(docker) $ cmake .. && make
```

### Compile and execute SafeMath smart contract

To show how to use SOLL to compile and execute smart contracts, we provide a very basic example to demonstrate it. In this example, the smart contract will provide Safe Math machnism that will make sure that calculation results will not be overflow or divided by zero. In following steps, we will show how to compile this example and use our benchmark to run this smart contract.

#### **Step 1. Use SOLL generate LLVM IR files from given contract**

Execute SOLL to generate a LLVM IR file for the next step.

```bash
(docker) $ ~/soll/build/tools/soll/soll ~/soll/doc/examples/0-0-1.sol > ~/soll/utils/ewasm-testbench/safeMath.ll
```

#### **Step 2. Generate Ewasm bytecodes from LLVM IR**

```bash
(docker) $ ~/soll/utils/compile -v ~/soll/utils/ewasm-testbench/safeMath.ll
```

#### **Step 3. Run in Test Env**

We use "16 divides 7" as our smart contract function to check whether our "SafeMath" execute correctly or not.

```bash
(docker) $ cd ~/soll/utils/ewasm-testbench
(docker) $ ./safeMath.js safeMath.wasm div 16 7
```

The result should be the same as the following content.

```bash
getCallDataSize()
{ size: 68 }
callDataCopy(66128, 0, 4)
{ data: a391c15b }
callDataCopy(66064, 4, 64)
{ data: 00000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000007 }
getCallValue(66048)
{ value: 00000000000000000000000000000000 }
finish(66032, 32)
{ returnData:
   '0000000000000000000000000000000000000000000000000000000000000002',
  storage: '{}' }
```

### Execute an ERC20 smart contract compiled by SOLL

We provide two examples, 0-0-2.sol and 0-0-3.sol, to demonstrate how to use SOLL to compile and execute ERC20 contracts. In this example, we use 0-0-2.sol that contains very core functions of ERC20 contract to demonstrate how to compile and execute ERC20 contracts.

#### **Step 1. Use SOLL generate LLVM IR files from given contract**

Execute SOLL to generate a LLVM IR file for the next step.

```bash
(docker) $ ~/soll/build/tools/soll/soll ~/soll/doc/examples/0-0-2.sol > ~/soll/utils/ewasm-testbench/erc20.ll
```

#### **Step 2. Generate Ewasm bytecodes from LLVM IR**

```bash
(docker) $ ~/soll/utils/compile -v ~/soll/utils/ewasm-testbench/erc20.ll
```

#### **Step 3-1. Run in Test Env (emit balanceOf)**

We use `{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}` as our storage current state and use smart contract function **balanceOf** to check the balance of default **msg.sender**.

> Here default msg.sender is address 0x1234567890123456789012345678901234567890
> [More](https://solidity.readthedocs.io/en/v0.5.3/miscellaneous.html#mappings-and-dynamic-arrays) about how storage layout

```bash
(docker) $ cd ~/soll/utils/ewasm-testbench/
(docker) $ ./erc20.js erc20.wasm '{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}' balanceOf 0x1234567890123456789012345678901234567890
```

The result should be the same as the following content.

```
getCallDataSize()
{ size: 36 }
callDataCopy(66096, 0, 4)
{ data: 70a08231 }
callDataCopy(66064, 4, 32)
{ data: 0000000000000000000000001234567890123456789012345678901234567890 }
getCallValue(65992)
{ value: 00000000000000000000000000000000 }
getGasLeft()
{ gas: 65522 }
callStatic(65522, 66040, 65792, 64)
{ address: 2, data: 00000000000000000000000012345678901234567890123456789012345678900000000000000000000000000000000000000000000000000000000000000000 }
getCallDataSize()
{ size: 64 }
useGas(84)
getCallDataSize()
{ size: 64 }
callDataCopy(1179584, 0, 64)
{ data: 00000000000000000000000012345678901234567890123456789012345678900000000000000000000000000000000000000000000000000000000000000000 }
finish(1048544, 32)
returnDataCopy(66008, 0, 32)
{ data: 13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f }
storageLoad(65728, 65760)
{ key: 13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f, value: f }
finish(66032, 32)
{ returnData:
   '000000000000000000000000000000000000000000000000000000000000000f',
  storage:
   '{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}' }
```

#### **Step 3-2. Run in Test Env (emit transfer)**

We still use `{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}` as our storage current state and use smart contract function **transfer** to transfer amount(1) from default **msg.sender** to other address as 0x1234567890123456789012345678901234567891.

```bash
(docker) $ cd ~/soll/utils/ewasm-testbench/
(docker) $ ./erc20.js erc20.wasm '{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}' transfer 0x1234567890123456789012345678901234567891 1
```

The result should be the same as the following content.

```
(... omitted)

{ returnData:
   '0000000000000000000000000000000000000000000000000000000000000001',
  storage:
   '{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"e","d3a40b027a96d16f0c9c02fdbf30dd031cb372ed53432958315b5da0226952e":"1"}' }
```

### Compile and execute Yul code

To show how to use SOLL to compile and execute Yul code, we also provide a very basic example to demonstrate it. In this example, the code will calculate first ten numbers of the Fibonacci sequence and store them into storage with address 0 to 9 . In following steps, we will show how to compile this example and use our benchmark to run this code.

#### **Step 1. Use SOLL generate LLVM IR files from Yul code**

Execute SOLL to generate a LLVM IR file for the next step.

```bash
(docker) $ ~/soll/build/tools/soll/soll -lang=Yul ~/soll/doc/examples/0-0-4.yul > ~/soll/utils/ewasm-testbench/fib.ll
```

#### **Step 2. Generate Ewasm bytecodes from LLVM IR**

```bash
(docker) $ ~/soll/utils/compile_yul -v ~/soll/utils/ewasm-testbench/fib.ll
```

#### **Step 3. Run in Test Env**

The Yul code section will run immediately after wasm load by index.js.

```bash
(docker) $ cd ~/soll/utils/ewasm-testbench
(docker) $ ./index.js fib.wasm '' '{}'
```

The result should be the same as the following content.

```bash
{ returnData: '',
  storage:
   '{"0":"1","1":"1","2":"2","3":"3","4":"5","5":"8","6":"d","7":"15","8":"22","9":"37"}' }
```

## Run SOLL regression tests and figure out our Solidity and Yul tests coverage

### **Step 1. Build SOLL with enabling built-in test suite**
```bash
(docker) $ cd ~/soll # Go to the SOLL source code folder
(docker) $ mkdir -p build && cd build # Create a build folder for cmake
(docker) $ cmake -DCMAKE_BUILD_TYPE=Debug -DSOLL_INCLUDE_TESTS=true ..
(docker) $ make
```

### **Step 2. Go to build folder and run tests**
```bash
(docker) $ ctest -V # In the build folder

# Output
UpdateCTestConfiguration  from :/home/ubuntu/workspace/soll/build/DartConfiguration.tcl
Parse Config file:/home/ubuntu/workspace/soll/build/DartConfiguration.tcl

(..omitted)

test 1
      Start  1: Expr

1: Test command: /home/ubuntu/workspace/soll/build/unittests/unittests "Expr"
1: Test timeout computed to be: 1500
1: Filters: Expr
1: ===============================================================================
1: All tests passed (4 assertions in 1 test case)
1: 
 1/23 Test  #1: Expr .............................   Passed    0.02 sec

(..omitted)

test 20
      Start 20: check-soll-benchmark

20: Test command: /usr/lib/llvm-8/build/utils/lit/lit.py "--timeout" "10" "--param" "soll_site_config=/home/ubuntu/workspace/soll/build/test/lit.site.cfg" "/home/ubuntu/workspace/soll/build/test/benchmark"
20: Test timeout computed to be: 1500
20: -- Testing: 4 tests, 4 threads --
20: PASS: SOLL :: benchmark/fib.sol (1 of 4)
20: PASS: SOLL :: benchmark/power.sol (2 of 4)
20: PASS: SOLL :: benchmark/warshall.sol (3 of 4)
20: PASS: SOLL :: benchmark/matrix.sol (4 of 4)
20: Testing Time: 0.11s
20:   Expected Passes    : 4
20/23 Test #20: check-soll-benchmark .............   Passed    0.22 sec
test 21
      Start 21: check-soll-solidity

21: Test command: /usr/lib/llvm-8/build/utils/lit/lit.py "--timeout" "10" "--param" "soll_site_config=/home/ubuntu/workspace/soll/build/test/lit.site.cfg" "/home/ubuntu/workspace/soll/build/test/solidity"
21: Test timeout computed to be: 1500
21: -- Testing: 66 tests, 8 threads --
21: UNSUPPORTED: SOLL :: solidity/codegenTests/operator.sol (1 of 66)
21: UNSUPPORTED: SOLL :: solidity/codegenTests/namedCall.sol (2 of 66)
21: UNSUPPORTED: SOLL :: solidity/compilationTests/MultiSigWallet/Factory.sol (3 of 66)
21: UNSUPPORTED: SOLL :: solidity/codegenTests/statements/BreakStmt.sol (4 of 66)

(..omitted)

21: PASS: SOLL :: solidity/constructor.sol (62 of 66)
21: PASS: SOLL :: solidity/helloWorld.sol (63 of 66)
21: PASS: SOLL :: solidity/math.sol (64 of 66)
21: PASS: SOLL :: solidity/event.sol (65 of 66)
21: PASS: SOLL :: solidity/string.sol (66 of 66)
21: Testing Time: 0.11s
21:   Expected Passes    : 10
21:   Unsupported Tests  : 56
21/23 Test #21: check-soll-solidity ..............   Passed    0.27 sec
test 22
      Start 22: check-soll-yul

22: Test command: /usr/lib/llvm-8/build/utils/lit/lit.py "--timeout" "10" "--param" "soll_site_config=/home/ubuntu/workspace/soll/build/test/lit.site.cfg" "/home/ubuntu/workspace/soll/build/test/yul"
22: Test timeout computed to be: 1500
22: -- Testing: 3 tests, 3 threads --
22: PASS: SOLL :: yul/factorial.yul (1 of 3)
22: PASS: SOLL :: yul/fib.yul (2 of 3)
22: PASS: SOLL :: yul/feature.yul (3 of 3)
22: Testing Time: 0.11s
22:   Expected Passes    : 3
22/23 Test #22: check-soll-yul ...................   Passed    0.23 sec
test 23
      Start 23: check-soll-libyul

23: Test command: /usr/lib/llvm-8/build/utils/lit/lit.py "--timeout" "10" "--param" "soll_site_config=/home/ubuntu/workspace/soll/build/test/lit.site.cfg" "/home/ubuntu/workspace/soll/build/test/libyul"
23: Test timeout computed to be: 1500
23: -- Testing: 499 tests, 8 threads --
23: UNSUPPORTED: SOLL :: libyul/ewasmTranslationTests/address.yul (1 of 499)
23: UNSUPPORTED: SOLL :: libyul/ewasmTranslationTests/balance.yul (2 of 499)
23: UNSUPPORTED: SOLL :: libyul/ewasmTranslationTests/bitwise_and.yul (3 of 499)

(..omitted)

23: PASS: SOLL :: libyul/yulOptimizerTests/varNameCleaner/function_parameters.yul (494 of 499)
23: PASS: SOLL :: libyul/yulOptimizerTests/varDeclInitializer/simple.yul (495 of 499)
23: PASS: SOLL :: libyul/yulOptimizerTests/varNameCleaner/instructions.yul (496 of 499)
23: PASS: SOLL :: libyul/yulOptimizerTests/varNameCleaner/reshuffling-inverse.yul (497 of 499)
23: PASS: SOLL :: libyul/yulOptimizerTests/varNameCleaner/reshuffling.yul (498 of 499)
23: PASS: SOLL :: libyul/yulOptimizerTests/wordSizeTransform/or_bool_renamed.yul (499 of 499)
23: Testing Time: 0.41s
23:   Expected Passes    : 63
23:   Unsupported Tests  : 436
23/23 Test #23: check-soll-libyul ................   Passed    0.55 sec

100% tests passed, 0 tests failed out of 23

Total Test time (real) =   1.58 sec
```

### **Step 3. Check the test coverage**

You will find the test coverage like this:

Solidity test suite:
```bash
21:   Expected Passes    : 10 # Solidity has 66 testing contract, and SOLL can pass 10.
21:   Unsupported Tests  : 56 # Unimplemented by SOLL
21/23 Test #21: check-soll-solidity ..............   Passed    0.27 sec
```

Yul test suite:
```bash
23:   Expected Passes    : 63 # libyul has 499 testing contracts, and SOLL can pass 63.
23:   Unsupported Tests  : 436 # Unimplemented by SOLL
23/23 Test #23: check-soll-libyul ................   Passed    0.55 sec
```
