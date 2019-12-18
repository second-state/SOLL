// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/CompilerInstance.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/DiagnosticIDs.h"
#include "soll/Basic/FileManager.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Frontend/ASTConsumers.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/FrontendAction.h"
#include "soll/Frontend/FrontendActions.h"
#include "soll/Frontend/TextDiagnostic.h"
#include "soll/Frontend/TextDiagnosticPrinter.h"
#include "soll/Lex/Lexer.h"
#include "soll/Sema/Sema.h"
#include <cassert>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Errc.h>
#include <llvm/Support/Signals.h>
#include <memory>

namespace soll {

CompilerInstance::CompilerInstance()
    : Invocation(std::make_unique<CompilerInvocation>()) {}

CompilerInvocation &CompilerInstance::GetInvocation() {
  assert(Invocation.get() != nullptr);
  return *Invocation;
}

void CompilerInstance::setFileManager(FileManager *Value) { FileMgr = Value; }

void CompilerInstance::setSourceManager(SourceManager *Value) {
  SourceMgr = Value;
}

void CompilerInstance::setASTContext(ASTContext *Value) {
  Context = Value;

  if (Context && Consumer)
    getASTConsumer().Initialize(getASTContext());
}

void CompilerInstance::setLexer(std::unique_ptr<Lexer> &&Value) {
  TheLexer = std::move(Value);
}

void CompilerInstance::setSema(std::unique_ptr<Sema> &&S) {
  TheSema = std::move(S);
}

void CompilerInstance::setASTConsumer(std::unique_ptr<ASTConsumer> &&Value) {
  Consumer = std::move(Value);

  if (Context && Consumer)
    getASTConsumer().Initialize(getASTContext());
}

void CompilerInstance::addOutputFile(OutputFile &&OutFile) {
  OutputFiles.push_back(std::move(OutFile));
}

void CompilerInstance::clearOutputFiles(bool EraseFiles) {
  for (OutputFile &OF : OutputFiles) {
    if (!OF.TempFilename.empty()) {
      if (EraseFiles) {
        llvm::sys::fs::remove(OF.TempFilename);
      } else {
        llvm::SmallString<128> NewOutFile(OF.Filename);

        // If '-working-directory' was passed, the output filename should be
        // relative to that.
        FileMgr->FixupRelativePath(NewOutFile);
        if (std::error_code ec =
                llvm::sys::fs::rename(OF.TempFilename, NewOutFile)) {
          getDiagnostics().Report(diag::err_unable_to_rename_temp)
              << OF.TempFilename << OF.Filename << ec.message();

          llvm::sys::fs::remove(OF.TempFilename);
        }
      }
    } else if (!OF.Filename.empty() && EraseFiles)
      llvm::sys::fs::remove(OF.Filename);
  }
  OutputFiles.clear();
  NonSeekStream.reset();
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

void CompilerInstance::createLexer() {
  FileID FID = SourceMgr->getMainFileID();
  const llvm::MemoryBuffer *InputFile = SourceMgr->getBuffer(FID);
  assert(InputFile != nullptr);

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

void CompilerInstance::createASTContext() {
  auto *Context = new ASTContext();
  setASTContext(Context);
}

void CompilerInstance::createSema() {
  TheSema =
      std::make_unique<Sema>(getLexer(), getASTContext(), getASTConsumer());
}

std::unique_ptr<llvm::raw_pwrite_stream>
CompilerInstance::createDefaultOutputFile(bool Binary, llvm::StringRef InFile,
                                          llvm::StringRef Extension) {
  return createOutputFile("-", Binary,
                          /*RemoveFileOnSignal=*/true, InFile, Extension,
                          /*UseTemporary=*/true);
}

std::unique_ptr<llvm::raw_pwrite_stream> CompilerInstance::createOutputFile(
    llvm::StringRef OutputPath, bool Binary, bool RemoveFileOnSignal,
    llvm::StringRef InFile, llvm::StringRef Extension, bool UseTemporary,
    bool CreateMissingDirectories) {
  std::string OutputPathName, TempPathName;
  std::error_code EC;
  std::unique_ptr<llvm::raw_pwrite_stream> OS = createOutputFile(
      OutputPath, EC, Binary, RemoveFileOnSignal, InFile, Extension,
      UseTemporary, CreateMissingDirectories, &OutputPathName, &TempPathName);
  if (!OS) {
    // getDiagnostics().Report(diag::err_fe_unable_to_open_output) << OutputPath
    // << EC.message();
    getDiagnostics().Report(diag::err_cannot_open_file) << OutputPath;
    return nullptr;
  }

  addOutputFile(
      OutputFile((OutputPathName != "-") ? OutputPathName : "", TempPathName));

  return OS;
}

std::unique_ptr<llvm::raw_pwrite_stream> CompilerInstance::createOutputFile(
    llvm::StringRef OutputPath, std::error_code &Error, bool Binary,
    bool RemoveFileOnSignal, llvm::StringRef InFile, llvm::StringRef Extension,
    bool UseTemporary, bool CreateMissingDirectories,
    std::string *ResultPathName, std::string *TempPathName) {
  assert((!CreateMissingDirectories || UseTemporary) &&
         "CreateMissingDirectories is only allowed when using temporary files");

  std::string OutFile, TempFile;
  if (!OutputPath.empty()) {
    OutFile = OutputPath;
  } else if (InFile == "-") {
    OutFile = "-";
  } else if (!Extension.empty()) {
    llvm::SmallString<128> Path(InFile);
    llvm::sys::path::replace_extension(Path, Extension);
    OutFile = Path.str();
  } else {
    OutFile = "-";
  }

  std::unique_ptr<llvm::raw_fd_ostream> OS;
  std::string OSFile;

  if (UseTemporary) {
    if (OutFile == "-")
      UseTemporary = false;
    else {
      llvm::sys::fs::file_status Status;
      llvm::sys::fs::status(OutputPath, Status);
      if (llvm::sys::fs::exists(Status)) {
        if (!llvm::sys::fs::can_write(OutputPath)) {
          Error = make_error_code(llvm::errc::operation_not_permitted);
          return nullptr;
        }

        if (!llvm::sys::fs::is_regular_file(Status))
          UseTemporary = false;
      }
    }
  }

  if (UseTemporary) {
    llvm::StringRef OutputExtension = llvm::sys::path::extension(OutFile);
    llvm::SmallString<128> TempPath =
        llvm::StringRef(OutFile).drop_back(OutputExtension.size());
    TempPath += "-%%%%%%%%";
    TempPath += OutputExtension;
    TempPath += ".tmp";
    int fd;
    std::error_code EC =
        llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);

    if (CreateMissingDirectories &&
        EC == llvm::errc::no_such_file_or_directory) {
      llvm::StringRef Parent = llvm::sys::path::parent_path(OutputPath);
      EC = llvm::sys::fs::create_directories(Parent);
      if (!EC) {
        EC = llvm::sys::fs::createUniqueFile(TempPath, fd, TempPath);
      }
    }

    if (!EC) {
      OS.reset(new llvm::raw_fd_ostream(fd, /*shouldClose=*/true));
      OSFile = TempFile = TempPath.str();
    }
  }

  if (!OS) {
    OSFile = OutFile;
    OS.reset(new llvm::raw_fd_ostream(
        OSFile, Error,
        (Binary ? llvm::sys::fs::F_None : llvm::sys::fs::F_Text)));
    if (Error)
      return nullptr;
  }

  if (RemoveFileOnSignal)
    llvm::sys::RemoveFileOnSignal(OSFile);

  if (ResultPathName)
    *ResultPathName = OutFile;
  if (TempPathName)
    *TempPathName = TempFile;

  if (!Binary || OS->supportsSeeking())
    return std::move(OS);

  auto B = std::make_unique<llvm::buffer_ostream>(*OS);
  assert(!NonSeekStream);
  NonSeekStream = std::move(OS);
  return std::move(B);
}

bool CompilerInstance::InitializeSourceManager(const FrontendInputFile &Input) {
  llvm::StringRef InputFile = Input.getFile();

  const FileEntry *File = FileMgr->getFile(InputFile, /*OpenFile=*/true);
  if (!File) {
    // Diagnostics->Report(diag::err_fe_error_reading) << InputFile;
    Diagnostics->Report(diag::err_cannot_open_file)
        << InputFile << std::strerror(errno);
    return false;
  }

  SourceMgr->setMainFileID(SourceMgr->createFileID(File, SourceLocation()));

  assert(SourceMgr->getMainFileID().isValid() &&
         "Couldn't establish MainFileID!");
  return true;
}

bool CompilerInstance::ExecuteAction(FrontendAction &Act) {
  if (!hasDiagnostics()) {
    createDiagnostics();
  }

  for (FrontendInputFile &InputFile : getFrontendOpts().Inputs) {
    if (Act.BeginSourceFile(*this, InputFile)) {
      Act.Execute();
      Act.EndSourceFile();
    }
  }
  return !getDiagnostics().getClient()->getNumErrors();
}

} // namespace soll
