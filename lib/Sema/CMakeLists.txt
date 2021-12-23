# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollSema
  Scope.cpp
  Sema.cpp
  SemaExprAsm.cpp
  ReferenceResolver.cpp
  SemaResolveType.cpp
  SemaResolveScope.cpp
  SemaNameAndTypeResolver.cpp
  SemaResolveInherit.cpp
  SemaResolveUniqueName.cpp
  SemaResolveImmutable.cpp
  SemaResolveIdentifier.cpp
  LINK_LIBS
  sollAST
  )
