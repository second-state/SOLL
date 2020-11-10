// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/BackendUtil.h"
#include "soll/Basic/CodeGenOptions.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticFrontend.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/LoweringInteger.h"
#include <llvm/ADT/Triple.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Bitcode/BitcodeWriterPass.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>

namespace soll {

class EmitAssemblyHelper {
  DiagnosticsEngine &Diags;
  const CodeGenOptions &CodeGenOpts;
  const TargetOptions &TargetOpts;
  llvm::Module *TheModule;
  std::unique_ptr<llvm::raw_pwrite_stream> OS;

  llvm::TargetIRAnalysis getTargetIRAnalysis() const {
    if (TM)
      return TM->getTargetIRAnalysis();

    return llvm::TargetIRAnalysis();
  }

public:
  std::unique_ptr<llvm::TargetMachine> TM;

  EmitAssemblyHelper(DiagnosticsEngine &Diags, const CodeGenOptions &CGOpts,
                     const TargetOptions &TargetOpts, llvm::Module *M)
      : Diags(Diags), CodeGenOpts(CGOpts), TargetOpts(TargetOpts),
        TheModule(M) {}

  /// Generates the TargetMachine.
  /// Leaves TM unchanged if it is unable to create the target machine.
  /// Some of our clang tests specify triples which are not built
  /// into clang. This is okay because these tests check the generated
  /// IR, and they require DataLayout which depends on the triple.
  /// In this case, we allow this method to fail and not report an error.
  /// When MustCreateTM is used, we print an error if we are unable to load
  /// the requested target.
  void CreateTargetMachine(bool MustCreateTM);

  /// Add passes necessary to emit assembly or LLVM IR.
  ///
  /// \return True on success.
  bool AddEmitPasses(llvm::legacy::PassManager &CodeGenPasses,
                     BackendAction Action, llvm::raw_pwrite_stream &OS);

