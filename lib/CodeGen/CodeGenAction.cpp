// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/CodeGenAction.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Basic/TargetOptions.h"
#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/Frontend/CompilerInstance.h"
#include "llvm/Support/Alignment.h"
#include <lld/Common/Driver.h>
#include <llvm/IR/ConstantFolder.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Transforms/Utils/Cloning.h>
extern "C" {
typedef void *BinaryenModuleRef;
void BinaryenModuleDispose(BinaryenModuleRef module);
void BinaryenRemoveExport(BinaryenModuleRef module, const char *externalName);
BinaryenModuleRef BinaryenModuleRead(const char *input, size_t inputSize);
size_t BinaryenModuleWrite(BinaryenModuleRef module, char *output,
                           size_t outputSize);
void BinaryenModuleOptimize(BinaryenModuleRef module);
void BinaryenSetOptimizeLevel(int level);
void BinaryenSetShrinkLevel(int level);
void BinaryenSetDebugInfo(int on);
}

namespace {
llvm::Error removeExports(const std::string &Filename) {
  BinaryenModuleRef WasmModule;
  size_t BufferSize = 0;

  {
    auto Binary = llvm::MemoryBuffer::getFile(Filename);
    if (!Binary) {
      return llvm::errorCodeToError(Binary.getError());
    }

    auto Buffer = (*Binary)->getBuffer();
    BufferSize = Buffer.size();
    WasmModule = BinaryenModuleRead(Buffer.data(), Buffer.size());
  }

  BinaryenRemoveExport(WasmModule, "__heap_base");
  BinaryenRemoveExport(WasmModule, "__data_end");
  BinaryenSetOptimizeLevel(0);
  BinaryenSetShrinkLevel(0);
  BinaryenModuleOptimize(WasmModule);

  std::vector<char> OutputBuffer(BufferSize);
  auto Size =
      BinaryenModuleWrite(WasmModule, OutputBuffer.data(), OutputBuffer.size());
  BinaryenModuleDispose(WasmModule);

  std::error_code EC;
  auto WasmStream = llvm::raw_fd_ostream(Filename, EC);
  if (EC) {
    return llvm::errorCodeToError(EC);
  }
  WasmStream.write(OutputBuffer.data(), Size);
  return llvm::Error::success();
}
} // namespace

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
  const CodeGenOptions &CodeGenOpts;
  const TargetOptions &TargetOpts;
  std::string InFile;
  std::unique_ptr<llvm::raw_pwrite_stream> AsmOutStream;
  ASTContext *Context;
  llvm::LLVMContext &LLVMContext;
  std::function<std::unique_ptr<llvm::raw_pwrite_stream>(
      llvm::StringRef, BackendAction, llvm::StringRef)>
      GetOutputStreamCallback;

  std::unique_ptr<CodeGenerator> Gen;

private:
  static void emitEntry(llvm::Module &Module, const std::string &EntryName) {
    llvm::LLVMContext &VMContext = Module.getContext();
    llvm::IRBuilder<llvm::ConstantFolder> Builder(VMContext);
    auto *FuncTy = llvm::FunctionType::get(Builder.getVoidTy(), false);
    auto *MainFunc = llvm::Function::Create(
        FuncTy, llvm::Function::ExternalLinkage, "main", Module);
    llvm::BasicBlock *Entry =
        llvm::BasicBlock::Create(VMContext, "entry", MainFunc);
    Builder.SetInsertPoint(Entry);
    Builder.CreateCall(Module.getFunction(EntryName));
    Builder.CreateRetVoid();
  }

  static void emitNestedBytecodeFunction(llvm::Module &Module,
                                         const std::string &FuncName,
                                         llvm::StringRef Bytecodes) {
    llvm::LLVMContext &VMContext = Module.getContext();
    llvm::IRBuilder<llvm::ConstantFolder> Builder(VMContext);

    llvm::Constant *StrConstant =
        llvm::ConstantDataArray::getString(VMContext, Bytecodes, false);
    const uint64_t Length = StrConstant->getType()->getArrayNumElements();
    auto *GV = new llvm::GlobalVariable(Module, StrConstant->getType(), true,
                                        llvm::GlobalValue::PrivateLinkage,
                                        StrConstant, FuncName + ".data");
    GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    GV->setAlignment(llvm::MaybeAlign(1));

    auto *Func = Module.getFunction(FuncName);
    llvm::BasicBlock *Entry =
        llvm::BasicBlock::Create(VMContext, "entry", Func);
    Builder.SetInsertPoint(Entry);
    llvm::Value *Result = llvm::UndefValue::get(Module.getTypeByName("bytes"));
    Result = Builder.CreateInsertValue(Result, Builder.getIntN(256, Length), 0);
    Result = Builder.CreateInsertValue(
        Result, Builder.CreateBitCast(GV, Builder.getInt8PtrTy()), 1);
    Builder.CreateRet(Result);
  }

  llvm::Expected<std::unique_ptr<llvm::MemoryBuffer>>
  compileAndLink(llvm::Module &Module) {
    auto Object = llvm::sys::fs::TempFile::create(InFile + "-%%%%%%%%%%.o");
    if (!Object) {
      return Object.takeError();
    }

    auto Wasm = llvm::sys::fs::TempFile::create(InFile + "-%%%%%%%%%%.wasm");
    if (!Wasm) {
      llvm::consumeError(Object->discard());
      return Wasm.takeError();
    }

    std::error_code EC;
    auto OutStream =
        std::make_unique<llvm::raw_fd_ostream>(Object->TmpName, EC);
    if (EC) {
      llvm::consumeError(Wasm->discard());
      llvm::consumeError(Object->discard());
      return llvm::errorCodeToError(EC);
    }
    EmitBackendOutput(Diags, CodeGenOpts, TargetOpts, Module.getDataLayout(),
                      &Module, BackendAction::EmitObj, std::move(OutStream));
    const char *Args[] = {
      "wasm-ld",
      "--entry",
      "main",
      "--gc-sections",
      "--allow-undefined",
#if LLVM_VERSION_MAJOR == 9
      // workaround for https://reviews.llvm.org/D63833
      "--export=__heap_base",
#endif
      Object->TmpName.c_str(),
      "-o",
      Wasm->TmpName.c_str()
    };
    lld::wasm::link(llvm::ArrayRef<const char *>(Args), false, llvm::outs(),
                    llvm::errs());

    if (auto Error = removeExports(Wasm->TmpName)) {
      llvm::consumeError(Wasm->discard());
      llvm::consumeError(Object->discard());
      return Error;
    }

    auto Binary = llvm::MemoryBuffer::getFile(Wasm->TmpName);

    llvm::consumeError(Wasm->discard());
    llvm::consumeError(Object->discard());
    return std::move(*Binary);
  }

