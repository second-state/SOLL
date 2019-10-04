# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollLex
  Lexer.cpp
  LINK_LIBS
  sollBasic
  )
