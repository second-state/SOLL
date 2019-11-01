// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/ModuleBuilder.h"
#include "CodeGenModule.h"
#include "soll/AST/AST.h"
#include "soll/Basic/Diagnostic.h"

namespace {
using namespace soll;
class CodeGeneratorImpl : public CodeGenerator,
                          public ConstDeclVisitor,
                          public ConstStmtVisitor {
  DiagnosticsEngine &Diags;
  ASTContext *Ctx;

protected:
  std::unique_ptr<llvm::Module> M;
  std::unique_ptr<CodeGen::CodeGenModule> Builder;
  const TargetOptions &TargetOpts;
  bool isEVM() const noexcept { return TargetOpts.BackendTarget == EVM; }
  bool isEWASM() const noexcept { return TargetOpts.BackendTarget == EWASM; }

public:
  CodeGeneratorImpl(DiagnosticsEngine &diags, llvm::StringRef ModuleName,
                    llvm::LLVMContext &C, const TargetOptions &TargetOpts)
      : Diags(diags), Ctx(nullptr),
        M(std::make_unique<llvm::Module>(ModuleName, C)),
        TargetOpts(TargetOpts) {}

  CodeGen::CodeGenModule &CGM() { return *Builder; }

  llvm::Module *getModule() { return M.get(); }

  void Initialize(ASTContext &Context) override {
    Ctx = &Context;

    if (isEVM()) {
      M->setTargetTriple("evm-unknown-unknown-evm");
      M->setDataLayout(llvm::DataLayout("E-m:e-p:256:256-i:256:256-n256-S0"));
    } else if (isEWASM()) {
      // WebAssembly32TargetInfo
      M->setTargetTriple("wasm32-unknown-unknown-wasm");
      M->setDataLayout(llvm::DataLayout("e-m:e-p:32:32-i64:64-n32:64-S128"));
    }
    Builder = std::make_unique<CodeGen::CodeGenModule>(Context, *M, Diags,
                                                       TargetOpts);
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    if (Diags.hasErrorOccurred()) {
      return;
    }
    for (auto &Node : S.getNodes()) {
      if (!Node)
        continue;
      HandleTopLevelDecl(Node);
    }
  }

  void HandleTopLevelDecl(Decl *D) {
    assert(Builder.get() && "CodeGenerator need to call Initialize first!");
    if (Diags.hasErrorOccurred()) {
      return;
    }
    if (dynamic_cast<PragmaDirective *>(D)) {
    } else if (auto *CD = dynamic_cast<ContractDecl *>(D)) {
      Builder->emitContractDecl(CD);
    } else if (auto *YO = dynamic_cast<YulObject *>(D)) {
      Builder->emitYulObject(YO);
    } else {
      assert(false && "invalid top level decl");
      __builtin_unreachable();
    }
  }
};

} // namespace

namespace soll {

CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
                                 llvm::StringRef ModuleName,
                                 llvm::LLVMContext &C,
                                 const TargetOptions &TargetOpts) {
  return new CodeGeneratorImpl(Diags, ModuleName, C, TargetOpts);
}

llvm::Module *CodeGenerator::getModule() {
  return static_cast<CodeGeneratorImpl *>(this)->getModule();
}

void CodeGenerator::HandleSourceUnit(ASTContext &C, SourceUnit &S) {
  return static_cast<CodeGeneratorImpl *>(this)->HandleSourceUnit(C, S);
}

} // namespace soll
