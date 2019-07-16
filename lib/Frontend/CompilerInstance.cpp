#include "soll/Frontend/CompilerInstance.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/Basic/DiagnosticIDs.h"
#include "soll/Basic/FileManager.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Frontend/ASTConsumers.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/TextDiagnostic.h"
#include "soll/Frontend/TextDiagnosticPrinter.h"
#include "soll/Lex/Lexer.h"
#include "soll/Parse/ParseAST.h"
#include "soll/Sema/Sema.h"
#include <cassert>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>

namespace soll {

CompilerInstance::CompilerInstance()
    : Invocation(std::make_unique<CompilerInvocation>()) {}

bool CompilerInstance::Execute() { return Invocation->Execute(*this); }

CompilerInvocation &CompilerInstance::GetInvocation() {
  assert(Invocation.get() != nullptr);
  return *Invocation;
}

void CompilerInstance::setASTContext(ASTContext *Value) {
  Context = Value;

  if (Context && Consumer)
    getASTConsumer().Initialize(getASTContext());
}

void CompilerInstance::setASTConsumer(std::unique_ptr<ASTConsumer> Value) {
  Consumer = std::move(Value);

  if (Context && Consumer)
    getASTConsumer().Initialize(getASTContext());
}

void CompilerInstance::createDiagnostics(DiagnosticConsumer *Client,
                                         bool ShouldOwnClient) {
  DiagnosticOptions *Opts = &Invocation->GetDiagnosticOptions();
  llvm::IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
  Diagnostics = new DiagnosticsEngine(DiagID, Opts);

  if (Client) {
    Diagnostics->setClient(Client, ShouldOwnClient);
  } else {
    Diagnostics->setClient(new TextDiagnosticPrinter(llvm::errs(), Opts), true);
  }
}

void CompilerInstance::createLexer(FileID FID) {
  const llvm::MemoryBuffer *InputFile = SourceMgr->getBuffer(FID);

  TheLexer = std::make_unique<Lexer>(FID, InputFile, *SourceMgr);
  TheLexer->Initialize();
}

FileManager *CompilerInstance::createFileManager() {
  if (!hasVirtualFileSystem()) {
    llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> VFS =
        llvm::vfs::getRealFileSystem();
    setVirtualFileSystem(VFS);
  }
  FileMgr = new FileManager(getFileSystemOpts(), VirtualFileSystem);
  return FileMgr.get();
}

void CompilerInstance::createSourceManager(FileManager &FileMgr) {
  SourceMgr = new SourceManager(getDiagnostics(), FileMgr);
}

void CompilerInstance::createSema() {
  TheSema =
      std::make_unique<Sema>(getLexer(), getASTContext(), getASTConsumer());
}

bool CompilerInstance::Execute(llvm::StringRef filename) {
  static llvm::LLVMContext Context;
  static llvm::IRBuilder<> Build(Context);

  std::unique_ptr<llvm::Module> Module =
      std::make_unique<llvm::Module>(filename, Context);

  if (!hasDiagnostics()) {
    createDiagnostics();
  }

  if (!hasFileManager()) {
    if (!createFileManager()) {
      return false;
    }
  }
  if (!hasSourceManager())
    createSourceManager(getFileManager());

  const FileEntry *Input = FileMgr->getFile(filename, true);
  if (!Input) {
    Diagnostics->Report(diag::err_cannot_open_file) << filename;
    return false;
  }
  FileID FID = SourceMgr->getOrCreateFileID(Input);
  SourceMgr->setMainFileID(FID);

  auto InputBuffer = FileMgr->getBufferForFile(Input);
  if (!InputBuffer) {
    return false;
  }

  setASTConsumer(CreateASTPrinter(nullptr, ""));
  setASTContext(new ASTContext);
  createLexer(FID);

  if (!hasSema())
    createSema();

  ParseAST(getSema(), true);

  /*
  Lexer L(id, InputBuffer.get().get());
  L.Lex();
  */

  Module->print(llvm::errs(), nullptr);

  return true;
}

} // namespace soll
