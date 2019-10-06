# Introduction
**SOLL** is a new compiler for generating [ewasm](https://github.com/ewasm) (Ethereum flavored WebAssembly) files from Solidity.

To support developers as many as possible, we design projects to not only support more smart contract programming languages, such as Rust and C++, but also support various VMs, such as ewasm VM and evm. To achieve this goal, in the very first step, we develop SOLL, a compiler for Solidity-based smart contract running on ewasm VM.

In SOLL, we design two phases of code generation flow. Generate *.ll (LLVM IR file) first, then generate *.wasm (EWASM file).

In the Getting Started in following sections, we will show you several steps to use SOLL to generate ewasm file.

1. Build SOLL from source code.
2. Generate ewasm files from our demo contracts.
    - [0-0-1.sol](./test/release/0-0-1.sol) - Safemath (simplified version)
    - [0-0-2.sol](./test/release/0-0-2.sol) - [ERC20](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-20.md) (simplified version)
3. Execute those ewasm files in our test environment.

# Getting Started
To get started with our demo, you will need two components at first.

- Pre-install **Docker** and pull our [docker image](https://hub.docker.com/r/secondstate/soll) (optional)
> *We provide an image include build and execute environment (recommend).  
> If you don't want to use docker directly you will need below tools (cmake, llvm, xxd, wabt, node.js).*

- **Soll** https://github.com/second-state/soll

## Preparation
- Pull official docker image to get an already established build/execute environment.
```Shell
> docker pull secondstate/soll
```

- Get Source Code from Github
```Shell
> git clone --recursive https://github.com/second-state/soll.git
```

## Working Tree
```
soll
├── (...)
├── build                 // Build code path, manually create it
├── test
│   ├── (...)
│   └── release           // Release Test Contract
│       ├── 0-0-1.sol
│       └── 0-0-2.sol
├── (...)
└── utils
    ├── (...)
    └── ewasm-testbench
        ├── (...)
        ├── safeMath.js   // 0-0-1.sol Test Environment
        └── erc20.js      // 0-0-2.sol Test Environment
```

## Launch Environment
Attach shell to container and bind volume with repositories' path.
```Shell
> docker run -it --rm \
      -v $(pwd)/soll:/root/soll \
      secondstate/soll
```

## Build SOLL
Build SOLL first (we use cmake with llvm library)
```Shell
(docker) $ cd ~/soll && mkdir -p build && cd build
(docker) $ cmake .. && make
```

## Demo with [0-0-1.sol](./test/release/0-0-1.sol)

**Phase 1. Use SOLL generate .ll from test contract**

Execute SOLL to generate a *.ll file for the next step.
```Shell
(docker) $ ~/soll/build/tools/soll/soll ~/soll/test/release/0-0-1.sol > ~/soll/utils/ewasm-testbench/safeMath.ll
```

**Phase 2. Generate .wasm from .ll**

```Shell
(docker) $ ~/soll/utils/compile -v ~/soll/utils/ewasm-testbench/safeMath.ll
```

**Run in Test Env**

We use "16 divides 7" as our smart contract function to check whether our "SafeMath" execute correctly or not.
```Shell
(docker) $ cd ~/soll/utils/ewasm-testbench
(docker) $ ./safeMath.js safeMath.wasm div 16 7
```

The result should be the same as the following content.

```Shell
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

## Demo with [0-0-2.sol](./test/release/0-0-2.sol)

**Phase 1. Use SOLL generate .ll from test contract**

Execute SOLL to generate a *.ll file for the next step.
```Shell
(docker) $ ~/soll/build/tools/soll/soll ~/soll/test/release/0-0-2.sol > ~/soll/utils/ewasm-testbench/erc20.ll
```

**Phase 2. Generate .wasm from .ll**

```Shell
(docker) $ ~/soll/utils/compile -v ~/soll/utils/ewasm-testbench/erc20.ll
```

**Run in Test Env**

We use `{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}` as our storage current state and use smart contract function **balanceOf** to check the balance of default **msg.sender**.
> Here default msg.sender is address 0x1234567890123456789012345678901234567890  
> [More](https://solidity.readthedocs.io/en/v0.5.3/miscellaneous.html#mappings-and-dynamic-arrays) about how storage layout

```Shell
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

**Run in Test Env**

We still use `{"2":"f","13425c139e83d895e2b184742e4c3c48f19def0307be60e6900f6563e300a60f":"f"}` as our storage current state and use smart contract function **transfer** to transfer amount(1) from default **msg.sender** to other address as 0x1234567890123456789012345678901234567891.

```Shell
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