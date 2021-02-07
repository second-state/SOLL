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
  SourceRange Location;
  std::string Name;
  Visibility Vis;

protected:
  friend class ASTReader;

protected:
  Decl(SourceRange L,
       llvm::StringRef Name = llvm::StringRef::withNullAsEmpty(nullptr),
       Visibility vis = Visibility::Default)
      : Location(L), Name(Name.str()), Vis(vis) {}

public:
  virtual void accept(DeclVisitor &visitor) = 0;
  virtual void accept(ConstDeclVisitor &visitor) const = 0;
  const SourceRange &getLocation() const { return Location; }
  llvm::StringRef getName() const { return Name; }
  Visibility getVisibility() const { return Vis; }
};

class SourceUnit : public Decl {
  std::vector<DeclPtr> Nodes;

public:
  SourceUnit(SourceRange L, std::vector<DeclPtr> &&Nodes)
      : Decl(L), Nodes(std::move(Nodes)) {}

  void setNodes(std::vector<DeclPtr> &&Nodes);

  std::vector<Decl *> getNodes();
  std::vector<const Decl *> getNodes() const;

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class PragmaDirective : public Decl {
public:
  PragmaDirective(SourceRange L) : Decl(L) {}
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
  TypePtr ContractTy;

public:
  ContractDecl(
      SourceRange L, llvm::StringRef Name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      std::vector<DeclPtr> &&subNodes,
      std::unique_ptr<FunctionDecl> &&constructor,
      std::unique_ptr<FunctionDecl> &&fallback,
      ContractKind kind = ContractKind::Contract, TypePtr ContractTy = nullptr)
      : Decl(L, Name), BaseContracts(std::move(baseContracts)),
        SubNodes(std::move(subNodes)), Constructor(std::move(constructor)),
        Fallback(std::move(fallback)), Kind(kind), ContractTy(ContractTy) {}
  TypePtr getType() { return ContractTy; }
  void setType(TypePtr Ty) { ContractTy = Ty; }
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
  CallableVarDecl(SourceRange L, llvm::StringRef Name, Visibility V,
                  std::unique_ptr<ParamList> &&Params,
                  std::unique_ptr<ParamList> &&ReturnParams = nullptr)
      : Decl(L, Name, V), Params(std::move(Params)),
        ReturnParams(std::move(ReturnParams)) {}

  ParamList *getParams() { return Params.get(); }
  const ParamList *getParams() const { return Params.get(); }

  ParamList *getReturnParams() { return ReturnParams.get(); }
  const ParamList *getReturnParams() const { return ReturnParams.get(); }

  std::vector<unsigned char> getSignatureHash() const;
  std::uint32_t getSignatureHashUInt32() const;

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;
};

class FunctionDecl : public CallableVarDecl {
  StateMutability SM;
  bool IsConstructor;
  bool IsFallback;
  std::vector<std::unique_ptr<ModifierInvocation>> FunctionModifiers;
  std::unique_ptr<Block> Body;
  TypePtr FuncTy;

public:
  FunctionDecl(SourceRange L, llvm::StringRef Name, Visibility V,
               StateMutability SM, bool IsConstructor, bool IsFallback,
               std::unique_ptr<ParamList> &&Params,
               std::vector<std::unique_ptr<ModifierInvocation>> &&Modifiers,
               std::unique_ptr<ParamList> &&ReturnParams,
               std::unique_ptr<Block> &&Body);

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }
  void setBody(std::unique_ptr<Block> &&B) { Body = std::move(B); }
  TypePtr &getType() { return FuncTy; }
  const TypePtr &getType() const { return FuncTy; }
  StateMutability getStateMutability() const { return SM; }
  bool isConstructor() const { return IsConstructor; }
  bool isFallback() const { return IsFallback; }

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;
};

class EventDecl : public CallableVarDecl {
  bool IsAnonymous;
  TypePtr FuncTy;

public:
  EventDecl(SourceRange L, llvm::StringRef Name,
            std::unique_ptr<ParamList> &&Params, bool IsAnonymous = false);

  TypePtr getType() const { return FuncTy; }
  bool isAnonymous() const { return IsAnonymous; }

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;
};

class ParamList {
  std::vector<std::unique_ptr<VarDeclBase>> Params;

public:
  ParamList(std::vector<std::unique_ptr<VarDeclBase>> &&params)
      : Params(std::move(params)) {}

  std::vector<const VarDeclBase *> getParams() const;
  std::vector<VarDeclBase *> getParams();
  unsigned getABIStaticSize() const;

  void accept(DeclVisitor &Visitor);
  void accept(ConstDeclVisitor &Visitor) const;
};

class VarDeclBase : public Decl {
  TypePtr TypeName;
  ExprPtr Value;

public:
  VarDeclBase(SourceRange L, llvm::StringRef Name, Visibility Vi, TypePtr &&T,
              ExprPtr &&V)
      : Decl(L, Name, Vi), TypeName(std::move(T)), Value(std::move(V)) {}

  TypePtr getType() { return TypeName; }
  const TypePtr &getType() const { return TypeName; }
  void setType(TypePtr Ty) { TypeName = Ty; }
  Expr *getValue() { return Value.get(); }
  const Expr *getValue() const { return Value.get(); }
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
  VarDecl(SourceRange L, llvm::StringRef Name, Visibility Vi, TypePtr &&T,
          ExprPtr &&V, bool IsStateVar = false, bool IsIndexed = false,
          bool IsConstant = false,
          Location ReferenceLocation = Location::Unspecified)
      : VarDeclBase(L, Name, Vi, std::move(T), std::move(V)),
        IsStateVariable(IsStateVar), IsIndexed(IsIndexed),
        IsConstant(IsConstant), ReferenceLocation(ReferenceLocation) {}

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;

  Location getLoc() const { return ReferenceLocation; }
  bool isIndexed() const { return IsIndexed; }
  bool isStateVariable() const { return IsStateVariable; }
};

class StructDecl : public Decl {
private:
  TypePtr Ty;
  TypePtr ConstructorTy;

public:
  StructDecl(SourceRange L, llvm::StringRef Name, std::vector<TypePtr> &&ET,
             std::vector<std::string> &&EN);

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;

  TypePtr getType() const { return Ty; }
  TypePtr getConstructorType() const { return ConstructorTy; }
};

class ModifierInvocation {
  std::string ModifierName;
  std::vector<ExprPtr> Arguments;

public:
  ModifierInvocation(llvm::StringRef Name, std::vector<ExprPtr> Arguments)
      : ModifierName(Name), Arguments(std::move(Arguments)) {}

  const std::string &getName() const { return ModifierName; }

  void accept(DeclVisitor &Visitor);
  void accept(ConstDeclVisitor &Visitor) const;
};

} // namespace soll
