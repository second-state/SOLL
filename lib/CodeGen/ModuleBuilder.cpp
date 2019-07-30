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

  void visit(ContractDeclType &CD) override {
    std::vector<llvm::Type *> ArgsType;
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = nullptr;

    // CallDataCopy
    ArgsType.clear();
    ArgsType.push_back(llvm::Type::getInt8PtrTy(Context));
    ArgsType.push_back(llvm::Type::getInt32Ty(Context));
    ArgsType.push_back(llvm::Type::getInt32Ty(Context));    
    FT = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), ArgsType, false);
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "callDataCopy", *M);

    // finish
    ArgsType.clear();
    ArgsType.push_back(llvm::Type::getInt8PtrTy(Context));
    ArgsType.push_back(llvm::Type::getInt32Ty(Context));
    FT = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), ArgsType, false);
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "finish", *M);

    // revert
    ArgsType.clear();
    ArgsType.push_back(llvm::Type::getInt8PtrTy(Context));
    ArgsType.push_back(llvm::Type::getInt32Ty(Context));
    FT = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), ArgsType, false);
    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "revert", *M);

    // main
    ArgsType.clear();
    FT = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), ArgsType, false);
    llvm::Function *Main = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", *M);
    llvm::BasicBlock *EntryBB = llvm::BasicBlock::Create(Context, "entry", Main);
    llvm::IRBuilder<llvm::NoFolder> IRBuilder(Context);
    

    // two phase codegen
    llvm::BasicBlock *Default = llvm::BasicBlock::Create(Context, "default", Main);
    IRBuilder.SetInsertPoint(Default);
    std::vector<llvm::Value *> ArgsV;
    ArgsV.push_back(llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(Context)));
    ArgsV.push_back(IRBuilder.getInt32(0));
    IRBuilder.CreateCall(M->getFunction("revert"), ArgsV, "callrevert");

    IRBuilder.SetInsertPoint(EntryBB);
    llvm::Value *FakeCondV = IRBuilder.getInt32(0);
    llvm::SwitchInst * SI = IRBuilder.CreateSwitch(FakeCondV, Default, CD.getSubNodes().size());
    for (auto F : CD.getSubNodes()) {
      std::string signature = F->getName().str();
      llvm::BasicBlock * CondBB = llvm::BasicBlock::Create(Context, signature, Main);
      signature += '(';
      bool first = true;
      for (const VarDecl *var : dynamic_cast<FunctionDeclType *>(F)->getParams()->getParams()) {
        if (!first)
          signature += ',';
        first = false;
        signature += "uint256"; // XXX: Implement typename
      }
      signature += ')';
      {
        Keccak h(256);
        h.addData((uint8_t*)signature.c_str(),0,signature.length());
        std::vector<unsigned char> op = h.digest();
        uint hash = 0;
        for(int i = 0; i < 4; i++){
          hash = (hash << 8) | op[i];
        }
        SI->addCase (IRBuilder.getInt32(hash), CondBB);
      }
      
    }

    for (auto F : CD.getSubNodes()) {
      F->accept(*this);
    }
  }

  void visit(FunctionDeclType &F) override {
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
