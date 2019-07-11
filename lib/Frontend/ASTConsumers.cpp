#include "soll/Frontend/ASTConsumers.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/AST/RecursiveASTVisitor.h"

namespace {
using namespace soll;
class ASTPrinter : public ASTConsumer, public RecursiveASTVisitor<ASTPrinter> {
  typedef RecursiveASTVisitor<ASTPrinter> base;

public:
  enum Kind { DumpFull, Dump, Print, None };
  ASTPrinter(std::unique_ptr<llvm::raw_ostream> Out, Kind K,
             llvm::StringRef FilterString, bool DumpLookups = false)
      : Out(Out ? *Out : llvm::outs()), OwnedOut(std::move(Out)), OutputKind(K),
        FilterString(FilterString), DumpLookups(DumpLookups) {}

  void HandleTranslationUnit(ASTContext &Context) override {
  }

private:
  llvm::raw_ostream &Out;
  std::unique_ptr<llvm::raw_ostream> OwnedOut;
  Kind OutputKind;
  std::string FilterString;
  bool DumpLookups;
};

} // end anonymous namespace

namespace soll {

std::unique_ptr<ASTConsumer>
CreateASTPrinter(std::unique_ptr<llvm::raw_ostream> Out,
                 llvm::StringRef FilterString) {
  return std::make_unique<ASTPrinter>(std::move(Out), ASTPrinter::Print,
                                      FilterString);
}

} // namespace soll
