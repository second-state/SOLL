// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Decl.h"
#include "soll/AST/DeclAsm.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/DeclYul.h"

namespace soll {

void SourceUnit::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void SourceUnit::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void PragmaDirective::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void PragmaDirective::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void UsingFor::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void UsingFor::accept(ConstDeclVisitor &visitor) const { visitor.visit(*this); }

void ContractDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void ContractDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void FunctionDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void FunctionDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void EventDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void EventDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void ParamList::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void ParamList::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void CallableVarDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void CallableVarDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void VarDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void VarDecl::accept(ConstDeclVisitor &visitor) const { visitor.visit(*this); }

void StructDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void StructDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void YulCode::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void YulCode::accept(ConstDeclVisitor &visitor) const { visitor.visit(*this); }

void YulData::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void YulData::accept(ConstDeclVisitor &visitor) const { visitor.visit(*this); }

void YulObject::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void YulObject::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmFunctionDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void AsmFunctionDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

void AsmVarDecl::accept(DeclVisitor &visitor) { visitor.visit(*this); }
void AsmVarDecl::accept(ConstDeclVisitor &visitor) const {
  visitor.visit(*this);
}

} // namespace soll
