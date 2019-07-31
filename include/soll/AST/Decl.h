// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceLocation.h"
#include <vector>

namespace soll {

class ASTContext;

class Decl {
public:
  enum class Visibility { Default, Private, Internal, Public, External };
  virtual ~Decl() noexcept {}

private:
  Visibility Vis;
  std::string Name;

protected:
  friend class ASTReader;

protected:
  Decl() {}
  Decl(llvm::StringRef Name, Visibility vis = Visibility::Default)
      : Name(Name.str()), Vis(vis) {}

public:
  virtual void accept(DeclVisitor &visitor) = 0;
  virtual void accept(ConstDeclVisitor &visitor) const = 0;
  llvm::StringRef getName() const { return Name; }
};

class SourceUnit : public Decl {
  std::vector<DeclPtr> Nodes;

public:
  SourceUnit(std::vector<DeclPtr> &&Nodes) : Nodes(std::move(Nodes)) {}

  void setNodes(std::vector<DeclPtr> &&Nodes);

  std::vector<Decl *> getNodes();
  std::vector<const Decl *> getNodes() const;

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class PragmaDirective : public Decl {
public:
  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class ContractDecl : public Decl {
public:
  enum class ContractKind { Interface, Contract, Library };

private:
  std::vector<std::unique_ptr<InheritanceSpecifier>> BaseContracts;
  std::vector<DeclPtr> SubNodes;
  ContractKind Kind;

public:
  ContractDecl(
      llvm::StringRef name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      std::vector<DeclPtr> &&subNodes,
      ContractKind kind = ContractKind::Contract)
      : Decl(name), BaseContracts(std::move(baseContracts)),
        SubNodes(std::move(subNodes)), Kind(kind) {}

  std::vector<Decl *> getSubNodes();
  std::vector<const Decl *> getSubNodes() const;

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class InheritanceSpecifier {
  std::string BaseName;
  std::vector<ExprPtr> Arguments;

public:
  InheritanceSpecifier(llvm::StringRef baseName,
                       std::vector<ExprPtr> &&arguments)
      : BaseName(baseName.str()), Arguments(std::move(arguments)) {}
};

class CallableVarDecl : public Decl {
  std::unique_ptr<ParamList> Params;
  std::unique_ptr<ParamList> ReturnParams;

public:
  CallableVarDecl(llvm::StringRef name, Visibility visibility,
                  std::unique_ptr<ParamList> &&Params,
                  std::unique_ptr<ParamList> &&returnParams = nullptr)
      : Decl(name, visibility), Params(std::move(Params)),
        ReturnParams(std::move(returnParams)) {}

  ParamList *getParams() { return Params.get(); }
  const ParamList *getParams() const { return Params.get(); }

  ParamList *getReturnParams() { return ReturnParams.get(); }
  const ParamList *getReturnParams() const { return ReturnParams.get(); }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

enum class StateMutability { Pure, View, NonPayable, Payable };

class FunctionDecl : public CallableVarDecl {
  StateMutability SM;
  bool IsConstructor;
  std::vector<std::unique_ptr<ModifierInvocation>> FunctionModifiers;
  std::unique_ptr<Block> Body;
  bool Implemented;

public:
  FunctionDecl(llvm::StringRef name, Visibility visibility, StateMutability sm,
               bool isConstructor, std::unique_ptr<ParamList> &&Params,
               std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
               std::unique_ptr<ParamList> &&returnParams,
               std::unique_ptr<Block> &&body)
      : CallableVarDecl(name, visibility, std::move(Params),
                        std::move(returnParams)),
        SM(sm), IsConstructor(isConstructor),
        FunctionModifiers(std::move(modifiers)), Body(std::move(body)),
        Implemented(body != nullptr) {}

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class ParamList {
  std::vector<std::unique_ptr<VarDecl>> Params;

public:
  ParamList(std::vector<std::unique_ptr<VarDecl>> &&Params)
      : Params(std::move(Params)) {}

  std::vector<const VarDecl *> getParams() const;
  std::vector<VarDecl *> getParams();

  void accept(DeclVisitor &visitor);
  void accept(ConstDeclVisitor &visitor) const;
};

class VarDecl : public Decl {
public:
  enum class Location { Unspecified, Storage, Memory, CallData };

private:
  TypePtr TypeName;
  ExprPtr Value;
  bool IsStateVariable;
  bool IsIndexed;
  bool IsConstant;
  Location ReferenceLocation;

public:
  VarDecl(TypePtr &&T, llvm::StringRef name, ExprPtr &&value,
          Visibility visibility, bool isStateVar = false,
          bool isIndexed = false, bool isConstant = false,
          Location referenceLocation = Location::Unspecified)
      : Decl(name, visibility), TypeName(std::move(T)), Value(std::move(value)),
        IsStateVariable(isStateVar), IsIndexed(isIndexed),
        IsConstant(isConstant), ReferenceLocation(referenceLocation) {}

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;

  Type *GetType() { return TypeName.get(); }
  const Type *GetType() const { return TypeName.get(); }
};

class ModifierInvocation {
  std::string ModifierName;
  std::vector<ExprPtr> Arguments;

public:
  ModifierInvocation(llvm::StringRef name, std::vector<ExprPtr> arguments)
      : ModifierName(name), Arguments(std::move(arguments)) {}

  const std::string &getName() const { return ModifierName; }

  void accept(DeclVisitor &visitor);
  void accept(ConstDeclVisitor &visitor) const;
};

} // namespace soll
