// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <memory>

namespace soll {

class AST;
class Decl;
class InheritanceSpecifier;
class FunctionDecl;
class ModifierInvocation;
class ParamList;
class VarDeclBase;
class Stmt;
class Block;
class Expr;
class Type;

class AsmFunctionDecl;

using DeclPtr = std::unique_ptr<Decl>;
using VarDeclBasePtr = std::unique_ptr<VarDeclBase>;
using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;
using BlockPtr = std::unique_ptr<Block>;
using TypePtr = std::shared_ptr<Type>;

enum class StateMutability { Pure, View, NonPayable, Payable };

} // namespace soll
