#pragma once

namespace soll {

class ASTContext;
class DeclGroupRef;
class TagDecl;

class ASTConsumer {
public:
  virtual ~ASTConsumer() {}
  virtual void Initialize(ASTContext &Context) {}
  virtual bool HandleTopLevelDecl(DeclGroupRef D);
  virtual void HandleInterestingDecl(DeclGroupRef D);
  virtual void HandleTranslationUnit(ASTContext &Ctx) {}
  virtual void HandleTagDeclDefinition(TagDecl *D) {}
  virtual void PrintStats() {}
};

} // namespace soll
