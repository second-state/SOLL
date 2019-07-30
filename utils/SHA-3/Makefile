

sha3_headers = Endian.h Rotation.h HashFunction.h Keccak.h CommandParser.h ParserCommon.h RAII_Wrapper.h Hex.h stdafx.h
sha3_sources = HashFunction.cpp  Keccak.cpp  CommandParser.cpp
sha3_deps = $(sha3_headers) $(sha3_sources)

sha3sum: $(sha3_deps)
	g++ -o sha3sum -O3 -std=c++0x $(sha3_sources)


