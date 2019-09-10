// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Frontend/CompilerInvocation.h"
#include "soll/Frontend/DiagnosticRenderer.h"
#include "soll/Lex/Lexer.h"
#include "soll/Sema/Sema.h"
#include <algorithm>
#include <list>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <memory>

namespace soll {

class FileManager;
class FrontendAction;
class FrontendInputFile;
class SourceManager;

class CompilerInstance {
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> VirtualFileSystem;
  std::unique_ptr<CompilerInvocation> Invocation;
  llvm::IntrusiveRefCntPtr<DiagnosticsEngine> Diagnostics;
  llvm::IntrusiveRefCntPtr<FileManager> FileMgr;
  llvm::IntrusiveRefCntPtr<SourceManager> SourceMgr;
  std::unique_ptr<Lexer> TheLexer;
  llvm::IntrusiveRefCntPtr<ASTContext> Context;
  std::unique_ptr<ASTConsumer> Consumer;
  std::unique_ptr<Sema> TheSema;

  struct OutputFile {
    std::string Filename;
    std::string TempFilename;

    OutputFile(std::string filename, std::string tempFilename)
        : Filename(std::move(filename)), TempFilename(std::move(tempFilename)) {
    }
  };

  std::unique_ptr<llvm::raw_fd_ostream> NonSeekStream;
  std::list<OutputFile> OutputFiles;

  CompilerInstance(const CompilerInstance &) = delete;
  CompilerInstance &operator=(const CompilerInstance &) = delete;

public:
  explicit CompilerInstance();
  CompilerInvocation &GetInvocation();

  FileSystemOptions &getFileSystemOpts() {
    return Invocation->getFileSystemOpts();
  }
  const FileSystemOptions &getFileSystemOpts() const {
    return Invocation->getFileSystemOpts();
  }

  FrontendOptions &getFrontendOpts() { return Invocation->getFrontendOpts(); }
  const FrontendOptions &getFrontendOpts() const {
    return Invocation->getFrontendOpts();
  }

  TargetOptions &getTargetOpts() { return Invocation->getTargetOpts(); }
  const TargetOptions &getTargetOpts() const {
    return Invocation->getTargetOpts();
  }

  bool hasDiagnostics() const { return Diagnostics != nullptr; }
  DiagnosticsEngine &getDiagnostics() const {
    assert(Diagnostics && "Compiler instance has no diagnostics!");
    return *Diagnostics;
  }
  void setDiagnostics(DiagnosticsEngine *Value);
  DiagnosticConsumer &getDiagnosticClient() const {
    assert(Diagnostics && Diagnostics->getClient() &&
           "Compiler instance has no diagnostic client!");
    return *Diagnostics->getClient();
  }

  bool hasVirtualFileSystem() const { return VirtualFileSystem != nullptr; }
  llvm::vfs::FileSystem &getVirtualFileSystem() const {
    assert(hasVirtualFileSystem() &&
           "Compiler instance has no virtual file system");
    return *VirtualFileSystem;
  }
  void
  setVirtualFileSystem(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS) {
    VirtualFileSystem = std::move(FS);
  }

  bool hasFileManager() const { return FileMgr != nullptr; }
  FileManager &getFileManager() const {
    assert(FileMgr && "Compiler instance has no file manager!");
    return *FileMgr;
  }
  void setFileManager(FileManager *Value);

  bool hasSourceManager() const { return SourceMgr != nullptr; }
  SourceManager &getSourceManager() const {
    assert(SourceMgr && "Compiler instance has no source manager!");
    return *SourceMgr;
  }
  void setSourceManager(SourceManager *Value);

  bool hasLexer() const { return static_cast<bool>(TheLexer); }
  Lexer &getLexer() const {
    assert(TheLexer && "Compiler instance has no lexer!");
    return *TheLexer;
  }
  void setLexer(std::unique_ptr<Lexer> &&Value);

  bool hasASTContext() const { return Context != nullptr; }
  ASTContext &getASTContext() const {
    assert(Context && "Compiler instance has no AST context!");
    return *Context;
  }
  void setASTContext(ASTContext *Value);

  bool hasASTConsumer() const { return static_cast<bool>(Consumer); }
  ASTConsumer &getASTConsumer() const {
    assert(Consumer && "Compiler instance has no AST consumer!");
    return *Consumer;
  }
  void setASTConsumer(std::unique_ptr<ASTConsumer> &&Value);

  bool hasSema() const { return static_cast<bool>(TheSema); }
  Sema &getSema() const {
    assert(TheSema && "Compiler instance has no Sema object!");
    return *TheSema;
  }
  void setSema(std::unique_ptr<Sema> &&S);

  void addOutputFile(OutputFile &&OutFile);
  void clearOutputFiles(bool EraseFiles);

  void createDiagnostics(DiagnosticConsumer *Client = nullptr,
                         bool ShouldOwnClient = true);
  static llvm::IntrusiveRefCntPtr<DiagnosticsEngine>
  createDiagnostics(DiagnosticOptions *Opts,
                    DiagnosticConsumer *Client = nullptr,
                    bool ShouldOwnClient = true);
  FileManager *createFileManager();
  void createSourceManager(FileManager &FileMgr);
  void createLexer();
  void createASTContext();
  void createSema();

  std::unique_ptr<llvm::raw_pwrite_stream>
  createDefaultOutputFile(bool Binary = true, llvm::StringRef BaseInput = "",
                          llvm::StringRef Extension = "");

  std::unique_ptr<llvm::raw_pwrite_stream>
  createOutputFile(llvm::StringRef OutputPath, bool Binary,
                   bool RemoveFileOnSignal, llvm::StringRef BaseInput,
                   llvm::StringRef Extension, bool UseTemporary,
                   bool CreateMissingDirectories = false);

  std::unique_ptr<llvm::raw_pwrite_stream>
  createOutputFile(llvm::StringRef OutputPath, std::error_code &Error,
                   bool Binary, bool RemoveFileOnSignal, llvm::StringRef InFile,
                   llvm::StringRef Extension, bool UseTemporary,
                   bool CreateMissingDirectories, std::string *ResultPathName,
                   std::string *TempPathName);

  bool InitializeSourceManager(const FrontendInputFile &Input);

  bool Execute();
  bool ExecuteAction(FrontendAction &action, llvm::StringRef filename);
};

} // namespace soll
