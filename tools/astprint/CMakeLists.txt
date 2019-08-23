set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(astprint
  main.cpp
  )

target_link_libraries(astprint
  PRIVATE
  sollBasic
  sollFrontend
  sollCodeGen
  )
