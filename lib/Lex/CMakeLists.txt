# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_library(sollLex
  Lexer.cpp
  )

target_link_libraries(sollLex
  sollBasic
  )
