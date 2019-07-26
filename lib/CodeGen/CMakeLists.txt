add_library(sollCodeGen
  FuncBodyCodeGen.cpp
  )

target_link_libraries(sollCodeGen
  sollAST
  sollBasic
  )
