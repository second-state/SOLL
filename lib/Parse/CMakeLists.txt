# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_library(sollParse
  ParseAST.cpp
  Parser.cpp
  )

target_link_libraries(sollParse
  sollAST
  sollBasic
  )
