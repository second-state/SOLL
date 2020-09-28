# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollBasic
  STATIC
  Diagnostic.cpp
  DiagnosticIDs.cpp
  FileManager.cpp
  IdentifierTable.cpp
  SourceLocation.cpp
  SourceManager.cpp
  TokenKinds.cpp
  OperatorPrecedence.cpp
  LINK_COMPONENTS
  support
  )
