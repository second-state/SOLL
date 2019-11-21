// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTContext.h"
#include "soll/AST/ExprYul.h"
#include "soll/Sema/Scope.h"
#include <memory>
#include <vector>

namespace soll {

class ASTConsumer;
class ASTContext;
class DiagnosticsEngine;
class DiagnosticBuilder;
class Lexer;
class Sema;
class SourceManager;
class Token;

class Sema {
  Sema(const Sema &) = delete;
  Sema &operator=(const Sema &) = delete;

  std::vector<std::unique_ptr<Scope>> Scopes;
  std::vector<TypePtr> FunRtnTys;

public:
  Lexer &Lex;
  ASTContext &Context;
  ASTConsumer &Consumer;
  DiagnosticsEngine &Diags;
  SourceManager &SourceMgr;

  Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer);

  DiagnosticBuilder Diag(SourceLocation Loc, unsigned DiagID);

  void SetFunRtnTys(std::vector<TypePtr> Tys) {
    this->FunRtnTys = std::vector(Tys);
  }
  void EraseFunRtnTys() { this->FunRtnTys.clear(); }

  // Stmt
  StmtPtr CreateReturnStmt();
  StmtPtr CreateReturnStmt(ExprPtr &&Vaule);

  // Decl
  std::unique_ptr<FunctionDecl> CreateFunctionDecl(
      llvm::StringRef name, FunctionDecl::Visibility visibility,
      StateMutability sm, bool isConstructor, bool isFallback,
      std::unique_ptr<ParamList> &&params,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body);
  std::unique_ptr<EventDecl>
  CreateEventDecl(llvm::StringRef name, std::unique_ptr<ParamList> &&Params,
                  bool Anonymous);

  // Expr
  ExprPtr CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS, ExprPtr &&RHS);
  ExprPtr CreateIndexAccess(ExprPtr &&LHS, ExprPtr &&RHS);
  std::unique_ptr<CallExpr>
  CreateCallExpr(ExprPtr &&Callee, std::vector<std::unique_ptr<Expr>> &&Args);
  std::unique_ptr<Identifier> CreateIdentifier(Token Tok);
  std::unique_ptr<MemberExpr> CreateMemberExpr(std::unique_ptr<Expr> &&BaseExpr,
                                               Token Tok);

  /// ExprYul
  std::unique_ptr<YulIdentifier> CreateYulIdentifier(llvm::StringRef Name);
  std::unique_ptr<Expr> CreateYulCallExpr(ExprPtr &&Callee,
                                          std::vector<ExprPtr> &&Args);
  std::unique_ptr<Expr> CreateYulBuiltinCallExpr(const YulIdentifier &Callee,
                                                 std::vector<ExprPtr> &&Args,
                                                 TypePtr ReturnTy);

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
  /// <<, >>, <<=, >>=
  TypePtr CheckShiftOperands(ExprPtr &LHS, ExprPtr &RHS, BinaryOperatorKind Opc,
                             bool IsCompAssign = false);
  /// <, >, <=, >=, ==
  TypePtr CheckCompareOperands(ExprPtr &LHS, ExprPtr &RHS,
                               BinaryOperatorKind Opc);
  // &&, ||
  TypePtr CheckLogicalOperands(ExprPtr &LHS, ExprPtr &RHS,
                               BinaryOperatorKind Opc);
  // &, |
  TypePtr CheckBitwiseOperands(ExprPtr &LHS, ExprPtr &RHS,
                               BinaryOperatorKind Opc);
  TypePtr CheckAssignmentOperands(ExprPtr &LHS, ExprPtr &RHS,
                                  TypePtr CompoundType);

  TypePtr UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS,
                                     bool IsCompAssign);

  ExprPtr UsualUnaryConversions(ExprPtr &&E);

  ExprPtr DefaultLvalueConversion(ExprPtr &&E);

  void PushScope(unsigned Flags) {
    Scopes.push_back(std::make_unique<Scope>(CurrentScope(), Flags));
  }
  void PopScope() { Scopes.pop_back(); }
  Scope *CurrentScope() const {
    return Scopes.empty() ? nullptr : Scopes.back().get();
  }
  void addDecl(Decl *D) { CurrentScope()->addDecl(D); }
  Decl *lookupName(llvm::StringRef Name) const {
    return CurrentScope()->lookupName(Name);
  }
};

} // namespace soll
