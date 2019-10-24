# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollAST
  ASTContext.cpp
  Decl.cpp
  DeclAccept.cpp
  DeclVisitor.cpp
  Expr.cpp
  ExprYul.cpp
  Stmt.cpp
  StmtYul.cpp
  StmtAccept.cpp
  StmtVisitor.cpp
  Type.cpp
  LINK_LIBS
  SHA3
  )
