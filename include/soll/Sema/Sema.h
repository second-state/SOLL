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
  // TODO: refactor this
  // current impl. assumes no name scope
  std::unordered_map<std::string, Decl *> ID2DeclTable;

public:
  Lexer &Lex;
  ASTContext &Context;
  ASTConsumer &Consumer;
  DiagnosticsEngine &Diags;
  SourceManager &SourceMgr;

  Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer);

  // Decl
  std::unique_ptr<FunctionDecl> CreateFunctionDecl(
      llvm::StringRef name, FunctionDecl::Visibility visibility,
      StateMutability sm, bool isConstructor,
      std::unique_ptr<ParamList> &&Params,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body);

  // Expr
  ExprPtr CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS, ExprPtr &&RHS);
  ExprPtr CreateIndexAccess(ExprPtr &&LHS, ExprPtr &&RHS);
  ExprPtr CreateCallExpr(ExprPtr &&Callee,
                         std::vector<std::unique_ptr<Expr>> &&Args);
  std::unique_ptr<Identifier> CreateIdentifier(const std::string Name);

  // Stmt
  StmtPtr CreateReturnStmt(ExprPtr &&Vaule);

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
  TypePtr CheckAssignmentOperands(ExprPtr &LHS, ExprPtr &RHS,
                                  TypePtr CompoundType);

  TypePtr UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS,
                                     bool IsCompAssign);

  ExprPtr UsualUnaryConversions(ExprPtr &&E);

  ExprPtr DefaultLvalueConversion(ExprPtr &&E);

  void addIdentifierDecl(const std::string &Name, Decl &D);
  Decl *findIdentifierDecl(const std::string &Name);

  void resolveBreak(FunctionDecl &);

  bool isMagicFuncName(const std::string &Name) {
    // TODO: replace this
    // check all magic functionName
    return Name == "require";
  }
};

} // namespace soll
