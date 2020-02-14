// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/Frontend/CompilerInstance.h"
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/Support/ToolOutputFile.h>

namespace soll {

class BackendConsumer;
class SollDiagnosticHandler final : public llvm::DiagnosticHandler {
  BackendConsumer *BackendCon;

public:
  explicit SollDiagnosticHandler(BackendConsumer *BCon) : BackendCon(BCon) {}

  inline bool handleDiagnostics(const llvm::DiagnosticInfo &DI) override;

  bool isAnalysisRemarkEnabled(llvm::StringRef) const override { return false; }
  bool isMissedOptRemarkEnabled(llvm::StringRef) const override {
    return false;
  }
  bool isPassedOptRemarkEnabled(llvm::StringRef) const override {
    return false;
  }
  bool isAnyRemarkEnabled() const override { return false; }
};

class BackendConsumer : public ASTConsumer {
  BackendAction Action;
  DiagnosticsEngine &Diags;
  const TargetOptions &TargetOpts;
  std::unique_ptr<llvm::raw_pwrite_stream> AsmOutStream;
  ASTContext *Context;

  std::unique_ptr<CodeGenerator> Gen;

public:
  BackendConsumer(BackendAction Action, DiagnosticsEngine &Diags,
                  TargetOptions &TargetOpts, const std::string &InFile,
                  std::unique_ptr<llvm::raw_pwrite_stream> OS,
                  llvm::LLVMContext &C)
      : Action(Action), Diags(Diags), TargetOpts(TargetOpts),
        AsmOutStream(std::move(OS)), Context(nullptr),
        Gen(CreateLLVMCodeGen(Diags, InFile, C, TargetOpts)) {}
  llvm::Module *getModule() const { return Gen->getModule(); }

  CodeGenerator *getCodeGenerator() { return Gen.get(); }

  void Initialize(ASTContext &Ctx) override {
    assert(!Context && "initialized multiple times");
    Context = &Ctx;

    Gen->Initialize(Ctx);
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    Gen->HandleSourceUnit(C, S);

    // Silently ignore if we weren't initialized for some reason.
    if (!getModule()) {
      return;
    }

    EmitBackendOutput(Diags, TargetOpts, getModule()->getDataLayout(),
                      getModule(), Action, std::move(AsmOutStream));
  }
};

bool SollDiagnosticHandler::handleDiagnostics(const llvm::DiagnosticInfo &DI) {
  // BackendCon->DiagnosticHandlerImpl(DI);
  return true;
}

CodeGenAction::CodeGenAction(BackendAction Action, llvm::LLVMContext *VMContext)
    : Action(Action),
      OwnedVMContext(VMContext ? nullptr
                               : std::make_unique<llvm::LLVMContext>()),
      VMContext(VMContext ? VMContext : OwnedVMContext.get()) {}

static std::unique_ptr<llvm::raw_pwrite_stream>
GetOutputStream(CompilerInstance &CI, llvm::StringRef InFile,
                BackendAction Action) {
  switch (Action) {
  case BackendAction::EmitAssembly:
    return CI.createDefaultOutputFile(false, InFile, "s");
  case BackendAction::EmitLL:
    return CI.createDefaultOutputFile(false, InFile, "ll");
  case BackendAction::EmitBC:
    return CI.createDefaultOutputFile(true, InFile, "bc");
  case BackendAction::EmitNothing:
    return nullptr;
  case BackendAction::EmitMCNull:
    return CI.createNullOutputFile();
  case BackendAction::EmitObj:
    return CI.createDefaultOutputFile(true, InFile, "o");
  }

  llvm_unreachable("Invalid action!");
}

std::unique_ptr<ASTConsumer>
CodeGenAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) {
  std::unique_ptr<llvm::raw_pwrite_stream> OS =
      GetOutputStream(CI, InFile, Action);

  if (Action != BackendAction::EmitNothing && !OS) {
    return nullptr;
  }

  return std::make_unique<BackendConsumer>(Action, CI.getDiagnostics(),
                                           CI.getTargetOpts(), InFile,
                                           std::move(OS), *VMContext);
}

EmitAssemblyAction::EmitAssemblyAction(llvm::LLVMContext *VMContext)
    : CodeGenAction(BackendAction::EmitAssembly, VMContext) {}

EmitBCAction::EmitBCAction(llvm::LLVMContext *VMContext)
    : CodeGenAction(BackendAction::EmitBC, VMContext) {}

EmitLLVMAction::EmitLLVMAction(llvm::LLVMContext *VMContext)
    : CodeGenAction(BackendAction::EmitLL, VMContext) {}

EmitLLVMOnlyAction::EmitLLVMOnlyAction(llvm::LLVMContext *VMContext)
    : CodeGenAction(BackendAction::EmitNothing, VMContext) {}

EmitCodeGenOnlyAction::EmitCodeGenOnlyAction(llvm::LLVMContext *_VMContext)
    : CodeGenAction(BackendAction::EmitMCNull, _VMContext) {}

EmitObjAction::EmitObjAction(llvm::LLVMContext *_VMContext)
    : CodeGenAction(BackendAction::EmitObj, _VMContext) {}

} // namespace soll
