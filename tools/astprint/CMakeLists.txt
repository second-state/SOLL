# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_executable(astprint
  main.cpp
  )

target_link_libraries(astprint
  PRIVATE
  sollAST
  sollFrontend
  )
