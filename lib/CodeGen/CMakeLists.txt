# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
set(LLVM_LINK_COMPONENTS
  all
  )

add_llvm_library(sollCodeGen
  BackendUtil.cpp
  CodeGenAction.cpp
  CodeGenModule.cpp
  FuncBodyCodeGen.cpp
  LoweringInteger.cpp
  ModuleBuilder.cpp
  LINK_LIBS
  sollAST
  sollBasic
  sollFrontend
  SHA3
  )
