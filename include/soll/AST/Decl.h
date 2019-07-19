#pragma once
#include "soll/AST/Expr.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceLocation.h"
#include <memory>
#include <vector>

namespace soll {

class ASTContext;
class Token;

class Decl {
public:
  enum class Visibility { Default, Private, Internal, Public, External };

private:
  SourceLocation Loc;
  Visibility Vis;
  std::string Name;

protected:
  friend class ASTReader;

protected:
  Decl(SourceLocation L, llvm::StringRef Name,
       Visibility vis = Visibility::Default)
      : Loc(L), Name(Name.str()), Vis(vis) {}
  virtual ~Decl();

public:
  virtual SourceRange getSourceRange() const LLVM_READONLY {
    return SourceRange(getLocation(), getLocation());
  }

  SourceLocation getBeginLoc() const LLVM_READONLY {
    return getSourceRange().getBegin();
  }

  SourceLocation getEndLoc() const LLVM_READONLY {
    return getSourceRange().getEnd();
  }

  SourceLocation getLocation() const { return Loc; }
  void setLocation(SourceLocation L) { Loc = L; }
};

class PragmaDirective {
  SourceLocation Loc;
  std::vector<Token> Tokens;
  std::vector<std::string> Literals;

public:
  PragmaDirective(SourceLocation const &location,
                  std::vector<Token> const &tokens,
                  std::vector<std::string> const &literals)
      : Loc(location), Tokens(tokens), Literals(literals) {}
};

class InheritanceSpecifier;
class FunctionDefinition;
class ContractDefinition : public Decl {
public:
  enum class ContractKind { Interface, Contract, Library };

private:
  std::vector<std::unique_ptr<InheritanceSpecifier>> BaseContracts;
  std::vector<std::unique_ptr<FunctionDefinition>> Functions;
  ContractKind Kind;

public:
  ContractDefinition(
      const SourceLocation &location, llvm::StringRef name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      ContractKind kind = ContractKind::Contract)
      : Decl(location, name), BaseContracts(std::move(baseContracts)),
        Kind(kind) {}
  virtual ~ContractDefinition() override = default;
};

class InheritanceSpecifier {
  SourceLocation Loc;
  std::string BaseName;
  std::vector<std::unique_ptr<Expr>> Arguments;

public:
  InheritanceSpecifier(const SourceLocation &location, llvm::StringRef baseName,
                       std::vector<std::unique_ptr<Expr>> &&arguments)
      : Loc(location), BaseName(baseName.str()),
        Arguments(std::move(arguments)) {}
};

class ParameterList;
class CallableDeclaration : public Decl {
  std::unique_ptr<ParameterList> Parameters;
  std::unique_ptr<ParameterList> ReturnParameters;

public:
  CallableDeclaration(
      const SourceLocation &location, llvm::StringRef name,
      Visibility visibility, std::unique_ptr<ParameterList> &&parameters,
      std::unique_ptr<ParameterList> &&returnParameters = nullptr)
      : Decl(location, name, visibility), Parameters(std::move(parameters)),
        ReturnParameters(std::move(returnParameters)) {}
};

enum class StateMutability { Pure, View, NonPayable, Payable };
class ModifierInvocation;
class Block;

class FunctionDefinition : public CallableDeclaration {
  StateMutability SM;
  bool IsConstructor;
  std::vector<std::unique_ptr<ModifierInvocation>> FunctionModifiers;
  std::unique_ptr<Block> Body;
  bool Implemented;

public:
  FunctionDefinition(
      const SourceLocation &location, llvm::StringRef name,
      Visibility visibility, StateMutability sm, bool isConstructor,
      std::unique_ptr<ParameterList> &&parameters,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParameterList> &&returnParameters,
      std::unique_ptr<Block> &&body)
      : CallableDeclaration(location, name, visibility, std::move(parameters),
                            std::move(returnParameters)),
        SM(sm), IsConstructor(isConstructor),
        FunctionModifiers(std::move(modifiers)), Body(std::move(body)),
        Implemented(body != nullptr) {}
};

class VariableDeclaration;
class ParameterList {
  SourceLocation Loc;
  std::vector<std::unique_ptr<VariableDeclaration>> Parameters;

public:
  ParameterList(const SourceLocation &location,
                std::vector<std::unique_ptr<VariableDeclaration>> &&parameters)
      : Loc(location), Parameters(std::move(parameters)) {}
};

class VariableDeclaration : public Decl {
public:
  enum class Location { Unspecified, Storage, Memory, CallData };

private:
  std::string TypeName;
  std::unique_ptr<Expr> Value;
  bool IsStateVariable;
  bool IsIndexed;
  bool IsConstant;
  Location ReferenceLocation;

public:
  VariableDeclaration(const SourceLocation &sourceLocation,
                      llvm::StringRef type, llvm::StringRef name,
                      std::unique_ptr<Expr> &&value, Visibility visibility,
                      bool isStateVar = false, bool isIndexed = false,
                      bool isConstant = false,
                      Location referenceLocation = Location::Unspecified)
      : Decl(sourceLocation, name, visibility), TypeName(type),
        Value(std::move(value)), IsStateVariable(isStateVar),
        IsIndexed(isIndexed), IsConstant(isConstant),
        ReferenceLocation(referenceLocation) {}
};

class ModifierInvocation {
  SourceLocation Loc;
  std::string ModifierName;
  std::vector<std::unique_ptr<Expr>> Arguments;

public:
  ModifierInvocation(const SourceLocation &location, llvm::StringRef name,
                     std::vector<std::unique_ptr<Expr>> arguments)
      : Loc(location), ModifierName(name), Arguments(std::move(arguments)) {}
};

} // namespace soll
