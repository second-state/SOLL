// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/ASTConsumers.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/Decl.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"
#include "soll/AST/Type.h"
#include <llvm/ADT/Twine.h>
#include <llvm/Support/raw_ostream.h>

namespace {

std::string ToString(soll::UnaryOperatorKind op) {
  switch (op) {
  case soll::UO_Plus:
    return "+";
  case soll::UO_Minus:
    return "-";
  case soll::UO_Not:
    return "~";
  case soll::UO_LNot:
    return "!";
  case soll::UO_PreInc:
    return "(prefix) ++";
  case soll::UO_PostInc:
    return "(postfix) ++";
  case soll::UO_PreDec:
    return "(prefix) --";
  case soll::UO_PostDec:
    return "(postfix) --";
  default:
    return "(unknown unary op)";
  }
}

std::string ToString(soll::BinaryOperatorKind op) {
  switch (op) {
  case soll::BO_Exp:
    return "**";
  case soll::BO_Mul:
    return "*";
  case soll::BO_Div:
    return "/";
  case soll::BO_Rem:
    return "%";
  case soll::BO_Add:
    return "+";
  case soll::BO_Sub:
    return "-";
  case soll::BO_Shl:
    return "<<";
  case soll::BO_Shr:
    return ">>";
  case soll::BO_And:
    return "&";
  case soll::BO_Xor:
    return "^";
  case soll::BO_Or:
    return "|";
  case soll::BO_LT:
    return "<";
  case soll::BO_GT:
    return ">";
  case soll::BO_LE:
    return "<=";
  case soll::BO_GE:
    return ">=";
  case soll::BO_EQ:
    return "==";
  case soll::BO_NE:
    return "!=";
  case soll::BO_LAnd:
    return "&&";
  case soll::BO_LOr:
    return "||";
  case soll::BO_Assign:
    return "=";
  case soll::BO_MulAssign:
    return "*=";
  case soll::BO_DivAssign:
    return "/=";
  case soll::BO_RemAssign:
    return "%=";
  case soll::BO_AddAssign:
    return "+=";
  case soll::BO_SubAssign:
    return "-=";
  case soll::BO_ShlAssign:
    return "<<=";
  case soll::BO_ShrAssign:
    return ">>=";
  case soll::BO_AndAssign:
    return "&=";
  case soll::BO_XorAssign:
    return "^=";
  case soll::BO_OrAssign:
    return "|=";
  case soll::BO_Comma:
    return ",";
  default:
    return "(unknown binary op)";
  }
}

std::string ToString(const soll::Type *type) {
  if (nullptr == type)
    return "(no type)";
  soll::Type::Category c = type->getCategory();
  switch (c) {
  case soll::Type::Category::Address:
    return "address";
  case soll::Type::Category::Integer: {
    unsigned int kind = static_cast<unsigned int>(
        static_cast<const soll::IntegerType *>(type)->getKind());
    bool isUnsigned = kind < 32;
    unsigned int bits = ((isUnsigned ? kind : (kind - 32)) + 1) << 3;
    return (llvm::Twine(isUnsigned ? "u" : "") + "int" + llvm::Twine(bits))
        .str();
  }
  case soll::Type::Category::String:
    return "string";
  case soll::Type::Category::Bool:
    return "bool";
  case soll::Type::Category::Array: {
    auto at = static_cast<const soll::ArrayType *>(type);
    return llvm::Twine(
               ToString(at->getElementType()) + "[" +
               (at->isDynamicSized() ? "" : llvm::Twine(at->getLength())) + "]")
        .str();
  }
  case soll::Type::Category::Mapping: {
    auto mt = static_cast<const soll::MappingType *>(type);
    return (llvm::Twine("mapping(") + ToString(mt->getKeyType()) + " => " +
            ToString(mt->getValueType()) + ")")
        .str();
  }
  default:
    return "(unknown type)";
  }
  return "";
}

} // namespace

