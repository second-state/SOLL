# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(IndexAccess
  main.cpp
  )

target_link_libraries(IndexAccess
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
