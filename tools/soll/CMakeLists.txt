# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
list(APPEND LLVM_LINK_COMPONENTS
  AllTargetsCodeGens
  AllTargetsAsmPrinters
  AllTargetsAsmParsers
  AllTargetsDescs
  AllTargetsInfos
  )

add_llvm_executable(soll
  main.cpp
  )

if(SOLL_COVERAGE)
target_link_libraries(soll
  PUBLIC
  gcov
  PRIVATE
  sollBasic
  sollFrontend
  sollFrontendTool
  )
else()
target_link_libraries(soll
  PRIVATE
  sollBasic
  sollFrontend
  sollFrontendTool
  )
endif()
