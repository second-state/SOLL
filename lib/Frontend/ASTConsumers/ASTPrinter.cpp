// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Frontend/ASTConsumers.h"
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

std::string ToString(soll::TypePtr type) {
  if (nullptr == type)
    return "(no type)";
  soll::Type::Category c = type->getCategory();
  switch (c) {
  case soll::Type::Category::Address:
    return "address";
  case soll::Type::Category::Integer: {
    unsigned int kind = static_cast<unsigned int>(
        static_cast<const soll::IntegerType *>(type.get())->getKind());
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
    auto at = static_cast<const soll::ArrayType *>(type.get());
    return llvm::Twine(
               ToString(at->getElementType()) + "[" +
               (at->isDynamicSized() ? "" : llvm::Twine(at->getLength())) + "]")
        .str();
  }
  case soll::Type::Category::Mapping: {
    auto mt = static_cast<const soll::MappingType *>(type.get());
    return (llvm::Twine("mapping(") + ToString(mt->getKeyType()) + " => " +
            ToString(mt->getValueType()) + ")")
        .str();
  }
  case soll::Type::Category::Function: {
    auto FT = static_cast<const soll::FunctionType *>(type.get());
    auto &PTys = FT->getParamTypes();
    auto &RTys = FT->getReturnTypes();

    std::string Res;
    for (size_t i = 0; i < RTys.size(); i++) {
      if (i > 0)
        Res += ',';
      Res += ToString(RTys[i]);
    }
    if (RTys.size() > 1)
      Res = "(" + Res + ")";
    Res += "(";
    for (size_t i = 0; i < PTys.size(); i++) {
      if (i > 0)
        Res += ',';
      Res += ToString(PTys[i]);
    }
    Res += ")";
    return Res;
  }
  default:
    return "(unknown type)";
  }
  return "";
}

std::string ToString(soll::CastKind CK) {
  switch (CK) {
  case soll::CastKind::IntegralCast:
    return "<IntegralCast>";
  case soll::CastKind::LValueToRValue:
    return "<LValueToRValue>";
  case soll::CastKind::TypeCast:
    return "<TypeCast>";
  }
  assert(false && "unknown cast kind!");
  __builtin_unreachable();
}

