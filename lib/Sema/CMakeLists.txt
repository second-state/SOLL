# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollSema
  Scope.cpp
  Sema.cpp
  SemaExprAsm.cpp
  SemaResolveType.cpp
  SemaResolveNameAndType.cpp
  SemaResolveInherit.cpp
  SemaResolveUniqueName.cpp
  SemaResolveIdentifier.cpp
  LINK_LIBS
  sollAST
  )
