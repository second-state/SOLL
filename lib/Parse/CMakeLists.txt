# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollParse
  ParseAST.cpp
  ParseAsm.cpp
  ParseLiteral.cpp
  ParseYul.cpp
  Parser.cpp
  LINK_LIBS
  sollBasic
  sollAST
  sollLex
  sollSema
  )
