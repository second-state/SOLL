// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/BackendUtil.h"
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
  DiagnosticsEngine &Diags;
  const TargetOptions &TargetOpts;
  std::unique_ptr<llvm::raw_pwrite_stream> AsmOutStream;
  ASTContext *Context;

  std::unique_ptr<CodeGenerator> Gen;

public:
  BackendConsumer(DiagnosticsEngine &Diags, TargetOptions &TargetOpts,
                  const std::string &InFile,
                  std::unique_ptr<llvm::raw_pwrite_stream> OS,
                  llvm::LLVMContext &C)
      : Diags(Diags), TargetOpts(TargetOpts), AsmOutStream(std::move(OS)),
        Context(nullptr), Gen(CreateLLVMCodeGen(Diags, InFile, C, TargetOpts)) {
  }
  llvm::Module *getModule() const { return Gen->getModule(); }

  CodeGenerator *getCodeGenerator() { return Gen.get(); }

  void Initialize(ASTContext &Ctx) override {
    assert(!Context && "initialized multiple times");
    Context = &Ctx;

    Gen->Initialize(Ctx);
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    Gen->HandleSourceUnit(C, S);

    assert(getModule() && "module == nullptr");

    EmbedBitcode(getModule(), llvm::MemoryBufferRef());
    EmitBackendOutput(Diags, TargetOpts, getModule()->getDataLayout(),
                      getModule(), std::move(AsmOutStream));
  }
};

bool SollDiagnosticHandler::handleDiagnostics(const llvm::DiagnosticInfo &DI) {
  // BackendCon->DiagnosticHandlerImpl(DI);
  return true;
}

CodeGenAction::CodeGenAction()
    : OwnedVMContext(std::make_unique<llvm::LLVMContext>()),
      VMContext(OwnedVMContext.get()) {}

CodeGenAction::CodeGenAction(llvm::LLVMContext *VMContext)
    : OwnedVMContext(), VMContext(VMContext) {}

std::unique_ptr<ASTConsumer>
CodeGenAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) {
  std::unique_ptr<llvm::raw_pwrite_stream> OS =
      CI.createDefaultOutputFile(false, InFile, "ll");

  return std::make_unique<BackendConsumer>(CI.getDiagnostics(),
                                           CI.getTargetOpts(), InFile,
                                           std::move(OS), *VMContext);
}

EmitLLVMAction::EmitLLVMAction() : CodeGenAction() {}

EmitLLVMAction::EmitLLVMAction(llvm::LLVMContext *VMContext)
    : CodeGenAction(VMContext) {}

} // namespace soll
