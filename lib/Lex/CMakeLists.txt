# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollLex
  STATIC
  Lexer.cpp
  LINK_LIBS
  sollBasic
  )
