#pragma once

#include <ostream>

#include "soll/AST/StmtVisitor.h"

namespace soll {

class ASTPrinter : public ConstStmtVisitor {
public:
  ASTPrinter(std::ostream &os) : OS(os), Level(0) {}

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
