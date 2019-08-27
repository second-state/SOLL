# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
include_directories(
  ${CMAKE_SOURCE_DIR}/utils/unittests
  )

set(LLVM_LINK_COMPONENTS
  all
  )

add_llvm_executable(unittests
  main.cpp
  AST/ExprTest.cpp
  )

target_link_libraries(unittests
  PRIVATE
  sollAST
  )

add_test(NAME libsoll_unittests
  COMMAND unittests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
