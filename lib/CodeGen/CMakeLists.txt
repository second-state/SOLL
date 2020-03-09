# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollCodeGen
  BackendUtil.cpp
  CGExpr.cpp
  CodeGenAction.cpp
  CodeGenFunction.cpp
  CodeGenModule.cpp
  LoweringInteger.cpp
  ModuleBuilder.cpp
  LINK_LIBS
  sollAST
  sollFrontend
  lldWasm
  lldCommon
  binaryen
  LINK_COMPONENTS
  codegen
  passes
  support
#  webassembly
  )

if (SOLL_ENABLE_EVM)
  add_definitions(-DENABLE_EVM_BACKEND)
endif()
