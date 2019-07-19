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
  void visit(LiteralType &) override;

  std::ostream& os() { return OS; }

private:
  std::ostream& OS;
  int Level;
};

}
