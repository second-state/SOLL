# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_executable(stmtcodegen
  main.cpp
  )

target_link_libraries(stmtcodegen
  PRIVATE
  sollBasic
  sollAST
  sollFrontend
  sollCodeGen
  sollSema
  )
