# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollSema
  STATIC
  Scope.cpp
  Sema.cpp
  SemaExprAsm.cpp
  SemaResolveType.cpp
  SemaResolveInherit.cpp
  SemaResolveUniqueName.cpp
  SemaResolveIdentifier.cpp
  LINK_LIBS
  sollAST
  )