  void EmitAssembly(BackendAction Action,
                    std::unique_ptr<llvm::raw_pwrite_stream> OS);
};

static llvm::CodeGenFileType getCodeGenFileType(BackendAction Action) {
  if (Action == BackendAction::EmitObj)
    return llvm::CGFT_ObjectFile;
  else if (Action == BackendAction::EmitMCNull)
    return llvm::CGFT_Null;
  else {
    assert(Action == BackendAction::EmitAssembly && "Invalid action!");
    return llvm::CGFT_AssemblyFile;
  }
}

void EmitAssemblyHelper::CreateTargetMachine(bool MustCreateTM) {
  // Create the TargetMachine for generating code.
  std::string Error;
  std::string Triple = TheModule->getTargetTriple();
  const llvm::Target *TheTarget =
      llvm::TargetRegistry::lookupTarget(Triple, Error);
  if (!TheTarget) {
    if (MustCreateTM) {
      Diags.Report(diag::err_fe_unable_to_create_target) << Error;
    }
    return;
  }

  llvm::TargetOptions Options;
  llvm::Reloc::Model RM = llvm::Reloc::Static;
  TM.reset(TheTarget->createTargetMachine(Triple, "", "", Options, RM,
                                          llvm::None,
                                          llvm::CodeGenOpt::Level::Default));
}

bool EmitAssemblyHelper::AddEmitPasses(llvm::legacy::PassManager &CodeGenPasses,
                                       BackendAction Action,
                                       llvm::raw_pwrite_stream &OS) {
  // Add LibraryInfo.
  auto TLII = std::make_unique<llvm::TargetLibraryInfoImpl>(
      llvm::Triple(TheModule->getTargetTriple()));
  TLII->disableAllFunctions();
  CodeGenPasses.add(new llvm::TargetLibraryInfoWrapperPass(*TLII));

  // Normal mode, emit a .s or .o file by running the code generator. Note,
  // this also adds codegenerator level optimization passes.
  llvm::CodeGenFileType CGFT = getCodeGenFileType(Action);

  if (TM->addPassesToEmitFile(CodeGenPasses, OS, nullptr, CGFT, false)) {
    Diags.Report(diag::err_fe_unable_to_interface_with_target);
    return false;
  }

  return true;
}

void EmitAssemblyHelper::EmitAssembly(
    BackendAction Action, std::unique_ptr<llvm::raw_pwrite_stream> OS) {
  // FIXME: We only allow generating LLVM IR for EVM backend.
  // If the EVM target machine can be created correctly and the EVM-ld
  // is implemented, then we can integrate EVM backend code generation
  // in the same way as EWASM.
  if (TargetOpts.BackendTarget == EVM) {
    if (Action == BackendAction::EmitLL) {
      TheModule->print(*OS, nullptr, false);
    } else {
      Diags.Report(diag::err_fe_only_accept_emitllvm_for_evm_target);
    }
    return;
  }

  // The following section is for EWASM backend.
  bool RequiresCodeGen =
      (Action != BackendAction::EmitNothing &&
       Action != BackendAction::EmitBC && Action != BackendAction::EmitLL);
  CreateTargetMachine(RequiresCodeGen);

  if (RequiresCodeGen && !TM) {
    return;
  }
  if (TM) {
    TheModule->setDataLayout(TM->createDataLayout());
  }

#if LLVM_VERSION_MAJOR >= 9
  llvm::PassBuilder PB(TM.get(), llvm::PipelineTuningOptions(), llvm::None);
#else
  llvm::PassBuilder PB(TM.get(), llvm::None);
#endif

  llvm::LoopAnalysisManager LAM(false);
  llvm::FunctionAnalysisManager FAM(false);
  llvm::CGSCCAnalysisManager CGAM(false);
  llvm::ModuleAnalysisManager MAM(false);

  // Register the AA manager first so that our version is the one used.
  FAM.registerPass([&] { return PB.buildDefaultAAPipeline(); });

  // Register the target library analysis directly and give it a customized
  // preset TLI.
  auto TLII = std::make_unique<llvm::TargetLibraryInfoImpl>(
      llvm::Triple(TheModule->getTargetTriple()));
  TLII->disableAllFunctions();
  FAM.registerPass([&] { return llvm::TargetLibraryAnalysis(*TLII); });
#if LLVM_VERSION_MAJOR <= 9
  MAM.registerPass([&] { return llvm::TargetLibraryAnalysis(*TLII); });
#endif

  // Register all the basic analyses with the managers.
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  llvm::ModulePassManager MPM(false);

  if (TargetOpts.BackendTarget == EWASM) {
    MPM.addPass(LoweringInteger());
  }
  switch (CodeGenOpts.OptimizationLevel) {
  case O0:
  default:
    break;
  case O1:
    MPM.addPass(PB.buildPerModuleDefaultPipeline(llvm::PassBuilder::O1));
    break;
  case O2:
    MPM.addPass(PB.buildPerModuleDefaultPipeline(llvm::PassBuilder::O2));
    break;
  case O3:
    MPM.addPass(PB.buildPerModuleDefaultPipeline(llvm::PassBuilder::O3));
    break;
  case Os:
    MPM.addPass(PB.buildPerModuleDefaultPipeline(llvm::PassBuilder::Os));
    break;
  case Oz:
    MPM.addPass(PB.buildPerModuleDefaultPipeline(llvm::PassBuilder::Oz));
    break;
  }
  MPM.addPass(llvm::AlwaysInlinerPass());
  MPM.run(*TheModule, MAM);

  // FIXME: We still use the legacy pass manager to do code generation. We
  // create that pass manager here and use it as needed below.
  llvm::legacy::PassManager CodeGenPasses;
  bool NeedCodeGen = false;

  // Append any output we need to the pass manager.
  switch (Action) {
  case BackendAction::EmitNothing:
    break;

  case BackendAction::EmitBC:
    // Emit a module summary by default for Regular LTO except for ld64
    // targets
    MPM.addPass(llvm::BitcodeWriterPass(*OS, false, false));
    break;

  case BackendAction::EmitLL:
    MPM.addPass(llvm::PrintModulePass(*OS, "", false));
    break;

  case BackendAction::EmitAssembly:
  case BackendAction::EmitMCNull:
  case BackendAction::EmitObj:
  case BackendAction::EmitWasm:
    NeedCodeGen = true;
    CodeGenPasses.add(
        llvm::createTargetTransformInfoWrapperPass(getTargetIRAnalysis()));
    if (!AddEmitPasses(CodeGenPasses, Action, *OS))
      // FIXME: Should we handle this error differently?
      return;
    break;
  }

  // Now that we have all of the passes ready, run them.
  MPM.run(*TheModule, MAM);

  // Now if needed, run the legacy PM for codegen.
  if (NeedCodeGen) {
    CodeGenPasses.run(*TheModule);
  }
}

void EmitBackendOutput(DiagnosticsEngine &Diags, const CodeGenOptions &CGOpts,
                       const TargetOptions &TargetOpts,
                       const llvm::DataLayout &TDesc, llvm::Module *TheModule,
                       BackendAction Action,
                       std::unique_ptr<llvm::raw_pwrite_stream> OS) {
  EmitAssemblyHelper AsmHelper(Diags, CGOpts, TargetOpts, TheModule);

  AsmHelper.EmitAssembly(Action, std::move(OS));

  // Verify clang's TargetInfo DataLayout against the LLVM TargetMachine's
  // DataLayout.
  if (AsmHelper.TM) {
    std::string DLDesc = TheModule->getDataLayout().getStringRepresentation();
    if (DLDesc != TDesc.getStringRepresentation()) {
      Diags.Report(diag::err_fe_error_data_layout_mismatch)
          << DLDesc << TDesc.getStringRepresentation();
    }
  }
}

} // namespace soll
