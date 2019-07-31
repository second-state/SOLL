# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(LLVM_LINK_COMPONENTS
  all
  )

add_llvm_executable(soll
  main.cpp
  )

target_link_libraries(soll
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
