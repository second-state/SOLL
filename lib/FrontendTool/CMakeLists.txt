# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollFrontendTool
  STATIC
  ExecuteCompilerInvocation.cpp
  LINK_LIBS
  sollFrontend
  sollCodeGen
  )
