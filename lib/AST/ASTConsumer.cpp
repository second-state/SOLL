#include "soll/AST/ASTConsumer.h"

namespace soll {

class DeclGroupRef {};

bool ASTConsumer::HandleTopLevelDecl(DeclGroupRef D) { return true; }

void ASTConsumer::HandleInterestingDecl(DeclGroupRef D) {
  HandleTopLevelDecl(D);
}

} // namespace soll
