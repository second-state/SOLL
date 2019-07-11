#pragma once
#include "soll/Frontend/FrontendOptions.h"
#include <llvm/IR/Module.h>
#include <memory>

namespace soll {

class ASTConsumer;
class CompilerInstance;

class FrontendAction {
  FrontendInputFile CurrentInput;
  CompilerInstance *Instance = nullptr;

protected:
  virtual bool PrepareToExecuteAction(CompilerInstance &CI) { return true; }

  virtual std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) = 0;

  virtual bool BeginInvocation(CompilerInstance &CI) { return true; }

  virtual bool BeginSourceFileAction(CompilerInstance &CI) { return true; }

  virtual void ExecuteAction() = 0;

  virtual void EndSourceFileAction() {}

  virtual bool shouldEraseOutputFiles();

public:
  FrontendAction();
  virtual ~FrontendAction();

  CompilerInstance &getCompilerInstance() const {
    assert(Instance && "Compiler instance not registered!");
    return *Instance;
  }

  void setCompilerInstance(CompilerInstance *Value) { Instance = Value; }

  const FrontendInputFile &getCurrentInput() const { return CurrentInput; }

  llvm::StringRef getCurrentFile() const {
    assert(!CurrentInput.isEmpty() && "No current file!");
    return CurrentInput.getFile();
  }

  llvm::StringRef getCurrentFileOrBufferName() const {
    assert(!CurrentInput.isEmpty() && "No current file!");
    return CurrentInput.isFile()
               ? CurrentInput.getFile()
               : CurrentInput.getBuffer()->getBufferIdentifier();
  }

  llvm::Module *getCurrentModule() const;

  void setCurrentInput(const FrontendInputFile &CurrentInput);

  bool PrepareToExecute(CompilerInstance &CI) {
    return PrepareToExecuteAction(CI);
  }

  bool BeginSourceFile(CompilerInstance &CI, const FrontendInputFile &Input);

  bool Execute();

  void EndSourceFile();
};

class ASTFrontendAction : public FrontendAction {
protected:
  void ExecuteAction() override;

public:
  ASTFrontendAction() {}
  virtual ~ASTFrontendAction() {}
};

} // namespace soll