namespace soll {

class ASTPrinter : public ASTConsumer,
                   public ConstDeclVisitor,
                   public ConstStmtVisitor {

  llvm::raw_ostream &Out;
  int Level = 0;

public:
  ASTPrinter(llvm::raw_ostream &Out = llvm::outs()) : Out(Out) {}

  void HandleSourceUnit(ASTContext &Ctx, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(SourceUnitType &) override;
  void visit(ContractDeclType &) override;
  void visit(FunctionDeclType &) override;
  void visit(ParamListType &) override;
  void visit(VarDeclType &) override;
  void visit(ModifierInvocationType &) override;

  void visit(BlockType &) override;
  void visit(IfStmtType &) override;
  void visit(ForStmtType &) override;
  void visit(ContinueStmtType &) override;
  void visit(BreakStmtType &) override;
  void visit(ReturnStmtType &) override;
  void visit(DeclStmtType &) override;
  void visit(UnaryOperatorType &) override;
  void visit(BinaryOperatorType &) override;
  void visit(CallExprType &) override;
  void visit(ParenExprType &) override;
  void visit(IdentifierType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;
  void visit(ImplicitCastExprType &) override;
  void visit(ExplicitCastExprType &) override;
  llvm::raw_ostream &os() { return Out; }

protected:
  std::string indent() { return std::string(Level++ << 1, ' '); }
  void unindent() { --Level; }
};

std::unique_ptr<ASTConsumer> CreateASTPrinter(llvm::raw_ostream &Out) {
  return std::make_unique<ASTPrinter>(Out);
}

void ASTPrinter::visit(SourceUnitType &decl) {
  os() << indent() << "SourceUnit\n";
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ContractDeclType &decl) {
  os() << indent() << "ContractDecl \"" << decl.getName() << "\"\n";
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(FunctionDeclType &decl) {
  os() << indent() << "FunctionDecl \"" << decl.getName() << "\"\n";
  ConstDeclVisitor::visit(decl);
  decl.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(ParamListType &param) {
  os() << indent() << "ParamList\n";
  ConstDeclVisitor::visit(param);
  unindent();
}

void ASTPrinter::visit(VarDeclType &decl) {
  os() << indent() << "VarDecl \"" << decl.getName() << "\""
       << ", " << ToString(decl.GetType()) << "\n";
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ModifierInvocationType &modifier) {
  os() << indent() << "ModifierInvocation \"" << modifier.getName() << "\"\n";
  ConstDeclVisitor::visit(modifier);
  unindent();
}

void ASTPrinter::visit(BlockType &block) {
  os() << indent() << "Block\n";
  ConstStmtVisitor::visit(block);
  unindent();
}

void ASTPrinter::visit(IfStmtType &stmt) {
  os() << indent() << "IfStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ForStmtType &stmt) {
  os() << indent() << "ForStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ContinueStmtType &stmt) {
  os() << indent() << "ContinueStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(BreakStmtType &stmt) {
  os() << indent() << "BreakStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(ReturnStmtType &stmt) {
  os() << indent() << "ReturnStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(DeclStmtType &decstmt) {
  os() << indent() << "DeclStmt\n";
  ConstStmtVisitor::visit(decstmt);
  for (auto vdecl : decstmt.getVarDecls())
    vdecl->accept(*this);
  // [TODO] Debug use, this field should be removed in the furture.
  // Value will store in ValDecl node.
  if (decstmt.getValue() != nullptr)
    decstmt.getValue()->accept(*this);
  unindent();
}

void ASTPrinter::visit(UnaryOperatorType &op) {
  os() << indent() << "UnaryOperator \"" << ToString(op.getOpcode()) << "\"\n";
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(BinaryOperatorType &op) {
  os() << indent() << "BinaryOperator \"" << ToString(op.getOpcode()) << "\"\n";
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(CallExprType &call) {
  os() << indent() << "CallExpr\n";
  ConstStmtVisitor::visit(call);
  unindent();
}

void ASTPrinter::visit(ParenExprType &paren) {
  os() << indent() << "ParenExpr\n";
  ConstStmtVisitor::visit(paren);
  unindent();
}

void ASTPrinter::visit(IdentifierType &id) {
  os() << indent() << "Identifier \"" << id.getName() << "\"\n";
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(BooleanLiteralType &literal) {
  os() << indent() << "BooleanLiteral \"" << literal.getValue() << "\"\n";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(StringLiteralType &literal) {
  os() << indent() << "StringLiteral \"" << literal.getValue() << "\"\n";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(NumberLiteralType &literal) {
  os() << indent() << "NumberLiteral \"" << literal.getValue() << "\"\n";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(ImplicitCastExprType &IC) {
  os() << indent() << "ImplicitCastExpr\n";
  ConstStmtVisitor::visit(IC);
  unindent();
}

void ASTPrinter::visit(ExplicitCastExprType &EC) {
  os() << indent() << "ExplicitCastExpr\n";
  ConstStmtVisitor::visit(EC);
  unindent();
}

} // namespace soll
