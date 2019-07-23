#include "soll/AST/Decl.h"
#include "soll/AST/DeclVisitor.h"

using namespace soll;

void FunctionDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void FunctionDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void VarDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void VarDecl::accept(ConstDeclVisitor &visitor) const { visitor.visit(*this); }
