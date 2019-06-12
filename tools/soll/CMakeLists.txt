set(LLVM_LINK_COMPONENTS
  CodeGen
  )

add_llvm_executable(soll
  main.cpp
  )

target_link_libraries(soll
  PRIVATE
  sollBasic
  sollFrontend
  )
