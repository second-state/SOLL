# Introduction
**SOLL** is a new compiler for generating [ewasm](https://github.com/ewasm) (Ethereum flavored WebAssembly) files from Solidity.

To support developers as many as possible, we design projects to not only support more smart contract programming languages, such as Rust and C++, but also support various VMs, such as ewasm VM and evm. To achieve this goal, in the very first step, we develop SOLL, a compiler for Solidity-based smart contract running on ewasm VM.

In SOLL, we design two phases of code generation flow. Generate *.ll (LLVM IR file) first, then generate *.wasm (EWASM file).

In the Getting Started in following sections, we will show you several steps to use SOLL to generate ewasm file.

1. Build SOLL from source code.
2. Generate an ewasm file from our demo contract.
    - [0-0-1.sol](./test/release/0-0-1.sol) - Safemath (simplified version)
3. Execute the ewasm file.

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
│       └── 0-0-1.sol
├── (...)
└── utils
    ├── (...)
    └── ewasm-testbench
        ├── (...)
        └── safeMath.js   // 0-0-1.sol Test Environment
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

**Run**

We use "16 divides 7" as our smart contract function to check whether our "SafeMath" execute correctly or not.
```Shell
(docker) $ cd ~/soll/utils/ewasm-testbench
(docker) $ ./safeMath.js safeMath.wasm div 16 7
```

The result should be the same as the following content.

```Shell
getCallDataSize() = 68
callDataCopy(66128, 0, 4)
callDataCopy(66064, 4, 64)
finish(66032, 32)
[ Uint8Array [
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2 ],
  '{}' ]
```
