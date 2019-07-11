#pragma once

namespace soll {
class Decl;

template <typename Derived> class RecursiveASTVisitor {
public:
  bool TraverseDecl(Decl *D);
};

template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseDecl(Decl *D) {
  // TODO: implement
  return true;
}

} // namespace soll
