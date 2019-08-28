// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/ModuleBuilder.h"
#include "CodeGenModule.h"
#include "soll/AST/Decl.h"
#include "soll/AST/DeclVisitor.h"
#include "soll/AST/Expr.h"
#include "soll/AST/StmtVisitor.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"
#include <iostream>

#include "../utils/SHA-3/CommandParser.h"
#include "../utils/SHA-3/Keccak.h"
#include "../utils/SHA-3/stdafx.h"

#include "../utils/SHA-3/Endian.h"
#include "../utils/SHA-3/Rotation.h"

using llvm::Function;

namespace {
using namespace soll;
class CodeGeneratorImpl : public CodeGenerator,
                          public ConstDeclVisitor,
                          public ConstStmtVisitor {
  DiagnosticsEngine &Diags;
  ASTContext *Ctx;

  llvm::IntegerType *Int1Ty = nullptr;
  llvm::IntegerType *Int8Ty = nullptr;
  llvm::IntegerType *Int32Ty = nullptr;
  llvm::IntegerType *Int64Ty = nullptr;
  llvm::IntegerType *Int128Ty = nullptr;
  llvm::IntegerType *Int256Ty = nullptr;
  llvm::PointerType *Int8PtrTy = nullptr;
  llvm::PointerType *Int64PtrTy = nullptr;
  llvm::PointerType *Int256PtrTy = nullptr;
  llvm::Type *VoidTy = nullptr;

  llvm::Function *Func_callDataCopy = nullptr;
  llvm::Function *Func_finish = nullptr;
  llvm::Function *Func_revert = nullptr;
  llvm::Function *Func_print32 = nullptr;
  llvm::Function *Func_keccak = nullptr;
  llvm::Function *Func_sstore = nullptr;
  llvm::Function *Func_sload = nullptr;

  llvm::Function *Func_bswap256 = nullptr;

protected:
  std::unique_ptr<llvm::Module> M;
  std::unique_ptr<CodeGen::CodeGenModule> Builder;
  std::unique_ptr<llvm::IRBuilder<llvm::NoFolder>> IRBuilder;

public:
  CodeGeneratorImpl(DiagnosticsEngine &diags, llvm::StringRef ModuleName,
                    llvm::LLVMContext &C)
      : Diags(diags), Ctx(nullptr),
        M(std::make_unique<llvm::Module>(ModuleName, C)) {}

  CodeGen::CodeGenModule &CGM() { return *Builder; }

  llvm::Module *GetModule() { return M.get(); }

  void createTypes() {
    auto &Builder = *IRBuilder;
    Int1Ty = Builder.getInt1Ty();
    Int8Ty = Builder.getInt8Ty();
    Int32Ty = Builder.getInt32Ty();
    Int64Ty = Builder.getInt64Ty();
    Int128Ty = Builder.getIntNTy(128);
    Int256Ty = Builder.getIntNTy(256);
    Int8PtrTy = Builder.getInt8PtrTy();
    Int64PtrTy = llvm::PointerType::getUnqual(Int64Ty);
    Int256PtrTy = llvm::PointerType::getUnqual(Int256Ty);
    VoidTy = Builder.getVoidTy();
  }

  void createEEIDeclaration() {
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = nullptr;

    // CallDataCopy
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
    Func_callDataCopy = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "callDataCopy", *M);
    Func_callDataCopy->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // finish
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
    Func_finish = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "finish", *M);
    Func_finish->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // revert
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
    Func_revert = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "revert", *M);
    Func_revert->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // keccak
    FT = llvm::FunctionType::get(Int256Ty, {Int8PtrTy, Int256Ty}, false);
    Func_keccak = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "keccak", *M);
    Func_keccak->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // sload
    FT = llvm::FunctionType::get(Int256Ty, {Int256Ty}, false);
    Func_sload = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "sload", *M);
    Func_sload->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // sstore
    FT = llvm::FunctionType::get(VoidTy, {Int256Ty, Int256Ty}, false);
    Func_sstore = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "sstore", *M);
    Func_sstore->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum"));

    // debug.print32
    FT = llvm::FunctionType::get(VoidTy, {Int32Ty}, false);
    Func_print32 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                          "print32", *M);
    Func_print32->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-module", "debug"));
  }

  void createBswapI256() {
    llvm::LLVMContext &Context = M->getContext();
    auto *const Arg = Func_bswap256->arg_begin();
    auto &Builder = *IRBuilder;

    llvm::BasicBlock *Entry =
        llvm::BasicBlock::Create(Context, "entry", Func_bswap256);
    Builder.SetInsertPoint(Entry);

    llvm::Value *data[32];
    for (size_t i = 0; i < 32; ++i) {
      if (i < 16) {
        data[i] = Builder.CreateShl(Arg, 248 - i * 16);
      } else {
        data[i] = Builder.CreateLShr(Arg, i * 16 - 248);
      }
      if (i != 0 && i != 31) {
        data[i] = Builder.CreateAnd(data[i], llvm::APInt(256, 0xFF, false)
                                                 << ((31 - i) * 8));
      }
    }
    llvm::Value *result = Builder.CreateOr(data[0], data[1]);
    for (size_t i = 2; i < 32; ++i) {
      result = Builder.CreateOr(result, data[i]);
    }

    Builder.CreateRet(result);
  }

  void createI256Arithmetic() {
    llvm::LLVMContext &Context = M->getContext();

    Func_bswap256 = llvm::Function::Create(
        llvm::FunctionType::get(Int256Ty, {Int256Ty}, false),
        llvm::Function::InternalLinkage, "__bswapi256", *M);

    createBswapI256();
  }

  void Initialize(ASTContext &Context) override {
    Ctx = &Context;

    M->setTargetTriple("wasm32-unknown-unknown-wasm");
    // WebAssembly32TargetInfo
    M->setDataLayout(llvm::DataLayout("e-m:e-p:32:32-i64:64-n32:64-S128"));
    Builder.reset(new CodeGen::CodeGenModule(Context, *M, Diags));
    IRBuilder =
        std::make_unique<llvm::IRBuilder<llvm::NoFolder>>(M->getContext());
    createTypes();
    createEEIDeclaration();
    createI256Arithmetic();
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(SourceUnitType &SU) override {
    for (auto Node : SU.getNodes())
      Node->accept(*this);
  }

  void visit(ContractDeclType &CD) override {
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = nullptr;

    // main
    FT = llvm::FunctionType::get(VoidTy, {}, false);
    llvm::Function *Main =
        llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", *M);
    llvm::BasicBlock *EntryBB =
        llvm::BasicBlock::Create(Context, "entry", Main);

    IRBuilder->SetInsertPoint(EntryBB);
    auto *p = IRBuilder->CreateAlloca(Int32Ty, nullptr, "code.ptr");
    auto *voidptr = IRBuilder->CreateBitCast(p, Int8PtrTy, "code.voidptr");
    IRBuilder->CreateCall(Func_callDataCopy, {voidptr, IRBuilder->getInt32(0),
                                              IRBuilder->getInt32(4)});
    auto *CondV = IRBuilder->CreateLoad(Int32Ty, p, "hash");

    // two phase codegen
    llvm::BasicBlock *Default =
        llvm::BasicBlock::Create(Context, "default", Main);
    IRBuilder->SetInsertPoint(Default);
    IRBuilder->CreateCall(
        Func_revert,
        {llvm::ConstantPointerNull::get(Int8PtrTy), IRBuilder->getInt32(0)});
    IRBuilder->CreateUnreachable();

    IRBuilder->SetInsertPoint(EntryBB);

    std::map<std::string, llvm::BasicBlock *> Labels;

    llvm::Value *FakeCondV = IRBuilder->getInt32(0);
    llvm::SwitchInst *SI =
        IRBuilder->CreateSwitch(CondV, Default, CD.getFuncs().size());

    for (auto F : CD.getFuncs()) {
      llvm::BasicBlock *CondBB =
          llvm::BasicBlock::Create(Context, F->getName(), Main);
      Labels[F->getName()] = CondBB;
      std::uint32_t hash = funcSignatureHash(*F);
      SI->addCase(IRBuilder->getInt32(hash), CondBB);
    }

    for (auto F : CD.getFuncs())
      genABI(*F, Labels[F->getName()]);

    // codegen function body
    ConstDeclVisitor::visit(CD);
  }

  void visit(FunctionDeclType &F) override {
    FuncBodyCodeGen(M->getContext(), *IRBuilder, *GetModule()).compile(F);
  }

  size_t getABISize(const std::vector<const VarDecl *> &params) const {
    // TODO: get real size
    return params.size() * 32;
  }

  size_t getABIOffset(const std::vector<const VarDecl *> &params,
                      size_t i) const {
    // TODO: get real offset
    return i * 32;
  }

  llvm::Type* getLLVMTy(const VarDecl *VD) {
    return IRBuilder->getIntNTy(VD->GetType()->getBitNum());
  }
  llvm::Type* getLLVMTy(const FunctionDeclType &F) {
    auto *Ty = F.getReturnParams()->getParams()[0]->GetType().get();
    return IRBuilder->getIntNTy(Ty->getBitNum());
  }
  llvm::Value* castToTy(llvm::Value *Val, llvm::Type *TargetTy) {
    return IRBuilder->CreateTrunc(Val, TargetTy, "trunc");
  }
  llvm::Value* extTo256(llvm::Value *Val) {
    return IRBuilder->CreateZExt(Val, Int256Ty, "zext");
  }

  void genABI(FunctionDeclType &F, llvm::BasicBlock *BB) {
    // TODO: refactor this
    // this impl. assumes all types are uint64
    const std::string &Fname = F.getName();
    const auto &Fparams = F.getParams()->getParams();

    IRBuilder->SetInsertPoint(BB);
    // get arguments from calldata
    auto *arg_cptr = IRBuilder->CreateAlloca(
        llvm::ArrayType::get(Int8Ty, getABISize(Fparams)), nullptr,
        Fname + "_arg_cptr");
    auto *arg_vptr =
        IRBuilder->CreateBitCast(arg_cptr, Int8PtrTy, Fname + "_arg_vptr");
    IRBuilder->CreateCall(Func_callDataCopy,
                          {arg_vptr, IRBuilder->getInt32(4),
                           IRBuilder->getInt32(getABISize(Fparams))});

    std::vector<llvm::Value *> ArgsVal;
    std::vector<llvm::Type *> ArgsTy;
    for (size_t i = 0; i < Fparams.size(); i++) {
      std::string param_name = (Fname + "_" + Fparams[i]->getName()).str();
      auto *cptr = IRBuilder->CreateInBoundsGEP(
          arg_cptr,
          {IRBuilder->getInt32(0),
           IRBuilder->getInt32(getABIOffset(Fparams, i))},
          param_name + "_cptr");
      auto *ptr_b =
          IRBuilder->CreateBitCast(cptr, Int256PtrTy, param_name + "_ptr_b");
      auto *val_b = IRBuilder->CreateLoad(Int256Ty, ptr_b, param_name + "_b");
      auto *val = IRBuilder->CreateCall(Func_bswap256, {val_b}, param_name);
      ArgsVal.push_back(castToTy(val, getLLVMTy(Fparams[i])));
      ArgsTy.push_back(getLLVMTy(Fparams[i]));
    }

    // Call this function
    llvm::FunctionType *FT = llvm::FunctionType::get(getLLVMTy(F), ArgsTy, false);
    Function *Func = Function::Create(FT, Function::ExternalLinkage,
                                      F.getName(), *GetModule());
    auto *r = IRBuilder->CreateCall(Func, ArgsVal, Fname + "_r");
    auto *ext_r = extTo256(r);
    auto *r_b = IRBuilder->CreateCall(Func_bswap256, {ext_r}, Fname + "_r_b");

    // put return value to returndata
    auto *r_ptr = IRBuilder->CreateAlloca(Int256Ty, nullptr, Fname + "_r_ptr");
    IRBuilder->CreateStore(r_b, r_ptr);
    auto *r_vptr = IRBuilder->CreateBitCast(
        r_ptr, llvm::PointerType::getUnqual(Int8Ty), Fname + "_r_vptr");
    IRBuilder->CreateCall(M->getFunction("finish"),
                          {r_vptr, IRBuilder->getInt32(256 / 8)});
    IRBuilder->CreateUnreachable();
  }

  std::uint32_t funcSignatureHash(const FunctionDecl &F) {
    Keccak h(256);
    h.addData(F.getName().bytes_begin(), 0, F.getName().size());
    h.addData('(');
    bool first = true;
    for (const VarDecl *var : F.getParams()->getParams()) {
      if (!first)
        h.addData(',');
      first = false;
      // XXX: Implement typename
      static const std::array<uint8_t, 7> type = {'u', 'i', 'n', 't',
                                                  '2', '5', '6'};
      h.addData(type.data(), 0, type.size());
    }
    h.addData(')');
    const std::vector<std::uint8_t> op = h.digest();
    std::uint32_t hash = 0;
    for (int i = 0; i < 4; i++)
      hash = (hash << 8) | op[i];
    return __builtin_bswap32(hash);
  }
}; // namespace

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
