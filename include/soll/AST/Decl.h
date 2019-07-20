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
  Visibility Vis;
  std::string Name;

protected:
  friend class ASTReader;

protected:
  Decl(llvm::StringRef Name,
       Visibility vis = Visibility::Default)
      : Name(Name.str()), Vis(vis) {}
  virtual ~Decl();

};

class PragmaDirective {
  std::vector<Token> Tokens;
  std::vector<std::string> Literals;

public:
  PragmaDirective(std::vector<Token> const &tokens,
                  std::vector<std::string> const &literals)
      : Tokens(tokens), Literals(literals) {}
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
  ContractDefinition(llvm::StringRef name,
      std::vector<std::unique_ptr<InheritanceSpecifier>> &&baseContracts,
      ContractKind kind = ContractKind::Contract)
      : Decl(name), BaseContracts(std::move(baseContracts)),
        Kind(kind) {}
  virtual ~ContractDefinition() override = default;
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

class ParameterList;
class CallableDeclaration : public Decl {
  std::unique_ptr<ParameterList> Parameters;
  std::unique_ptr<ParameterList> ReturnParameters;

public:
  CallableDeclaration(llvm::StringRef name,
      Visibility visibility, std::unique_ptr<ParameterList> &&parameters,
      std::unique_ptr<ParameterList> &&returnParameters = nullptr)
      : Decl(name, visibility), Parameters(std::move(parameters)),
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
  FunctionDefinition(llvm::StringRef name,
      Visibility visibility, StateMutability sm, bool isConstructor,
      std::unique_ptr<ParameterList> &&parameters,
      std::vector<std::unique_ptr<ModifierInvocation>> &&modifiers,
      std::unique_ptr<ParameterList> &&returnParameters,
      std::unique_ptr<Block> &&body)
      : CallableDeclaration(name, visibility, std::move(parameters),
                            std::move(returnParameters)),
        SM(sm), IsConstructor(isConstructor),
        FunctionModifiers(std::move(modifiers)), Body(std::move(body)),
        Implemented(body != nullptr) {}
};

class VariableDeclaration;
class ParameterList {
  std::vector<std::unique_ptr<VariableDeclaration>> Parameters;

public:
  ParameterList(std::vector<std::unique_ptr<VariableDeclaration>> &&parameters)
      : Parameters(std::move(parameters)) {}
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
  VariableDeclaration(llvm::StringRef type, llvm::StringRef name,
                      std::unique_ptr<Expr> &&value, Visibility visibility,
                      bool isStateVar = false, bool isIndexed = false,
                      bool isConstant = false,
                      Location referenceLocation = Location::Unspecified)
      : Decl(name, visibility), TypeName(type),
        Value(std::move(value)), IsStateVariable(isStateVar),
        IsIndexed(isIndexed), IsConstant(isConstant),
        ReferenceLocation(referenceLocation) {}
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
