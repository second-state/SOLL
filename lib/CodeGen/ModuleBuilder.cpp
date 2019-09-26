// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/CodeGen/ModuleBuilder.h"
#include "CodeGenModule.h"
#include "soll/AST/AST.h"
#include "soll/CodeGen/FuncBodyCodeGen.h"

#include "../utils/SHA-3/CommandParser.h"
#include "../utils/SHA-3/Keccak.h"
#include "../utils/SHA-3/stdafx.h"

#include "../utils/SHA-3/Endian.h"
#include "../utils/SHA-3/Rotation.h"

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
  llvm::IntegerType *Int160Ty = nullptr;
  llvm::IntegerType *Int256Ty = nullptr;
  llvm::PointerType *Int8PtrTy = nullptr;
  llvm::PointerType *Int32PtrTy = nullptr;
  llvm::PointerType *Int64PtrTy = nullptr;
  llvm::PointerType *Int128PtrTy = nullptr;
  llvm::PointerType *Int160PtrTy = nullptr;
  llvm::PointerType *Int256PtrTy = nullptr;
  llvm::Type *VoidTy = nullptr;

  llvm::StructType *StringTy = nullptr;
  llvm::StructType *BytesTy = nullptr;

  llvm::Function *Func_getCallDataSize = nullptr;
  llvm::Function *Func_callDataCopy = nullptr;
  llvm::Function *Func_log = nullptr;
  llvm::Function *Func_finish = nullptr;
  llvm::Function *Func_revert = nullptr;
  llvm::Function *Func_callStatic = nullptr;
  llvm::Function *Func_returnDataCopy = nullptr;
  llvm::Function *Func_print32 = nullptr;
  llvm::Function *Func_storageLoad = nullptr;
  llvm::Function *Func_storageStore = nullptr;
  llvm::Function *Func_getCaller = nullptr;
  llvm::Function *Func_getCallValue = nullptr;

  llvm::Function *Func_keccak256 = nullptr;

  llvm::Function *Func_bswap256 = nullptr;
  llvm::Function *Func_memcpy = nullptr;

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
    Int160Ty = Builder.getIntNTy(160);
    Int256Ty = Builder.getIntNTy(256);
    Int8PtrTy = Builder.getInt8PtrTy();
    Int32PtrTy = llvm::PointerType::getUnqual(Int32Ty);
    Int64PtrTy = llvm::PointerType::getUnqual(Int64Ty);
    Int128PtrTy = llvm::PointerType::getUnqual(Int128Ty);
    Int160PtrTy = llvm::PointerType::getUnqual(Int160Ty);
    Int256PtrTy = llvm::PointerType::getUnqual(Int256Ty);
    VoidTy = Builder.getVoidTy();

    BytesTy = llvm::StructType::create(M->getContext(), {Int32Ty, Int8PtrTy},
                                       "bytes");
    StringTy = llvm::StructType::create(M->getContext(), {Int256Ty, Int8PtrTy},
                                        "string", false);
  }

  void createEEIDeclaration() {
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = nullptr;
    llvm::Attribute Ethereum =
        llvm::Attribute::get(Context, "wasm-import-module", "ethereum");
    llvm::Attribute Debug =
        llvm::Attribute::get(Context, "wasm-import-module", "debug");

    // getCallDataSize
    FT = llvm::FunctionType::get(Int32Ty, {}, false);
    Func_getCallDataSize = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.getCallDataSize", *M);
    Func_getCallDataSize->addFnAttr(Ethereum);
    Func_getCallDataSize->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "getCallDataSize"));

    // callDataCopy
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
    Func_callDataCopy = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.callDataCopy", *M);
    Func_callDataCopy->addFnAttr(Ethereum);
    Func_callDataCopy->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "callDataCopy"));

    // log
    FT = llvm::FunctionType::get(VoidTy,
                                 {Int8PtrTy, Int32Ty, Int32Ty, Int256PtrTy,
                                  Int256PtrTy, Int256PtrTy, Int256PtrTy},
                                 false);
    Func_log = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                      "ethereum.log", *M);
    Func_log->addFnAttr(Ethereum);
    Func_log->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "log"));

    // finish
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
    Func_finish = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "ethereum.finish", *M);
    Func_finish->addFnAttr(Ethereum);
    Func_finish->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "finish"));

    // revert
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty}, false);
    Func_revert = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                         "ethereum.revert", *M);
    Func_revert->addFnAttr(Ethereum);
    Func_revert->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "revert"));

    // storageLoad
    FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy, Int256PtrTy}, false);
    Func_storageLoad = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.storageLoad", *M);
    Func_storageLoad->addFnAttr(Ethereum);
    Func_storageLoad->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "storageLoad"));

    // storageStore
    FT = llvm::FunctionType::get(VoidTy, {Int256PtrTy, Int256PtrTy}, false);
    Func_storageStore = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.storageStore", *M);
    Func_storageStore->addFnAttr(Ethereum);
    Func_storageStore->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "storageStore"));

    // getCaller
    FT = llvm::FunctionType::get(VoidTy, {Int160PtrTy}, false);
    Func_getCaller = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                            "ethereum.getCaller", *M);
    Func_getCaller->addFnAttr(Ethereum);
    Func_getCaller->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "getCaller"));

    // getCallValue
    FT = llvm::FunctionType::get(VoidTy, {Int128PtrTy}, false);
    Func_getCallValue = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.getCallValue", *M);
    Func_getCallValue->addFnAttr(Ethereum);
    Func_getCallValue->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "getCallValue"));

    // callStatic
    FT = llvm::FunctionType::get(
        Int32Ty, {Int64Ty, Int160PtrTy, Int8PtrTy, Int32Ty}, false);
    Func_callStatic = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.callStatic", *M);
    Func_callStatic->addFnAttr(Ethereum);
    Func_callStatic->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "callStatic"));

    // returnDataCopy
    FT = llvm::FunctionType::get(VoidTy, {Int8PtrTy, Int32Ty, Int32Ty}, false);
    Func_returnDataCopy = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "ethereum.returnDataCopy", *M);
    Func_returnDataCopy->addFnAttr(Ethereum);
    Func_returnDataCopy->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "returnDataCopy"));

    // debug.print32
    FT = llvm::FunctionType::get(VoidTy, {Int32Ty}, false);
    Func_print32 = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                                          "ethereum.print32", *M);
    Func_print32->addFnAttr(Debug);
    Func_print32->addFnAttr(
        llvm::Attribute::get(Context, "wasm-import-name", "print32"));
  }

  void createBswapI256() {
    auto &Builder = *IRBuilder;
    llvm::LLVMContext &Context = M->getContext();
    auto *const Arg = Func_bswap256->arg_begin();

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

  void createMemcpy() {
    auto &Builder = *IRBuilder;
    llvm::LLVMContext &Context = M->getContext();
    llvm::Argument *const Dst = Func_memcpy->arg_begin();
    llvm::Argument *const Src = Dst + 1;
    llvm::Argument *const Length = Src + 1;
    llvm::ConstantInt *const One = Builder.getInt32(1);
    Dst->setName("dst");
    Src->setName("src");
    Length->setName("length");

    llvm::BasicBlock *Entry =
        llvm::BasicBlock::Create(Context, "entry", Func_memcpy);
    llvm::BasicBlock *Loop =
        llvm::BasicBlock::Create(Context, "loop", Func_memcpy);
    llvm::BasicBlock *Return =
        llvm::BasicBlock::Create(Context, "return", Func_memcpy);

    Builder.SetInsertPoint(Entry);
    llvm::Value *Cmp = Builder.CreateICmpNE(Length, Builder.getInt32(0));
    Builder.CreateCondBr(Cmp, Loop, Return);

    Builder.SetInsertPoint(Loop);
    llvm::PHINode *SrcPHI = Builder.CreatePHI(Int8PtrTy, 2);
    llvm::PHINode *DstPHI = Builder.CreatePHI(Int8PtrTy, 2);
    llvm::PHINode *LengthPHI = Builder.CreatePHI(Int32Ty, 2);

    llvm::Value *value = Builder.CreateLoad(SrcPHI);
    Builder.CreateStore(value, DstPHI);
    llvm::Value *Src2 = Builder.CreateInBoundsGEP(SrcPHI, {One});
    llvm::Value *Dst2 = Builder.CreateInBoundsGEP(DstPHI, {One});
    llvm::Value *Length2 = Builder.CreateSub(LengthPHI, One);
    llvm::Value *Cmp2 = Builder.CreateICmpNE(Length2, Builder.getInt32(0));
    Builder.CreateCondBr(Cmp2, Loop, Return);

    Builder.SetInsertPoint(Return);
    Builder.CreateRet(Dst);

    SrcPHI->addIncoming(Src, Entry);
    SrcPHI->addIncoming(Src2, Loop);
    DstPHI->addIncoming(Dst, Entry);
    DstPHI->addIncoming(Dst2, Loop);
    LengthPHI->addIncoming(Length, Entry);
    LengthPHI->addIncoming(Length2, Loop);
  }

  void createI256Arithmetic() {
    llvm::LLVMContext &Context = M->getContext();

    Func_bswap256 = llvm::Function::Create(
        llvm::FunctionType::get(Int256Ty, {Int256Ty}, false),
        llvm::Function::InternalLinkage, "solidity.bswapi256", *M);

    Func_memcpy = llvm::Function::Create(
        llvm::FunctionType::get(Int8PtrTy, {Int8PtrTy, Int8PtrTy, Int32Ty},
                                false),
        llvm::Function::InternalLinkage, "solidity.memcpy", *M);

    createBswapI256();
    createMemcpy();
  }

  void createKeccak256() {
    auto &Builder = *IRBuilder;
    llvm::LLVMContext &Context = M->getContext();
    llvm::Argument *Memory = Func_keccak256->arg_begin();
    Memory->setName("memory");

    llvm::BasicBlock *Entry =
        llvm::BasicBlock::Create(Context, "entry", Func_keccak256);
    Builder.SetInsertPoint(Entry);

    llvm::Value *Length = Builder.CreateExtractValue(Memory, {0}, "length");
    llvm::Value *Ptr = Builder.CreateExtractValue(Memory, {1}, "ptr");

    llvm::ConstantInt *BaseFee = Builder.getInt64(30);
    llvm::ConstantInt *WordFee = Builder.getInt64(6);
    llvm::Value *PaddedLength = Builder.CreateLShr(
        Builder.CreateAdd(Builder.CreateZExtOrTrunc(Length, Int64Ty),
                          Builder.getInt64(31)),
        5);
    llvm::Value *Fee =
        Builder.CreateAdd(Builder.CreateMul(PaddedLength, WordFee), BaseFee);
    llvm::Value *AddressPtr =
        Builder.CreateAlloca(Int160Ty, nullptr, "address.ptr");
    Builder.CreateStore(Builder.getIntN(160, 9), AddressPtr);

    Builder.CreateCall(
        Func_callStatic,
        {Fee, AddressPtr, Ptr, Builder.CreateZExtOrTrunc(Length, Int32Ty)});
    llvm::Value *ResultPtr =
        Builder.CreateAlloca(Int256Ty, nullptr, "result.ptr");
    llvm::Value *ResultVPtr =
        Builder.CreateBitCast(ResultPtr, Int8PtrTy, "result.vptr");
    Builder.CreateCall(Func_returnDataCopy,
                       {ResultVPtr, Builder.getInt32(0), Builder.getInt32(32)});
    llvm::Value *Result = Builder.CreateLoad(ResultPtr);

    Builder.CreateRet(Result);
  }

  void createPrebuiltContract() {
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = nullptr;

    // keccak256
    FT = llvm::FunctionType::get(Int256Ty, {BytesTy}, false);
    Func_keccak256 = llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                            "solidity.keccak256", *M);

    createKeccak256();
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
    createPrebuiltContract();
  }

  void HandleSourceUnit(ASTContext &C, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(SourceUnitType &SU) override {
    for (auto Node : SU.getNodes())
      Node->accept(*this);
  }

  void visit(ContractDeclType &CD) override {
    auto &Builder = *IRBuilder;
    llvm::LLVMContext &Context = M->getContext();
    llvm::FunctionType *FT = llvm::FunctionType::get(VoidTy, {}, false);

    // event
    if (const auto &Es = CD.getEvents(); !Es.empty()) {
      for (auto E : Es) {
        std::vector<llvm::Constant *> Signature;
        for (auto s : E->getSignatureHash())
          Signature.push_back(Builder.getInt8(s));

        llvm::ArrayType *aty = llvm::ArrayType::get(Int8Ty, 32);
        llvm::Constant *init = llvm::ConstantArray::get(aty, Signature);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(
            *GetModule(), aty, true, llvm::GlobalVariable::InternalLinkage,
            init, "solidity.event");
        gv->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
        gv->setAlignment(1);
        Ctx->setEvent(E->getName(), gv);
      }
    }

    // constructor
    {
      llvm::GlobalVariable *deploy_size = new llvm::GlobalVariable(
          *GetModule(), Int32Ty, true, llvm::GlobalVariable::ExternalLinkage,
          nullptr, "deploy.size");
      deploy_size->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::Local);
      deploy_size->setAlignment(8);
      deploy_size->setVisibility(llvm::GlobalValue::HiddenVisibility);
      llvm::GlobalVariable *deploy_data = new llvm::GlobalVariable(
          *GetModule(), Int8Ty, true, llvm::GlobalVariable::ExternalLinkage,
          nullptr, "deploy.data");
      deploy_data->setAlignment(1);
      deploy_data->setVisibility(llvm::GlobalValue::HiddenVisibility);

      llvm::Function *Ctor = llvm::Function::Create(
          FT, llvm::Function::ExternalLinkage, "solidity.ctor", *M);
      Ctor->setVisibility(llvm::Function::VisibilityTypes::HiddenVisibility);
      Ctor->addFnAttr(
          llvm::Attribute::get(Context, llvm::Attribute::AlwaysInline));
      llvm::BasicBlock *Entry =
          llvm::BasicBlock::Create(Context, "entry", Ctor);
      Builder.SetInsertPoint(Entry);

      if (auto Constructor = CD.getConstructor()) {
        assert(Constructor->getParams()->getParams().empty() &&
               "parameters in constructor not supported!");
        assert(Constructor->getReturnParams()->getParams().empty() &&
               "no returns in constructor!");
        llvm::Function *Func =
            llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                   Constructor->getName(), *GetModule());

        Builder.CreateCall(Func, {});
      }

      Builder.CreateCall(Func_finish,
                         {deploy_data, Builder.CreateLoad(deploy_size)});
      Builder.CreateRetVoid();
    }

    // main
    llvm::Function *Main = llvm::Function::Create(
        FT, llvm::Function::ExternalLinkage, "solidity.main", *M);
    Main->setVisibility(llvm::Function::VisibilityTypes::HiddenVisibility);
    Main->addFnAttr(
        llvm::Attribute::get(Context, llvm::Attribute::AlwaysInline));

    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(Context, "entry", Main);
    llvm::BasicBlock *Error = llvm::BasicBlock::Create(Context, "error", Main);

    // two phase codegen
    Builder.SetInsertPoint(Error);
    if (auto Fallback = CD.getFallback()) {
      assert(Fallback->getParams()->getParams().empty() &&
             "not parameters in fallback functions!");
      assert(Fallback->getReturnParams()->getParams().empty() &&
             "no returns in fallback functions!");
      // fallback

      llvm::Function *Func =
          llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                                 Fallback->getName(), *GetModule());

      Builder.CreateCall(Func, {});
      Builder.CreateCall(
          Func_finish,
          {llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0)});
      Builder.CreateRetVoid();
    } else {
      Builder.CreateCall(
          Func_revert,
          {llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0)});
      Builder.CreateUnreachable();
    }

    if (const auto &Fs = CD.getFuncs(); !Fs.empty()) {
      llvm::BasicBlock *Switch =
          llvm::BasicBlock::Create(Context, "switch", Main);

      Builder.SetInsertPoint(Entry);
      llvm::Value *callDataSize =
          Builder.CreateCall(Func_getCallDataSize, {}, "size");
      llvm::Value *cmp =
          Builder.CreateICmpUGE(callDataSize, Builder.getInt32(4), "cmp");
      Builder.CreateCondBr(cmp, Switch, Error);

      Builder.SetInsertPoint(Switch);
      llvm::Value *HashVPtr =
          Builder.CreateAlloca(Int8Ty, Builder.getInt32(4), "hash.vptr");
      Builder.CreateCall(Func_callDataCopy,
                         {HashVPtr, Builder.getInt32(0), Builder.getInt32(4)});
      llvm::Value *HashPtr =
          Builder.CreateBitCast(HashVPtr, Int32PtrTy, "hash.ptr");
      llvm::Value *Hash = Builder.CreateLoad(Int32Ty, HashPtr, "hash");

      std::map<std::string, llvm::BasicBlock *> Labels;

      llvm::SwitchInst *SI =
          Builder.CreateSwitch(Hash, Error, CD.getFuncs().size());

      for (auto F : Fs) {
        llvm::BasicBlock *CondBB =
            llvm::BasicBlock::Create(Context, F->getName(), Main);
        Labels[F->getName()] = CondBB;
        std::uint32_t hash = funcSignatureHash(*F);
        SI->addCase(Builder.getInt32(hash), CondBB);
      }

      for (auto F : Fs) {
        genABI(*F, Labels[F->getName()], Error, callDataSize);
      }
    } else {
      Builder.SetInsertPoint(Entry);
      Builder.CreateBr(Error);
    }

    // codegen function body
    ConstDeclVisitor::visit(CD);
  }

  void visit(FunctionDeclType &F) override {
    FuncBodyCodeGen(M->getContext(), *IRBuilder, *GetModule(), *Ctx).compile(F);
  }

  size_t getABIMinSize(const std::vector<const VarDecl *> &params) const {
    size_t Result = 0;
    for (const auto &param : params) {
      const auto &Ty = *param->GetType();
      Result += Ty.getCalldataEncodedSize();
    }
    return Result;
  }

  llvm::Type *getStaticLLVMTy(const Type *Ty) {
    switch (Ty->getCategory()) {
    case Type::Category::Integer: {
      const IntegerType *IntTy = dynamic_cast<const IntegerType *>(Ty);
      return IRBuilder->getIntNTy(IntTy->getBitNum());
    }
    case Type::Category::Bool: {
      return Int1Ty;
    }
    case Type::Category::Address: {
      return IRBuilder->getIntNTy(160);
    }
    case Type::Category::String:
    case Type::Category::Bytes: {
      return Int256Ty;
    }
    default:
      assert(false && "unsupported type!");
    }
  }
  llvm::Type *getStaticLLVMTy(const VarDecl *VD) {
    return getStaticLLVMTy(VD->GetType().get());
  }

  llvm::Type *getLLVMTy(const Type *Ty) {
    switch (Ty->getCategory()) {
    case Type::Category::Integer: {
      const IntegerType *IntTy = dynamic_cast<const IntegerType *>(Ty);
      return IRBuilder->getIntNTy(IntTy->getBitNum());
    }
    case Type::Category::Bool: {
      return Int1Ty;
    }
    case Type::Category::Address: {
      return IRBuilder->getIntNTy(160);
    }
    case Type::Category::String: {
      return StringTy;
    }
    default:
      assert(false && "unsupported type!");
    }
  }
  llvm::Type *getLLVMTy(const VarDecl *VD) {
    return getLLVMTy(VD->GetType().get());
  }
  llvm::Type *getLLVMTy(const FunctionDeclType &F) {
    auto RetList = F.getReturnParams()->getParams();
    if (RetList.empty()) {
      return VoidTy;
    } else if (RetList.size() == 1) {
      return getLLVMTy(RetList.front()->GetType().get());
    } else {
      assert(false && "unsupported tuple return!");
    }
  }
  llvm::Value *castToTy(llvm::Value *Val, llvm::Type *TargetTy) {
    return IRBuilder->CreateTrunc(Val, TargetTy, "trunc");
  }
  llvm::Value *extTo256(llvm::Value *Val) {
    return IRBuilder->CreateZExt(Val, Int256Ty, "zext");
  }

  void genABI(FunctionDeclType &F, llvm::BasicBlock *Loader,
              llvm::BasicBlock *Error, llvm::Value *callDataSize) {
    auto &Builder = *IRBuilder;
    // TODO: refactor this
    // this impl. assumes all types are uint64
    const std::string &Fname = F.getName();
    const auto &Fparams = F.getParams()->getParams();

    Builder.SetInsertPoint(Loader);
    // get arguments from calldata
    const size_t ABIMinSize = getABIMinSize(Fparams);
    auto *arg_cptr = Builder.CreateAlloca(Int8Ty, Builder.getInt32(ABIMinSize),
                                          Fname + "_arg_cptr");
    auto *arg_vptr =
        Builder.CreateBitCast(arg_cptr, Int8PtrTy, Fname + "_arg_vptr");
    Builder.CreateCall(Func_callDataCopy, {arg_vptr, Builder.getInt32(4),
                                           Builder.getInt32(ABIMinSize)});

    unsigned offset = 0;
    std::vector<llvm::Value *> ArgsVal;
    std::vector<llvm::Type *> ArgsTy;
    std::vector<size_t> ArgsDynamic;
    for (size_t i = 0; i < Fparams.size(); i++) {
      Type &Ty = *Fparams[i]->GetType();
      switch (Ty.getCategory()) {
      case Type::Category::String:
      case Type::Category::Array:
        ArgsDynamic.push_back(i);
        break;
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::Integer:
        break;
      default:
        assert(false && "unsupported type!");
      }
      std::string param_name = (Fname + "_" + Fparams[i]->getName()).str();
      unsigned size = Fparams[i]->GetType()->getCalldataEncodedSize();
      auto *cptr = Builder.CreateInBoundsGEP(
          arg_cptr, {Builder.getInt32(offset)}, param_name + "_cptr");
      auto *ptr_b =
          Builder.CreateBitCast(cptr, Int256PtrTy, param_name + "_ptr_b");
      auto *val_b = Builder.CreateLoad(Int256Ty, ptr_b, param_name + "_b");
      auto *val = Builder.CreateCall(Func_bswap256, {val_b}, param_name);
      ArgsVal.push_back(castToTy(val, getStaticLLVMTy(Fparams[i])));
      ArgsTy.push_back(getLLVMTy(Fparams[i]));
      offset += size;
    }

    if (!ArgsDynamic.empty()) {
      llvm::BasicBlock *DynamicLoader = llvm::BasicBlock::Create(
          M->getContext(), F.getName() + ".dynamic", Loader->getParent());

      std::vector<llvm::Value *> ArgsOffset;
      llvm::Value *SizeCheck = Builder.getInt1(true);
      llvm::Value *DynamicSize = Builder.getInt32(0);
      for (size_t i : ArgsDynamic) {
        ArgsOffset.push_back(DynamicSize);
        SizeCheck = Builder.CreateAnd(
            SizeCheck, Builder.CreateICmpULE(
                           ArgsVal[i], Builder.getIntN(256, 0xFFFFFFFFU)));
        DynamicSize = Builder.CreateAdd(
            DynamicSize, Builder.CreateTrunc(ArgsVal[i], Int32Ty));
      }
      llvm::Value *ExceptedCallDataSize =
          Builder.CreateAdd(DynamicSize, Builder.getInt32(offset + 4));

      SizeCheck = Builder.CreateAnd(
          SizeCheck, Builder.CreateICmpEQ(ExceptedCallDataSize, callDataSize));
      Builder.CreateCondBr(SizeCheck, DynamicLoader, Error);

      Builder.SetInsertPoint(DynamicLoader);
      llvm::Value *arg_dyn_vptr =
          Builder.CreateAlloca(Int8Ty, DynamicSize, Fname + "_dyn_vptr");
      Builder.CreateCall(
          Func_callDataCopy,
          {arg_dyn_vptr, Builder.getInt32(offset + 4), DynamicSize});

      for (size_t i : ArgsDynamic) {
        switch (Fparams[i]->GetType()->getCategory()) {
        case Type::Category::String:
        case Type::Category::Array: {
          llvm::Value *string_data =
              Builder.CreateInBoundsGEP(arg_dyn_vptr, {ArgsOffset[i]});
          llvm::Value *string = llvm::ConstantAggregateZero::get(StringTy);
          string = Builder.CreateInsertValue(string, ArgsVal[i], {0});
          string = Builder.CreateInsertValue(string, string_data, {1});
          ArgsVal[i] = string;
          break;
        }
        default:
          __builtin_unreachable();
        }
      }
    }

    // Call this function
    llvm::FunctionType *FT =
        llvm::FunctionType::get(getLLVMTy(F), ArgsTy, false);
    llvm::Function *Func = llvm::Function::Create(
        FT, llvm::Function::InternalLinkage, F.getName(), *GetModule());
    const auto &Returns = F.getReturnParams()->getParams();
    if (Returns.empty()) {
      Builder.CreateCall(Func, ArgsVal);
      Builder.CreateCall(
          Func_finish,
          {llvm::ConstantPointerNull::get(Int8PtrTy), Builder.getInt32(0)});
    } else if (Returns.size() == 1) {
      switch (Returns.front()->GetType()->getCategory()) {
      case Type::Category::Address:
      case Type::Category::Bool:
      case Type::Category::Integer: {
        auto *r = Builder.CreateCall(Func, ArgsVal, Fname + "_r");
        // XXX: check signed
        auto *r_b = Builder.CreateCall(Func_bswap256,
                                       {Builder.CreateZExtOrTrunc(r, Int256Ty)},
                                       Fname + "_r_b");

        // put return value to returndata
        auto *r_ptr = Builder.CreateAlloca(Int256Ty, nullptr, Fname + "_r_ptr");
        Builder.CreateStore(r_b, r_ptr);
        auto *r_vptr = Builder.CreateBitCast(
            r_ptr, llvm::PointerType::getUnqual(Int8Ty), Fname + "_r_vptr");
        Builder.CreateCall(Func_finish, {r_vptr, Builder.getInt32(32)});
        break;
      }
      case Type::Category::String: {
        auto *r = Builder.CreateCall(Func, ArgsVal, Fname + "_r");
        auto *r_len = Builder.CreateExtractValue(r, {0}, Fname + "_r_len");
        auto *r_len_trunc =
            Builder.CreateTrunc(r_len, Int32Ty, Fname + "_r_len_trunc");
        auto *r_len_b =
            Builder.CreateCall(Func_bswap256, {r_len}, Fname + "_r_len_b");
        auto *r_data = Builder.CreateExtractValue(r, {1}, Fname + "_r_data");

        // put return value to returndata
        auto *r_size = Builder.CreateAdd(r_len_trunc, Builder.getInt32(32));
        auto *r_ptr = Builder.CreateAlloca(Int8Ty, r_size, Fname + "_r_ptr");
        auto *r_len_vptr =
            Builder.CreateInBoundsGEP(r_ptr, {Builder.getInt32(0)});
        auto *r_len_ptr = Builder.CreateBitCast(r_len_vptr, Int256PtrTy,
                                                Fname + "_r_len_ptr");
        Builder.CreateStore(r_len_b, r_len_ptr);
        auto *r_data_ptr = Builder.CreateInBoundsGEP(
            r_ptr, Builder.getInt32(32), Fname + "_r_data_ptr");

        Builder.CreateStore(r_len_b, r_len_ptr);

        Builder.CreateCall(Func_memcpy, {r_data_ptr, r_data, r_len_trunc});

        auto *r_vptr = Builder.CreateBitCast(
            r_ptr, llvm::PointerType::getUnqual(Int8Ty), Fname + "_r_vptr");
        Builder.CreateCall(Func_finish,
                           {r_vptr, Builder.CreateTrunc(r_size, Int32Ty)});
        break;
      }
      default:
        assert(false && "unsupported type!");
      }

    } else {
      assert(false && "unsupported tuple return!");
    }

    Builder.CreateRetVoid();
  }

  std::uint32_t funcSignatureHash(const FunctionDecl &F) {
    const std::vector<unsigned char> op = F.getSignatureHash();
    return *reinterpret_cast<const std::uint32_t *>(op.data());
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
