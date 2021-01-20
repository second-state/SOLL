SOLL_BASE="../.."

cp ${SOLL_BASE}/build/utils/ssvm-evmc/tools/soll-runtime-test/libsoll_runtime_test.so ${PWD}
cp ${SOLL_BASE}/build/utils/ssvm-evmc/tools/ssvm-evmc/libssvm-evmc.so ${PWD}

python soll-runtime-test.py

rm *.so
