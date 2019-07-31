# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(astdump
  main.cpp
  )

target_link_libraries(astdump
  PRIVATE
  sollBasic
  sollFrontend
  )
