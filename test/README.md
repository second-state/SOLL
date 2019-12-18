[//]: # (SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception)

# 1. Introduction
This directory contains all testing files for SOLL.

# 2. Preparation

1. ctest
`$ apt install ctest`
2. llvm-lit
`$ apt install llvm-8-tools`
2. python-psutil
`$ apt install python-psutil`

# 3. Run tests
```
$ cd ~/soll && mkdir -p build && cd build
$ cmake -DSOLL_INCLUDE_TESTS=ON -DLLVM_EXTERNAL_LIT=/usr/lib/llvm-8/build/utils/lit/lit.py .. && make -j4
$ ctest -V
```
