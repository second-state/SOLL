# Uniswap-V2 YUL TEST 

## Compile Uniswap Solidity Contracts into YUL files using solc

For the convenience of testing, the `mint` interface was added to the original uniswap-v2-erc20 interface.
We could use solc to compile solidity into YUL IR. 

```
solc --ir UniswapV2ERC20.sol > UniswapV2ERC20.yul
```

However, the compiled IR has deployment code. This test does not contain a runtime test, so you need to remove the deployment code manually.
It could be done by extracting` object "UniswapV2ERC20_431_deployed"` into a single YUL file.

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

Then, execute `python uniswap-test.py --input UniswapV2ERC20.wasm`. 

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

## Testing other Uniswap Interfaces

If we want to test Factory/Pair Interface, we need to set up a chain to initialize tokens.
However, the testing environment cannot support contract deployment, so we cannot test the full functionality of the interface.

Some interfaces inherited from ERC20 and some basic functions could still be tested with this environment.

```
python uniswap-test.py --input UniswapV2Pair.wasm
```

### Uniswap V2 Factory

After ERC20 tokens are deployed at some address, say `tokenA` is deployed at `addressA` and `tokenB` is deployed at `addressB`, we could create pair with Uniswap V2 Factory.  We could call `createPair` API to create a pair of tokens.

```
function createPair(address tokenA, address tokenB) external returns (address pair);
```

Further, after we create a pair of tokens, we could then validate the factory by checking its read-only functions.

```
function getPair(address tokenA, address tokenB) external view returns (address pair);
function allPairs(uint) external view returns (address pair);
```

The above process could test the correctness of Uniswap V2 Factory.

For more testing processes, please checkout [UniswapV2Factory test script](https://github.com/Uniswap/v2-core/blob/master/test/UniswapV2Factory.spec.ts)



### Uniswap V2 Pair

After we validate Uniswap V2 Factory, we could then validate Uniswap V2 Pair, which is the main interface of Uniswap. Pairs serve as automated market makers and keep track of pool token balances. List 5 tests below,

1. Mint  `function mint(address to) external returns (uint liquidity);`
    + Creates pool tokens to some address
2. Swap token `function swap(uint amount0Out, uint amount1Out, address to, bytes calldata data) external;`
    + Two directions of swapping
    + pay for the withdrawn ERC20 tokens with the corresponding pair tokens
    + return the withdrawn ERC20 tokens along with a small fee
3. burn `function burn(address to) external returns (uint amount0, uint amount1);`
    + Destroys pool tokens of some address
4. price Cumulative `function price{0,1}CumulativeLast() external view returns (uint);`
    + The  `price{0,1}CumulativeLast` is used by oracle to fetch the current accumulated price value
      + price0CumulativeLast = Token_1 / Token_0
      + price1CumulativeLast = Token_0 / Token_1
    + To calculate the correct price, please check [utilities.ts:encodePrice](https://github.com/Uniswap/v2-core/blob/master/test/shared/utilities.ts#L97), it divide the liquidity of 2 toekns 

For more testing processes, please checkout [UniswapV2Pair test script](https://github.com/Uniswap/v2-core/blob/master/test/UniswapV2Pair.spec.ts)

