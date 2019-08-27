# WHAT IS SOLL
**SOLL** is a new compiler for generate [EWASM](https://github.com/ewasm) from solidity.

Currently we apply two phases generate flow. Generate .ll (LLVM IR file) first, then generate .wasm (WASM file).

In this document we will show you how to achieve that goal.

1. Build soll.
2. Generate ewasm from our demo test case.
3. Execute wasm file.

# GETTING STARTED

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

- Git clone from official gitlab repository.
```Shell
git clone https://github.com/second-state/soll.git
git clone https://github.com/second-state/soll-ewasm-testbench.git ewasm-testbench
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

## Lauch Environment
Attach shell to container and bind volume with repositorys' path.
```Shell
docker run -it --rm \
      -v $(pwd)/ewasm-testbench:/root/ewasm-testbench \
      -v $(pwd)/soll:/root/soll \
      yi2nd/ubuntu-soll:v0.0.1
```

## Phase 1. Use soll generate .ll from test suite
Build soll first (we use cmake with llvm library).
```Shell
(docker) $ cd ~/soll && mkdir build && cd build
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
Trigger EWASM smart contract function call inside nativa javascript code.
```Shell
(docker) $ cd ~/ewasm-testbench
(docker) $ ./index.js safemath.wasm div 16 7
```
