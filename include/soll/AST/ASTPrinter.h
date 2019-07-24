#pragma once

#include <ostream>

#include "soll/AST/DeclVisitor.h"
#include "soll/AST/StmtVisitor.h"

namespace soll {

class ASTPrinter : public ConstDeclVisitor, public ConstStmtVisitor {
public:
  ASTPrinter(std::ostream &os) : OS(os), Level(0) {}
  virtual ~ASTPrinter() noexcept {}

  void visit(SourceUnitType &) override;
  void visit(ContractDeclType &) override;
  void visit(FunctionDeclType &) override;
  void visit(ParamListType &) override;
  void visit(VarDeclType &) override;

  void visit(BlockType &) override;
  void visit(UnaryOperatorType &) override;
  void visit(BinaryOperatorType &) override;
  void visit(IdentifierType &) override;
  void visit(BooleanLiteralType &) override;
  void visit(StringLiteralType &) override;
  void visit(NumberLiteralType &) override;

  std::ostream& os() { return OS; }

protected:
  std::string indent() { return std::string(Level++ << 1, ' '); }
  void unindent() { --Level; }

private:
  std::ostream& OS;
  int Level;
};

}
