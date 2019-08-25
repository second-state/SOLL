// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTContext.h"
#include "soll/Sema/BreakableVisitor.h"

namespace soll {

class Lexer;
class ASTContext;
class ASTConsumer;
class DiagnosticsEngine;
class SourceManager;

class Sema {
  Sema(const Sema &) = delete;
  Sema &operator=(const Sema &) = delete;

public:
  Lexer &Lex;
  ASTContext &Context;
  ASTConsumer &Consumer;
  DiagnosticsEngine &Diags;
  SourceManager &SourceMgr;

  Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer);

  ExprPtr CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS, ExprPtr &&RHS);
  ExprPtr CreateIndexAccess(ExprPtr &&LHS, ExprPtr &&RHS);
  ExprPtr CreateIdentifier(const std::string Name);
  /// type checking binary operators (subroutines of CreateBinOp)
  /// this may add type casting

  /// +, -
  TypePtr CheckAdditiveOperands(ExprPtr &LHS, ExprPtr &RHS,
                                BinaryOperatorKind Opc,
                                TypePtr CompLHSTy = nullptr);
  /// *, /, %
  TypePtr CheckMultiplicativeOperands(ExprPtr &LHS, ExprPtr &RHS,
                                      BinaryOperatorKind Opc,
                                      TypePtr CompLHSTy = nullptr);
  /// <, >, <=, >=, ==
  TypePtr CheckCompareOperands(ExprPtr &LHS, ExprPtr &RHS,
                               BinaryOperatorKind Opc);

  TypePtr UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS,
                                     bool IsCompAssign);

  ExprPtr UsualUnaryConversions(ExprPtr &&E);

  ExprPtr DefaultLvalueConversion(ExprPtr &&E);

  void addIdentifierDecl(const std::string &S, const Decl &D) {
    Context.addIdentifierDecl(S, D);
  }
  const Decl *findIdentifierDecl(const std::string &S) {
    return Context.findIdentifierDecl(S);
  }

  void resolveBreak(FunctionDecl &);
};

} // namespace soll
