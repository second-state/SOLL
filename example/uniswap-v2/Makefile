all: UniswapV2ERC20.wasm

UniswapV2ERC20.ll: UniswapV2ERC20.yul Makefile
	soll -lang=Yul --action=EmitLLVM UniswapV2ERC20.yul

UniswapV2ERC20.o: UniswapV2ERC20.ll
	llc -O3 -dwarf-version=4 -filetype=obj --march=wasm32 UniswapV2ERC20.ll 

UniswapV2ERC20.wasm: UniswapV2ERC20.o
	wasm-ld --entry main --gc-sections --allow-undefined --export=__heap_base --max-memory=16777216 --initial-memory=16777216 UniswapV2ERC20.o -o UniswapV2ERC20.wasm
	wasm-objdump --headers UniswapV2ERC20.wasm

clean: 
	rm UniswapV2ERC20.ll UniswapV2ERC20.o UniswapV2ERC20.wasm
