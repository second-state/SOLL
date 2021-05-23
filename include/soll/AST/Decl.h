// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceLocation.h"
#include "soll/Lex/Token.h"
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
  std::string UniqueName;

protected:
  friend class ASTReader;

protected:
  Decl(SourceRange L,
       llvm::StringRef Name = llvm::StringRef::withNullAsEmpty(nullptr),
       Visibility vis = Visibility::Default)
      : Location(L), Name(Name.str()), Vis(vis), UniqueName(Name.str()) {}

public:
  virtual void accept(DeclVisitor &visitor) = 0;
  virtual void accept(ConstDeclVisitor &visitor) const = 0;
  const SourceRange &getLocation() const { return Location; }
  llvm::StringRef getName() const { return Name; }
  llvm::StringRef getUniqueName() const { return UniqueName; }
  void setUniqueName(llvm::StringRef NewName) { UniqueName = NewName.str(); }
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

class IdentifierPath {
  std::vector<std::string> Path;

public:
  IdentifierPath(std::vector<std::string> &&P) : Path(std::move(P)) {}
  std::vector<std::string> &getPath() { return Path; }
  const std::vector<std::string> &getPath() const { return Path; }
};

class ContractDecl;

class UsingFor : public Decl {
public:
  UsingFor(SourceRange L, std::unique_ptr<IdentifierPath> &&LibraryName,
           TypePtr TypeName)
      : Decl(L), LibraryName(std::move(LibraryName)), TypeName(TypeName) {}

  IdentifierPath const &getLibraryName() const { return *LibraryName; }
  const std::vector<ContractDecl *> &getLibraries() const { return Libraries; }
  TypePtr getType() { return TypeName; }
  const TypePtr &getType() const { return TypeName; }
  void setType(TypePtr Ty) { TypeName = Ty; }
  void addLibrary(ContractDecl *Lib) { Libraries.emplace_back(Lib); }

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;

private:
  std::unique_ptr<IdentifierPath> LibraryName;
  std::vector<ContractDecl *> Libraries;
  TypePtr TypeName;
};

class ContractDecl : public Decl {
public:
  enum class ContractKind { Interface, Contract, Library };

private:
  std::vector<std::unique_ptr<InheritanceSpecifier>> BaseContracts;
  std::vector<ContractDecl *> ResolvedBaseContracts;
  std::vector<std::unique_ptr<UsingFor>> UsingForNodes;
  std::vector<DeclPtr> SubNodes;
  std::vector<Decl *> InheritNodes;
  std::unique_ptr<FunctionDecl> Constructor;
  std::unique_ptr<FunctionDecl> Fallback;
  TypePtr ContractTy;
  ContractKind Kind;
  bool IsAbstract;
  std::string LLVMMainFuncName;
  std::string LLVMContractFuncName;
  std::string LLVMCtorFuncName;

  llvm::StringMap<llvm::StringMap<std::pair<ContractDecl *, FunctionDecl *>>>
      TypeMemberMap;

public:
  ContractDecl(
      SourceRange L, llvm::StringRef Name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      std::vector<std::unique_ptr<UsingFor>> &&UsingForNodes,
      std::vector<DeclPtr> &&subNodes,
      std::unique_ptr<FunctionDecl> &&constructor,
      std::unique_ptr<FunctionDecl> &&fallback, TypePtr ContractTy = nullptr,
      ContractKind kind = ContractKind::Contract, bool isAbstract = false)
      : Decl(L, Name), BaseContracts(std::move(baseContracts)),
        UsingForNodes(std::move(UsingForNodes)), SubNodes(std::move(subNodes)),
        Constructor(std::move(constructor)), Fallback(std::move(fallback)),
        ContractTy(ContractTy), Kind(kind), IsAbstract(isAbstract) {}

  ContractKind getKind() const;
  bool isImplemented();
  TypePtr getType() { return ContractTy; }
  const TypePtr &getType() const { return ContractTy; }
  void setType(TypePtr Ty) { ContractTy = Ty; }
  llvm::StringMap<llvm::StringMap<std::pair<ContractDecl *, FunctionDecl *>>> &
  getTypeMemberMap() {
    return TypeMemberMap;
  }

  std::vector<InheritanceSpecifier *> getBaseContracts();
  std::vector<const InheritanceSpecifier *> getBaseContracts() const;

