# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
add_llvm_library(sollAST
  ASTContext.cpp
  Decl.cpp
  DeclYul.cpp
  DeclAccept.cpp
  DeclVisitor.cpp
  Expr.cpp
  ExprAsm.cpp
  Stmt.cpp
  StmtAsm.cpp
  StmtAccept.cpp
  StmtVisitor.cpp
  Type.cpp
  LINK_LIBS
  SHA3
  )
