# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_library(sollCodeGen
  BackendUtil.cpp
  CodeGenAction.cpp
  CodeGenModule.cpp
  FuncBodyCodeGen.cpp
  LoweringInteger.cpp
  ModuleBuilder.cpp
  )

target_link_libraries(sollCodeGen
  sollAST
  sollBasic
  sollFrontend
  SHA3
  )
