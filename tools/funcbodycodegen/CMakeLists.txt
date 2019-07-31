# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(functionboycodegen
  main.cpp
  )

target_link_libraries(functionboycodegen
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
