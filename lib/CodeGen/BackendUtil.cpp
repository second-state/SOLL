#include "soll/CodeGen/BackendUtil.h"
#include <llvm/ADT/Triple.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>

namespace soll {

void EmitBackendOutput(DiagnosticsEngine &Diags, const llvm::DataLayout &TDesc,
                       llvm::Module *TheModule,
                       std::unique_ptr<llvm::raw_pwrite_stream> OS) {
  std::unique_ptr<llvm::Module> EmptyModule;

  std::unique_ptr<llvm::TargetMachine> TM;
  {
    std::string Error;
    std::string Triple = TheModule->getTargetTriple();
    const llvm::Target *TheTarget =
        llvm::TargetRegistry::lookupTarget(Triple, Error);
    assert(TheTarget && "unable to create target");

    llvm::TargetOptions Options;
    llvm::Reloc::Model RM;
    TM.reset(TheTarget->createTargetMachine(Triple, "", "", Options, RM,
                                            llvm::None,
                                            llvm::CodeGenOpt::Level::Default));
  }
  if (!TM) {
    return;
  }
  TheModule->setDataLayout(TM->createDataLayout());

  llvm::PassBuilder PB(TM.get(), llvm::None);

  llvm::LoopAnalysisManager LAM(false);
  llvm::FunctionAnalysisManager FAM(false);
  llvm::CGSCCAnalysisManager CGAM(false);
  llvm::ModuleAnalysisManager MAM(false);

  FAM.registerPass([&] { return PB.buildDefaultAAPipeline(); });

  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  llvm::ModulePassManager MPM(false);

  MPM.addPass(llvm::AlwaysInlinerPass());

  MPM.addPass(llvm::PrintModulePass(*OS, "", false));

  MPM.run(*TheModule, MAM);

  std::string DLDesc = TheModule->getDataLayout().getStringRepresentation();
  assert(DLDesc == TDesc.getStringRepresentation() &&
         "backend data layout does not match");
}

void EmbedBitcode(llvm::Module *M, llvm::MemoryBufferRef Buf) {}
} // namespace soll
