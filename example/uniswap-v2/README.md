# Uniswap-V2 YUL TEST 

## Compile Uniswap Solidity Contracts into YUL files using solc

For the convenience of testing, the `mint` interface was added to the original uniswap-v2-erc20 interface.
We could use solc to compile solidity into YUL IR. 

```
solc --ir UniswapV2ERC20.sol > UniswapV2ERC20.yul
```

However, the compiled ir has deployment code. This test does not contain runtime test, so you need to remove the deployment code manually. 
It could be done by extract `object "UniswapV2ERC20_431_deployed"` into a single YUL file.

A preprocessed `UniswapV2ERC20.yul` is included in the folder, you could use the file directly.

## Compile YUL IR into WASM through SOLL toolchain

Prepare SOLL toolchain, and we could compile `UniswapV2ERC20.yul` into `UniswapV2ERC20.wasm`. You can use the Makefile in this folder.

```
make
```

## Compile libsoll_runtime_test

Required files:

+ `libssvm-evmc.so` a evmc wrapper for ssvm
+ `libsoll_runtime_test.so` a middle layer between python and `ssvm-evmc` 
+ `evmc.py` a python ctype wrapper for `libsoll_runtime_test.so`

Please clone and build [ssvm-evmc](https://github.com/second-state/ssvm-evmc).
```
$ git clone git@github.com:second-state/ssvm-evmc.git
$ cd ssvm-evmc
$ git checkout master
```

Build `libsoll_runtime_test.so` and `libssvm-evmc.so `.
```
$ cd <path/to/ssvm-evmc>
$ mkdir -p build && cd build
```

Copy the libraries to this folder.
```
$ cd <path/to/erc20-rs-ssvm-test>
$ cp <path/to/ssvm-evmc>/build/tools/soll-runtime-test/libsoll_runtime_test.so .
$ cp <path/to/ssvm-evmc>/build/tools/tools/ssvm-evmc/libssvm-evmc.so .
```


## Testing UniswapV2ERC20.wasm

After compiling `UniswapV2ERC20.wasm` and soll-runtime-test. Make sure that the result files are copied into this directory:

+ `UniswapV2ERC20.wasm`
+ `libsoll_runtime_test.so`
+ `libssvm-evmc.so `

Then, execute `python uniswap-test.py`. 

```
name
status:  EVMC_SUCCESS
return:  b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00 \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\nUniswap V2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
=====================================
symbol
status:  EVMC_SUCCESS
return:  b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00 \x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06UNI-V2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
=====================================
decimals
status:  EVMC_SUCCESS
return:  18
=====================================
total_supply
status:  EVMC_SUCCESS
return:  0
=====================================
mint_0xDEADBEEF_3
status:  EVMC_SUCCESS
return:  1
=====================================
total_supply
status:  EVMC_SUCCESS
return:  3
=====================================
balance_of_0xDEADBEEF
status:  EVMC_SUCCESS
return:  3
=====================================
balance_of_0xFACEB00C
status:  EVMC_SUCCESS
return:  0
=====================================
transfer_0xDEADBEEF_1_to_0xFACEB00C
status:  EVMC_SUCCESS
return:  1
=====================================
balance_of_0xDEADBEEF
status:  EVMC_SUCCESS
return:  2
=====================================
balance_of_0xFACEB00C
status:  EVMC_SUCCESS
return:  1
=====================================
total_supply
status:  EVMC_SUCCESS
return:  3
=====================================
transfer_0xDEADBEEF_5_to_0xFACEB00C
status:  EVMC_REVERT
return:  1
=====================================
```
