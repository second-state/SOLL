# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollSema
  Sema.cpp
  LINK_LIBS
  sollAST
  )
