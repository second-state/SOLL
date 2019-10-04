# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_executable(functionboycodegen
  main.cpp
  )

target_link_libraries(functionboycodegen
  PRIVATE
  sollBasic
  sollAST
  sollFrontend
  sollCodeGen
  )
