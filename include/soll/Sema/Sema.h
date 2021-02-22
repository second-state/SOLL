// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTContext.h"
#include "soll/AST/ExprAsm.h"
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

  llvm::StringMap<ContractDecl *> ContractDecls;
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

  void SetFunRtnTys(std::vector<TypePtr> &&Tys) { FunRtnTys = std::move(Tys); }
  void EraseFunRtnTys() { FunRtnTys.clear(); }

  // Decl
  std::unique_ptr<FunctionDecl> CreateFunctionDecl(
      SourceRange L, llvm::StringRef name, FunctionDecl::Visibility visibility,
      StateMutability sm, bool isConstructor, bool isFallback,
      std::unique_ptr<ParamList> &&params,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParamList> &&returnParams, std::unique_ptr<Block> &&body,
      bool isVirtual, std::unique_ptr<OverrideSpecifier> &&overrides);
  std::unique_ptr<EventDecl>
  CreateEventDecl(SourceRange L, llvm::StringRef name,
                  std::unique_ptr<ParamList> &&Params, bool Anonymous);

  // Expr
  std::unique_ptr<ImplicitCastExpr> CreateDummy(std::unique_ptr<Expr> &&Base);
  ExprPtr CreateBinOp(SourceRange L, BinaryOperatorKind Opc, ExprPtr &&LHS,
                      ExprPtr &&RHS);
  ExprPtr CreateIndexAccess(SourceLocation EndPos, ExprPtr &&LHS,
                            ExprPtr &&RHS);
  std::unique_ptr<CallExpr>
  CreateCallExpr(SourceRange L, ExprPtr &&Callee,
                 std::vector<std::unique_ptr<Expr>> &&Args);
  std::unique_ptr<CallExpr>
  CreateNamedCallExpr(SourceRange L, ExprPtr &&Callee,
                      std::vector<std::unique_ptr<Expr>> &&Args,
                      std::vector<llvm::StringRef> &&Names);
  std::unique_ptr<Identifier> CreateIdentifier(const Token &Tok);
  std::unique_ptr<MemberExpr> CreateMemberExpr(std::unique_ptr<Expr> &&BaseExpr,
                                               Token Tok);

  /// ExprAsm
  std::unique_ptr<AsmIdentifier> CreateAsmIdentifier(const Token &Tok,
                                                     bool IsCall);
  std::unique_ptr<Expr> CreateAsmCallExpr(SourceRange L, ExprPtr &&Callee,
                                          std::vector<ExprPtr> &&Args);
  std::unique_ptr<Expr> CreateAsmBuiltinCallExpr(SourceRange L,
                                                 const AsmIdentifier &Callee,
                                                 std::vector<ExprPtr> &&Args,
                                                 TypePtr ReturnTy);

  void PushScope(unsigned Flags) {
    Scopes.push_back(std::make_unique<Scope>(CurrentScope(), Flags));
  }
  void PopScope() {
    resolveIdentifiers();
    Scopes.pop_back();
  }
  Scope *CurrentScope() const {
    return Scopes.empty() ? nullptr : Scopes.back().get();
  }
  void addDecl(Decl *D) { CurrentScope()->addDecl(D); }
  bool addContractDecl(ContractDecl *D) {
    return ContractDecls.try_emplace(D->getName(), D).second;
  }
  Decl *lookupName(llvm::StringRef Name) const {
    return CurrentScope()->lookupName(Name);
  }

  ContractDecl *lookupContractDeclName(llvm::StringRef Name) const {
    if (auto Iter = ContractDecls.find(Name); Iter != ContractDecls.end()) {
      return Iter->second;
    }
    return nullptr;
  }
  void InjectInheritContract(std::vector<Decl *> &BaseDecl,
                             const std::vector<Decl *> &Child,
                             bool AppendChild);

  void resolveType(SourceUnit &SU);
  void resolveInherit(SourceUnit &SU);
  void resolveUniqueName(SourceUnit &SU);
  void resolveImplicitCast(ImplicitCastExpr &IC, TypePtr DstTy,
                           bool PrefereLValue);

private:
  void resolveIdentifiers();
};

} // namespace soll
