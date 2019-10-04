# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_executable(IndexAccess
  main.cpp
  )

target_link_libraries(IndexAccess
  PRIVATE
  sollBasic
  sollAST
  sollFrontend
  sollCodeGen
  )
