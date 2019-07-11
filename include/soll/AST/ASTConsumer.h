#pragma once

namespace soll {

class ASTContext;
class SourceUnit;

class ASTConsumer {
public:
  virtual ~ASTConsumer() {}
  virtual void Initialize(ASTContext &Context) {}
  virtual void HandleSourceUnit(ASTContext &Ctx, SourceUnit &S) {}
  virtual void PrintStats() {}
};

} // namespace soll
