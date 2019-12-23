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
  std::string Name;
  Visibility Vis;

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
  Visibility getVisibility() const { return Vis; }
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
  std::unique_ptr<FunctionDecl> Constructor;
  std::unique_ptr<FunctionDecl> Fallback;
  ContractKind Kind;

public:
  ContractDecl(
      llvm::StringRef name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      std::vector<DeclPtr> &&subNodes,
      std::unique_ptr<FunctionDecl> &&constructor,
      std::unique_ptr<FunctionDecl> &&fallback,
      ContractKind kind = ContractKind::Contract)
      : Decl(name), BaseContracts(std::move(baseContracts)),
        SubNodes(std::move(subNodes)), Constructor(std::move(constructor)),
        Fallback(std::move(fallback)), Kind(kind) {}

  std::vector<Decl *> getSubNodes();
  std::vector<const Decl *> getSubNodes() const;

  std::vector<VarDecl *> getVars();
  std::vector<const VarDecl *> getVars() const;

  std::vector<FunctionDecl *> getFuncs();
  std::vector<const FunctionDecl *> getFuncs() const;

  std::vector<EventDecl *> getEvents();
  std::vector<const EventDecl *> getEvents() const;

  FunctionDecl *getConstructor();
  const FunctionDecl *getConstructor() const;

  FunctionDecl *getFallback();
  const FunctionDecl *getFallback() const;

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
  CallableVarDecl(
      llvm::StringRef name, Visibility visibility,
      std::unique_ptr<ParamList> &&params,
      std::unique_ptr<ParamList> &&returnParams = std::make_unique<ParamList>(
          std::vector<std::unique_ptr<VarDeclBase>>()))
      : Decl(name, visibility), Params(std::move(params)),
        ReturnParams(std::move(returnParams)) {}

  ParamList *getParams() { return Params.get(); }
  const ParamList *getParams() const { return Params.get(); }

  ParamList *getReturnParams() { return ReturnParams.get(); }
  const ParamList *getReturnParams() const { return ReturnParams.get(); }

  std::vector<unsigned char> getSignatureHash() const;
  std::uint32_t getSignatureHashUInt32() const;

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class FunctionDecl : public CallableVarDecl {
  StateMutability SM;
  bool IsConstructor;
  bool IsFallback;
  std::vector<std::unique_ptr<ModifierInvocation>> FunctionModifiers;
  std::unique_ptr<Block> Body;
  bool Implemented;
  TypePtr FuncTy;

public:
  FunctionDecl(llvm::StringRef name, Visibility visibility, StateMutability sm,
               bool isConstructor, bool isFallback,
               std::unique_ptr<ParamList> &&params,
               std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
               std::unique_ptr<ParamList> &&returnParams,
               std::unique_ptr<Block> &&body);

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }
  void setBody(std::unique_ptr<Block> &&B) { Body = std::move(B); }
  TypePtr getType() const { return FuncTy; }
  StateMutability getStateMutability() const { return SM; }
  bool isConstructor() const { return IsConstructor; }
  bool isFallback() const { return IsFallback; }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class EventDecl : public CallableVarDecl {
  bool IsAnonymous;
  TypePtr FuncTy;

public:
  EventDecl(llvm::StringRef name, std::unique_ptr<ParamList> &&params,
            bool isAnonymous = false);

  TypePtr getType() const { return FuncTy; }
  bool isAnonymous() const { return IsAnonymous; }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class ParamList {
  std::vector<std::unique_ptr<VarDeclBase>> Params;

public:
  ParamList(std::vector<std::unique_ptr<VarDeclBase>> &&params)
      : Params(std::move(params)) {}

  std::vector<const VarDeclBase *> getParams() const;
  std::vector<VarDeclBase *> getParams();
  unsigned getABIStaticSize() const;

  void accept(DeclVisitor &visitor);
  void accept(ConstDeclVisitor &visitor) const;
};

class VarDeclBase : public Decl {
  TypePtr TypeName;
  ExprPtr Value;

public:
  VarDeclBase(TypePtr &&T, llvm::StringRef name, ExprPtr &&value,
              Visibility visibility)
      : Decl(name, visibility), TypeName(std::move(T)),
        Value(std::move(value)) {}

  TypePtr GetType() { return TypeName; }
  const TypePtr &GetType() const { return TypeName; }
  Expr *GetValue() { return Value.get(); }
  const Expr *GetValue() const { return Value.get(); }
};

class VarDecl : public VarDeclBase {
public:
  enum class Location { Unspecified, Storage, Memory, CallData };

private:
  bool IsStateVariable;
  bool IsIndexed;
  bool IsConstant;
  Location ReferenceLocation;

public:
  VarDecl(TypePtr &&T, llvm::StringRef name, ExprPtr &&value,
          Visibility visibility, bool isStateVar = false,
          bool isIndexed = false, bool isConstant = false,
          Location referenceLocation = Location::Unspecified)
      : VarDeclBase(std::move(T), name, std::move(value), visibility),
        IsStateVariable(isStateVar), IsIndexed(isIndexed),
        IsConstant(isConstant), ReferenceLocation(referenceLocation) {}

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;

  Location getLoc() const { return ReferenceLocation; }
  bool isIndexed() const { return IsIndexed; }
  bool isStateVariable() const { return IsStateVariable; }
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
