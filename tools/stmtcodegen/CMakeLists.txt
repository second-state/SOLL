set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(stmtcodegen
  main.cpp
  )

target_link_libraries(stmtcodegen
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
