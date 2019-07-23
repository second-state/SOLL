#pragma once

#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/Type.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceLocation.h"
#include <memory>
#include <vector>

namespace soll {

class ASTContext;

class Decl {
public:
  enum class Visibility { Default, Private, Internal, Public, External };
  virtual ~Decl() {}

private:
  Visibility Vis;
  std::string Name;

protected:
  friend class ASTReader;

protected:
  Decl() {}
  Decl(llvm::StringRef Name,
       Visibility vis = Visibility::Default)
      : Name(Name.str()), Vis(vis) {}
  virtual ~Decl() {}

public:
  virtual void accept(DeclVisitor &visitor) = 0;
  virtual void accept(ConstDeclVisitor &visitor) const = 0;
};

class PragmaDirective : public Decl {
  std::vector<llvm::Optional<Token>> Tokens;
  std::vector<std::string> Literals;

public:
  PragmaDirective(std::vector<llvm::Optional<Token>> const &tokens,
                  std::vector<std::string> const &literals)
      : Tokens(tokens), Literals(literals) {}
};

class InheritanceSpecifier;
class FunctionDecl;
class ContractDecl : public Decl {
public:
  enum class ContractKind { Interface, Contract, Library };

private:
  std::vector<std::unique_ptr<InheritanceSpecifier>> BaseContracts;
  std::vector<std::unique_ptr<FunctionDecl>> Functions;
  ContractKind Kind;

public:
  ContractDecl(llvm::StringRef name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      ContractKind kind = ContractKind::Contract)
      : Decl(name), BaseContracts(std::move(baseContracts)),
        Kind(kind) {}
  virtual ~ContractDecl() override = default;
};

class InheritanceSpecifier {
  std::string BaseName;
  std::vector<std::unique_ptr<Expr>> Arguments;

public:
  InheritanceSpecifier(llvm::StringRef baseName,
                       std::vector<std::unique_ptr<Expr>> &&arguments)
      : BaseName(baseName.str()),
        Arguments(std::move(arguments)) {}
};

class ParamList;
class CallableVarDecl : public Decl {
  std::unique_ptr<ParamList> Params;
  std::unique_ptr<ParamList> ReturnParams;

public:
  CallableVarDecl(llvm::StringRef name,
      Visibility visibility, std::unique_ptr<ParamList> &&Params,
      std::unique_ptr<ParamList> &&returnParams = nullptr)
      : Decl(name, visibility), Params(std::move(Params)),
        ReturnParams(std::move(returnParams)) {}
};

enum class StateMutability { Pure, View, NonPayable, Payable };
class ModifierInvocation;
class Block;

class FunctionDecl : public CallableVarDecl {
  StateMutability SM;
  bool IsConstructor;
  std::vector<std::unique_ptr<ModifierInvocation>> FunctionModifiers;
  std::unique_ptr<Block> Body;
  bool Implemented;

public:
  FunctionDecl(llvm::StringRef name,
      Visibility visibility, StateMutability sm, bool isConstructor,
      std::unique_ptr<ParamList> &&Params,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParamList> &&returnParams,
      std::unique_ptr<Block> &&body)
      : CallableVarDecl(name, visibility, std::move(Params),
                        std::move(returnParams)),
        SM(sm), IsConstructor(isConstructor),
        FunctionModifiers(std::move(modifiers)), Body(std::move(body)),
        Implemented(body != nullptr) {}

  Block *getBody() const { return Body.get(); }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class VarDecl;
class ParamList {
  std::vector<std::unique_ptr<VarDecl>> Params;

public:
  ParamList(std::vector<std::unique_ptr<VarDecl>> &&Params)
      : Params(std::move(Params)) {}
};

class VarDecl : public Decl {
public:
  enum class Location { Unspecified, Storage, Memory, CallData };

private:
  std::unique_ptr<Type> TypeName;
  std::unique_ptr<Expr> Value;
  bool IsStateVariable;
  bool IsIndexed;
  bool IsConstant;
  Location ReferenceLocation;

public:
  VarDecl(llvm::StringRef type, llvm::StringRef name,
          std::unique_ptr<Expr> &&value,
          Visibility visibility = Visibility::Default, bool isStateVar = false,
          bool isIndexed = false, bool isConstant = false,
          Location referenceLocation = Location::Unspecified)
      : Decl(name, visibility), TypeName(type), Value(std::move(value)),
        IsStateVariable(isStateVar), IsIndexed(isIndexed),
        IsConstant(isConstant), ReferenceLocation(referenceLocation) {}

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class ModifierInvocation {
  std::string ModifierName;
  std::vector<std::unique_ptr<Expr>> Arguments;

public:
  ModifierInvocation(llvm::StringRef name,
                     std::vector<std::unique_ptr<Expr>> arguments)
      : ModifierName(name), Arguments(std::move(arguments)) {}
};

} // namespace soll