std::string ToString(soll::VarDecl::Location LOC) {
  switch (LOC) {
  case soll::VarDecl::Location::Unspecified:
    return "Unspecified";
  case soll::VarDecl::Location::Storage:
    return "Storage";
  case soll::VarDecl::Location::Memory:
    return "Memory";
  case soll::VarDecl::Location::CallData:
    return "CallData";
  default:
    __builtin_unreachable();
  }
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
  void visit(EventDeclType &) override;
  void visit(ParamListType &) override;
  void visit(VarDeclType &) override;
  void visit(ModifierInvocationType &) override;

  void visit(BlockType &) override;
  void visit(EmitStmtType &) override;
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
  void visit(MemberExprType &) override;
  void visit(IndexAccessType &) override;
  void visit(IdentifierType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;
  void visit(ImplicitCastExprType &) override;
  void visit(ExplicitCastExprType &) override;

  void visit(YulObjectType &) override;
  void visit(YulCodeType &) override;
  void visit(YulDataType &) override;

  void visit(AsmCaseStmtType &) override;
  void visit(AsmDefaultStmtType &) override;
  void visit(AsmSwitchStmtType &) override;
  void visit(AsmAssignmentStmtType &) override;
  void visit(AsmFunctionDeclStmtType &) override;
  void visit(AsmVarDeclType &) override;
  void visit(AsmIdentifierType &) override;
  void visit(AsmIdentifierListType &) override;

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
  os() << indent() << "FunctionDecl \"" << decl.getName() << "\" "
       << ToString(decl.getType()) << " \n";
  ConstDeclVisitor::visit(decl);
  decl.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(EventDeclType &decl) {
  os() << indent() << "EventDecl \"" << decl.getName() << "\" "
       << ToString(decl.getType()) << " \n";
  ConstDeclVisitor::visit(decl);
  unindent();
}

void ASTPrinter::visit(ParamListType &param) {
  os() << indent() << "ParamList\n";
  ConstDeclVisitor::visit(param);
  unindent();
}

void ASTPrinter::visit(VarDeclType &decl) {
  os() << indent() << "VarDecl "
       << "\"" << decl.getName() << "\""
       << (decl.isStateVariable() ? " (StateVar) " : " (NonStateVar) ")
       << ToString(decl.getLoc()) << ", " << ToString(decl.GetType()) << " "
       << &decl << " \n";
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

void ASTPrinter::visit(EmitStmtType &block) {
  os() << indent() << "EmitStmt\n";
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
  // TODO: Consider directly store value in ValDecl node.
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
  os() << indent() << "BinaryOperator \"" << ToString(op.getOpcode()) << "\" "
       << ToString(op.getType()) << " \n";
  ConstStmtVisitor::visit(op);
  unindent();
}

void ASTPrinter::visit(CallExprType &call) {
  os() << indent() << "CallExpr " << ToString(call.getType()) << "\n";
  ConstStmtVisitor::visit(call);
  unindent();
}

void ASTPrinter::visit(ParenExprType &paren) {
  os() << indent() << "ParenExpr\n";
  ConstStmtVisitor::visit(paren);
  unindent();
}

void ASTPrinter::visit(MemberExprType &ia) {
  os() << indent() << "MemberExpr\n";
  ConstStmtVisitor::visit(ia);
  unindent();
}

void ASTPrinter::visit(IndexAccessType &ia) {
  os() << indent() << "IndexAccess " << ToString(ia.getType()) << "\n";
  ConstStmtVisitor::visit(ia);
  unindent();
}

void ASTPrinter::visit(IdentifierType &id) {
  os() << indent() << "Identifier \"" << id.getName() << "\" "
       << ToString(id.getType());
  if (id.isSpecialIdentifier()) {
    os() << " as " << static_cast<unsigned>(id.getSpecialIdentifier());
  } else if (auto D = id.getCorrespondDecl()) {
    os() << " from " << D;
  }
  os() << "\n";
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(BooleanLiteralType &literal) {
  os() << indent() << "BooleanLiteral "
       << (literal.getValue() ? "true" : "false") << "\n ";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(StringLiteralType &literal) {
  os() << indent() << "StringLiteral \"" << literal.getValue() << "\"\n";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(NumberLiteralType &literal) {
  os() << indent() << "NumberLiteral " << literal.getValue() << "\n";
  ConstStmtVisitor::visit(literal);
  unindent();
}

void ASTPrinter::visit(ImplicitCastExprType &IC) {
  os() << indent() << "ImplicitCastExpr " << ToString(IC.getType()) << " "
       << ToString(IC.getCastKind()) << "\n";
  ConstStmtVisitor::visit(IC);
  unindent();
}

void ASTPrinter::visit(ExplicitCastExprType &EC) {
  os() << indent() << "ExplicitCastExpr " << ToString(EC.getType()) << " "
       << ToString(EC.getCastKind()) << "\n";
  ConstStmtVisitor::visit(EC);
  unindent();
}

void ASTPrinter::visit(YulObjectType &YO) {
  os() << indent() << "YulObject \"" << YO.getName() << "\"\n";
  ConstDeclVisitor::visit(YO);
  unindent();
}

void ASTPrinter::visit(YulCodeType &YC) {
  os() << indent() << "YulCode\n";
  YC.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(YulDataType &YD) {
  os() << indent() << "YulData \"" << YD.getName() << "\"\n";
  ConstDeclVisitor::visit(YD);
  YD.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(AsmCaseStmtType &stmt) {
  os() << indent() << "AsmCaseStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(AsmDefaultStmtType &stmt) {
  os() << indent() << "AsmDefaultStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(AsmSwitchStmtType &stmt) {
  os() << indent() << "AsmSwitchStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(AsmAssignmentStmtType &stmt) {
  os() << indent() << "AsmAssignmentStmt\n";
  ConstStmtVisitor::visit(stmt);
  unindent();
}

void ASTPrinter::visit(AsmFunctionDeclStmtType &stmt) {
  os() << indent() << "AsmFunctionDeclStmt \"" << stmt.getName() << "\" "
       << ToString(stmt.getType()) << " \n";
  if (stmt.getParams() != nullptr)
    stmt.getParams()->accept(*this);
  if (stmt.getReturnParams() != nullptr)
    stmt.getReturnParams()->accept(*this);
  stmt.getBody()->accept(*this);
  unindent();
}

void ASTPrinter::visit(AsmVarDeclType &decl) {
  os() << indent() << "AsmVarDecl "
       << "\"" << decl.getName() << "\" " << ToString(decl.GetType()) << " "
       << &decl << " \n";

  if (auto V = decl.GetValue()) {
    V->accept(*this);
  }
  unindent();
}

void ASTPrinter::visit(AsmIdentifierType &id) {
  os() << indent() << "AsmIdentifier \"" << id.getName() << "\" "
       << ToString(id.getType());
  if (id.isSpecialIdentifier()) {
    os() << " as " << static_cast<unsigned>(id.getSpecialIdentifier());
  } else if (auto D = id.getCorrespondDecl()) {
    os() << " from " << D;
  }
  os() << "\n";
  ConstStmtVisitor::visit(id);
  unindent();
}

void ASTPrinter::visit(AsmIdentifierListType &expr) {
  os() << indent() << "AsmIdentifierList\n";
  ConstStmtVisitor::visit(expr);
  unindent();
}

} // namespace soll
