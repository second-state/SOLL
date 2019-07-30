#include "soll/CodeGen/ModuleBuilder.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include "CodeGenModule.h"
#include "soll/AST/Decl.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"
#include <iostream>

#include "../utils/SHA-3/stdafx.h"
#include "../utils/SHA-3/Keccak.h"
#include "../utils/SHA-3/CommandParser.h"

#include "../utils/SHA-3/Endian.h"
#include "../utils/SHA-3/Rotation.h"

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

public:
  CodeGeneratorImpl(DiagnosticsEngine &diags, llvm::StringRef ModuleName,
                    llvm::LLVMContext &C)
      : Diags(diags), Ctx(nullptr),
        M(std::make_unique<llvm::Module>(ModuleName, C)) {}

  CodeGen::CodeGenModule &CGM() { return *Builder; }

  llvm::Module *GetModule() { return M.get(); }

  void Initialize(ASTContext &Context) override {
    Ctx = &Context;

    M->setTargetTriple("wasm32-unknown-unknown-wasm");
    // WebAssembly32TargetInfo
    M->setDataLayout(llvm::DataLayout("e-m:e-p:32:32-i64:64-n32:64-S128"));
    Builder.reset(new CodeGen::CodeGenModule(Context, *M, Diags));
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(FunctionDeclType &F) override {
    std::string signature = F.getName().str();
    signature += '(';
    bool first = true;
    for (const VarDecl *var : F.getParams()->getParams()) {
      if (!first)
        signature += ',';
      first = false;
      signature += "uint256"; // XXX: Implement typename
    }
    signature += ')';
    std::cout << signature << std::endl;
    {
      Keccak h(256);
      h.addData((uint8_t*)signature.c_str(),0,signature.length());
      std::vector<unsigned char> op = h.digest();
      for(int i = 0; i < 4; i++)
        printf("%02x", op[i]);
      printf("\n");
    }
    llvm::IRBuilder<llvm::NoFolder> IRBuilder(M->getContext());
    FuncBodyCodeGen(M->getContext(), IRBuilder, *GetModule()).compile(F);
  }
};

} // namespace

namespace soll {

CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
                                 llvm::StringRef ModuleName,
                                 llvm::LLVMContext &C) {
  return new CodeGeneratorImpl(Diags, ModuleName, C);
}

llvm::Module *CodeGenerator::GetModule() {
  return static_cast<CodeGeneratorImpl *>(this)->GetModule();
}

void CodeGenerator::HandleSourceUnit(ASTContext &C, SourceUnit &S) {
  return static_cast<CodeGeneratorImpl *>(this)->HandleSourceUnit(C, S);
}

} // namespace soll