public:
  BackendConsumer(BackendAction Action, DiagnosticsEngine &Diags,
                  const CodeGenOptions &CodeGenOpts,
                  const TargetOptions &TargetOpts, const std::string &InFile,
                  llvm::LLVMContext &C,
                  std::function<std::unique_ptr<llvm::raw_pwrite_stream>(
                      llvm::StringRef, BackendAction, llvm::StringRef)>
                      GetOutputStreamCallback)
      : Action(Action), Diags(Diags), CodeGenOpts(CodeGenOpts),
        TargetOpts(TargetOpts), InFile(InFile), AsmOutStream(nullptr),
        Context(nullptr), LLVMContext(C),
        GetOutputStreamCallback(GetOutputStreamCallback), Gen(nullptr) {}
  llvm::Module *getModule() const { return Gen->getModule(); }

  CodeGenerator *getCodeGenerator() { return Gen.get(); }

  void Initialize(ASTContext &Ctx) override {
    assert(!Context && "initialized multiple times");
    Context = &Ctx;
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    Gen = std::unique_ptr<CodeGenerator>(
        CreateLLVMCodeGen(Diags, InFile, LLVMContext, CodeGenOpts, TargetOpts));
    Gen->Initialize(*Context);
    Gen->HandleSourceUnit(C, S);

    auto Str = S.getNodes().back()->getName().str() + ".solltmp";
    AsmOutStream = GetOutputStreamCallback(InFile, Action, Str);
    if (Action != BackendAction::EmitNothing && !AsmOutStream) {
      return;
    }
    // Silently ignore if we weren't initialized for some reason.
    if (!getModule()) {
      return;
    }

    if (TargetOpts.BackendTarget == EWASM) {
      for (const auto &[EntryName, FuncName] : Gen->getNestedEntries()) {
        auto ClonedModule = llvm::CloneModule(*getModule());
        emitEntry(*ClonedModule, EntryName);

        auto Binary = compileAndLink(*ClonedModule);
        if (!Binary) {
          llvm::errs() << Binary.takeError() << '\n';
          return;
        }

        emitNestedBytecodeFunction(*getModule(), FuncName,
                                   (*Binary)->getBuffer());
      }
    }

    emitEntry(*getModule(), Gen->getEntry());
    if (Action == BackendAction::EmitWasm) {
      auto Binary = compileAndLink(*getModule());
      if (!Binary) {
        llvm::errs() << Binary.takeError() << '\n';
        return;
      }
      (*AsmOutStream) << (*Binary)->getBuffer();
      AsmOutStream.reset();
    } else {
      EmitBackendOutput(Diags, CodeGenOpts, TargetOpts,
                        getModule()->getDataLayout(), getModule(), Action,
                        std::move(AsmOutStream));
    }
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

std::unique_ptr<ASTConsumer>
CodeGenAction::CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) {
  return std::make_unique<BackendConsumer>(
      Action, CI.getDiagnostics(), CI.getCodeGenOpts(), CI.getTargetOpts(),
      InFile, *VMContext, CI.GetOutputStreamFunc());
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

EmitWasmAction::EmitWasmAction(llvm::LLVMContext *_VMContext)
    : CodeGenAction(BackendAction::EmitWasm, _VMContext) {}

} // namespace soll
