# Introduction
**SOLL** is a new compiler for generating [ewasm](https://github.com/ewasm)(Ethereum flavored WebAssembly) files from Solidity.

To support developers as many as possible, we design projects to not only support more smart contract programming languages, such as Rust and C++, but also support various VMs, such as ewasm VM and evm. To achieve this goal, in the very first step, we develop SOLL, a compiler for Solidity-based smart contract running on ewasm VM.

In SOLL, we design two phases of code generation flow. Generate *.ll (LLVM IR file) first, then generate *.wasm (EWASM file).

In the Getting Started in following sections, we will show you several steps to use SOLL to generate ewasm file.

1. Build SOLL.
2. Generate an ewasm file from our demo test case.
3. Execute the ewasm file.



# Getting Started

To get started with our demo, you will need three components at first.

- Pre-install **Docker** and pull our [docker image](https://hub.docker.com/r/yi2nd/ubuntu-soll-build/) (optional)  

> *We provide an image include build and execute environment (recommend). If you don't want to use docker directly you will need below tools (cmake, llvm, node.js).*

- **Soll** https://github.com/second-state/soll
- **Test Suit** https://github.com/second-state/soll-ewasm-testbench

## Preparation
- Pull official docker image to get an already established build/execute environment.
```Shell
docker pull yi2nd/soll:v0.0.1
```

- Get Tar balls from Github and decompress these files
```Shell
wget -c https://github.com/second-state/soll/archive/0.0.2.tar.gz -O soll-0.0.2.tar.gz
tar zxvf 0.0.2.tar.gz soll-0.0.2.tar.gz
wget -c https://github.com/second-state/soll-ewasm-testbench/archive/0.0.2.tar.gz -O  ewasm-testbench-0.0.2.tar.gz
tar zxvf ewasm-testbench-0.0.2.tar.gz
```


## Working Tree
```
├── ewasm-testbench
│   ├── README.md
│   └── index.js              // Entry point for trigger ewasm 
└── soll
    ├── (...)
    ├── build                 // Build code path, manually create it
    └── test
        ├── lity
        │   └── safemath.lity // Solidity test contract (Test case)
        └── (...)
```

## Launch Environment
Attach shell to container and bind volume with repositories' path.
```Shell
docker run -it --rm \
      -v $(pwd)/ewasm-testbench-0.0.2:/root/ewasm-testbench \
      -v $(pwd)/soll-0.0.2:/root/soll \
      yi2nd/ubuntu-soll:v0.0.1
```

## Phase 1. Use SOLL generate .ll from test suite
Build SOLL first (we use cmake with llvm library) and then execute SOLL to generate a *.ll file for the next step.
```Shell
(docker) $ cd ~/soll && mkdir -p build && cd build
(docker) $ cmake .. && make
(docker) $ cd ~/ewasm-testbench
(docker) $ ../soll/build/tools/soll/soll ../soll/test/lity/safemath.lity > safemath.ll
```

## Phase 2. Generate .wasm from .ll
```Shell
(docker) $ cd ~/ewasm-testbench
(docker) $ ../soll/utils/compile -v safemath.ll
```

## Run 
We use "16 divides 7" as our smart contract function to check whether our "SafeMath" execute correctly or not.
```Shell
(docker) $ cd ~/ewasm-testbench
(docker) $ ./index.js safemath.wasm div 16 7
```

The result should be the same as the following content.

```Shell
callDataCopy(66716, 0, 4)
callDataCopy(66688, 4, 16)
finish(66672, 8)
[ '02', '00', '00', '00', '00', '00', '00', '00' ]
```

