TARGET=UniswapV2ERC20 UniswapV2Factory UniswapV2Pair
WASM=$(addsuffix .wasm,$(TARGET))
LLVM=$(addsuffix .ll,$(TARGET))
OBJECT=$(addsuffix .o,$(TARGET))
.PHONY: all clean
.PRECIOUS: %.o %.ll


all: $(WASM) 

%.ll: %.yul Makefile
	soll -lang=Yul --action=EmitLLVM $<

%.o: %.ll
	llc -O3 -dwarf-version=4 -filetype=obj --march=wasm32 $^ 

%.wasm: %.o
	wasm-ld --entry main --gc-sections --allow-undefined --export=__heap_base --max-memory=16777216 --initial-memory=16777216 $^ -o $@
	wasm-objdump --headers $@


clean: 
	rm $(WASM) $(LLVM) $(OBJECT)
