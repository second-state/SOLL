# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_executable(expr_testcase
  main.cpp
  )

target_link_libraries(expr_testcase
  PRIVATE
  sollBasic
  sollAST
  sollFrontend
  sollCodeGen
  )
