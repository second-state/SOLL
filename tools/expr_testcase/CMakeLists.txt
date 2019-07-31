set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(expr_testcase
  main.cpp
  )

target_link_libraries(expr_testcase
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
