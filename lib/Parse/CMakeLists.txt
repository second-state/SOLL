# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollParse
  ParseAST.cpp
  Parser.cpp
  ParseYul.cpp
  ParseAsm.cpp
  LINK_LIBS
  sollBasic
  sollAST
  sollLex
  sollSema
  )