  void setResolvedBaseContracts(const std::vector<ContractDecl *> &);
  std::vector<ContractDecl *> getResolvedBaseContracts();
  std::vector<const ContractDecl *> getResolvedBaseContracts() const;

  std::vector<UsingFor *> getUsingForNodes();
  std::vector<const UsingFor *> getUsingForNodes() const;

  std::vector<Decl *> getSubNodes();
  std::vector<const Decl *> getSubNodes() const;

  std::vector<Decl *> getInheritNodes();
  std::vector<Decl *> &getInheritNodesRef();
  std::vector<const Decl *> getInheritNodes() const;

  void setInheritNodes(const std::vector<Decl *> &);

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

  void resolveLLVMFuncName();
  llvm::StringRef getLLVMMainFuncName() const { return LLVMMainFuncName; }
  llvm::StringRef getLLVMContractFuncName() const {
    return LLVMContractFuncName;
  }
  llvm::StringRef getLLVMCtorFuncName() const { return LLVMCtorFuncName; }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class InheritanceSpecifier {
  llvm::StringRef BaseName;
  std::vector<ExprPtr> Arguments;
  ContractDecl *BaseCont;

public:
  InheritanceSpecifier(llvm::StringRef baseName,
                       std::vector<ExprPtr> &&arguments)
      : BaseName(baseName), Arguments(std::move(arguments)), BaseCont(nullptr) {
  }

  llvm::StringRef getBaseName() const { return BaseName; }

  void setBaseCont(ContractDecl *Cont) { BaseCont = Cont; }
  ContractDecl *getBaseCont() { return BaseCont; }
  const ContractDecl *getBaseCont() const { return BaseCont; }
};

class OverrideSpecifier {
  std::vector<IdentifierPath> Overrides;

public:
  OverrideSpecifier() {}
};

class CallableVarDecl : public Decl {
  std::unique_ptr<ParamList> Params;
  std::unique_ptr<ParamList> ReturnParams;

protected:
  bool IsVirtual;
  std::unique_ptr<OverrideSpecifier> Overrides;

public:
  CallableVarDecl(SourceRange L, llvm::StringRef Name, Visibility V,
                  std::unique_ptr<ParamList> &&Params,
                  std::unique_ptr<ParamList> &&ReturnParams = nullptr,
                  bool IsVirtual = false,
                  std::unique_ptr<OverrideSpecifier> &&Overrides = nullptr)
      : Decl(L, Name, V), Params(std::move(Params)),
        ReturnParams(std::move(ReturnParams)), IsVirtual(IsVirtual),
        Overrides(std::move(Overrides)) {}

  ParamList *getParams() { return Params.get(); }
  const ParamList *getParams() const { return Params.get(); }

  ParamList *getReturnParams() { return ReturnParams.get(); }
  const ParamList *getReturnParams() const { return ReturnParams.get(); }

  OverrideSpecifier *getOverrideSpecifier() { return Overrides.get(); }
  const OverrideSpecifier *getOverrideSpecifier() const {
    return Overrides.get();
  }

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
               std::unique_ptr<Block> &&Body, bool IsVirtual,
               std::unique_ptr<OverrideSpecifier> &&Overrides);

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }
  void setBody(std::unique_ptr<Block> &&B) { Body = std::move(B); }
  TypePtr &getType() { return FuncTy; }
  const TypePtr &getType() const { return FuncTy; }
  StateMutability getStateMutability() const { return SM; }
  bool isConstructor() const { return IsConstructor; }
  bool isFallback() const { return IsFallback; }
  bool isIncomplete() const { return Body.get() == nullptr; }
  // TODO: interface is virtual default.
  // however AST do not have a link to current ContDecl
  virtual bool isVirtual() const {
    return IsVirtual; /* || is Interface Cont */
  }
  // XXX: function for hotfix above
  void markVirtual() { IsVirtual = true; }
  FunctionDecl const *resolveVirtual(const ContractDecl &MostDerivedContract,
                                     const ContractDecl *SearchStart);

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
  Token Tok;
  TypePtr Ty;
  TypePtr ConstructorTy;

public:
  StructDecl(Token NameTok, SourceRange L, llvm::StringRef Name,
             std::vector<TypePtr> &&ET, std::vector<std::string> &&EN);

  void accept(DeclVisitor &Visitor) override;
  void accept(ConstDeclVisitor &Visitor) const override;

  Token getToken() const { return Tok; }
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
