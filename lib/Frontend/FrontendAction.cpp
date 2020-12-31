// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/FrontendAction.h"
#include "soll/Frontend/CompilerInstance.h"
#include "soll/Parse/ParseAST.h"

namespace soll {

FrontendAction::FrontendAction() {}
FrontendAction::~FrontendAction() {}

void FrontendAction::setCurrentInput(const FrontendInputFile &CurrentInput) {
  this->CurrentInput = CurrentInput;
}

llvm::Module *FrontendAction::getCurrentModule() const {
  // return getCompilerInstance().getCurrentModule();
  return nullptr;
}

bool FrontendAction::BeginSourceFile(CompilerInstance &CI,
                                     const FrontendInputFile &RealInput) {
  FrontendInputFile Input(RealInput);
  assert(!Instance && "Already processing a source file!");
  assert(!Input.isEmpty() && "Unexpected empty filename!");
  setCurrentInput(Input);
  setCompilerInstance(&CI);

  bool HasBegunSourceFile = false;
  {
    if (!BeginInvocation(CI))
      goto failure;

    if (!CI.hasFileManager()) {
      if (!CI.createFileManager()) {
        goto failure;
      }
    }
    if (!CI.hasSourceManager())
      CI.createSourceManager(CI.getFileManager());

    CI.getDiagnosticClient().BeginSourceFile();
    HasBegunSourceFile = true;

    // Initialize the main file entry.
    if (!CI.InitializeSourceManager(Input))
      goto failure;

    CI.createLexer();

    // Initialize the action.
    if (!BeginSourceFileAction(CI))
      goto failure;

    CI.createASTContext();

    return true;
  }

failure:
  if (HasBegunSourceFile)
    CI.getDiagnosticClient().EndSourceFile();
  setCurrentInput(FrontendInputFile());
  setCompilerInstance(nullptr);
  return false;
}

bool FrontendAction::Execute() {
  ExecuteAction();
  return true;
}

void FrontendAction::EndSourceFile() {
  CompilerInstance &CI = getCompilerInstance();

  CI.getDiagnosticClient().EndSourceFile();

  EndSourceFileAction();

  CI.setSema(nullptr);
  CI.setASTContext(nullptr);
  CI.clearASTConsumer();

  setCompilerInstance(nullptr);
  setCurrentInput(FrontendInputFile());
}

bool FrontendAction::shouldEraseOutputFiles() {
  return getCompilerInstance().getDiagnostics().hasErrorOccurred();
}

void ASTFrontendAction::ExecuteAction() {
  CompilerInstance &CI = getCompilerInstance();

  if (!CI.hasLexer())
    return;

  if (!CI.hasSema())
    CI.createSema();

  Sema &S = CI.getSema();
  ASTContext &Ctx = CI.getASTContext();
  auto SourceUnits = ParseAST(S, Ctx.getLang(), true);
  std::string PresumedInputFile = getCurrentFileOrBufferName();

  for (auto &SU : SourceUnits) {
    std::unique_ptr<ASTConsumer> Consumer =
        CreateASTConsumer(CI, PresumedInputFile);
    assert(Consumer && "Cannot create ASTConsumer");
    CI.addASTConsumer(std::move(Consumer));
    ASTConsumer &C = CI.getASTConsumer();
    C.HandleSourceUnit(Ctx, *SU);
  }
}

} // namespace soll
