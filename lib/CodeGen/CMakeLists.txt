add_library(sollCodeGen
  BackendUtil.cpp
  CodeGenAction.cpp
  CodeGenModule.cpp
  FuncBodyCodeGen.cpp
  ModuleBuilder.cpp
  )

target_link_libraries(sollCodeGen
  sollAST
  sollBasic
  sollFrontend
  )
