// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "ABICodec.h"
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include "ExprEmitter.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticCodeGen.h"

namespace soll::CodeGen {

ExprValuePtr CodeGenFunction::emitExpr(const Expr *E) {
  return ExprEmitter(*this).visit(E);
}

void CodeGenFunction::emitCallRequire(const CallExpr *CE) {
  // require function
  auto Arguments = CE->getArguments();
  assert(Arguments.size() == 1 || Arguments.size() == 2);

  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");

  emitBranchOnBoolExpr(Arguments[0], Continue, Revert);

  Builder.SetInsertPoint(Revert);
  if (Arguments.size() == 2) {
    llvm::Value *Message = emitExpr(Arguments[1])->load(Builder, CGM);
    CGM.emitRevert(
        Builder.CreateExtractValue(Message, {1}),
        Builder.CreateTrunc(Builder.CreateExtractValue(Message, {0}), Int32Ty));
  }
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
}

void CodeGenFunction::emitCallAssert(const CallExpr *CE) {
  // assert function
  auto Arguments = CE->getArguments();
  assert(Arguments.size() == 1);

  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");

  emitBranchOnBoolExpr(Arguments[0], Continue, Revert);

  Builder.SetInsertPoint(Revert);
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
}

void CodeGenFunction::emitCallRevert(const CallExpr *CE) {
  // revert function
  auto Arguments = CE->getArguments();
  assert(Arguments.size() == 0 || Arguments.size() == 1);

  if (Arguments.size() == 1) {
    llvm::Value *Message = emitExpr(Arguments[0])->load(Builder, CGM);
    CGM.emitRevert(
        Builder.CreateExtractValue(Message, {1}),
        Builder.CreateTrunc(Builder.CreateExtractValue(Message, {0}), Int32Ty));
  }
  Builder.CreateUnreachable();
  Builder.SetInsertPoint(createBasicBlock("after.revert"));
}

llvm::Value *CodeGenFunction::emitCallAddmod(const CallExpr *CE) {
  // addmod
  auto Arguments = CE->getArguments();
  llvm::Value *A = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *B = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *K = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *AddAB = Builder.CreateAdd(A, B);
  llvm::Value *Res = Builder.CreateURem(AddAB, K);
  return Res;
}

llvm::Value *CodeGenFunction::emitCallMulmod(const CallExpr *CE) {
  // mulmod
  auto Arguments = CE->getArguments();
  llvm::Value *A = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *B = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *K = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *MulAB = Builder.CreateMul(A, B);
  llvm::Value *Res = Builder.CreateURem(MulAB, K);
  return Res;
}

llvm::Value *CodeGenFunction::emitCallKeccak256(const CallExpr *CE) {
  // keccak256 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0])->load(Builder, CGM);
  return CGM.emitKeccak256(MessageValue);
}

llvm::Value *CodeGenFunction::emitCallSha256(const CallExpr *CE) {
  // sha256 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0])->load(Builder, CGM);
  return CGM.emitSha256(MessageValue);
}

llvm::Value *CodeGenFunction::emitCallRipemd160(const CallExpr *CE) {
  // ripemd160 function
  auto Arguments = CE->getArguments();
  llvm::Value *MessageValue = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Val = Builder.CreateCall(
      CGM.getModule().getFunction("solidity.ripemd160"), {MessageValue});
  return Builder.CreateTrunc(Val, Int160Ty);
}

llvm::Value *CodeGenFunction::emitCallEcrecover(const CallExpr *CE) {
  // ecrecover function
  auto Arguments = CE->getArguments();
  llvm::Value *HashValue = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *RecoveryIdValue =
      Builder.CreateZExt(emitExpr(Arguments[1])->load(Builder, CGM), Int256Ty);
  llvm::Value *RValue = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *SValue = emitExpr(Arguments[3])->load(Builder, CGM);
  llvm::Value *Val =
      Builder.CreateCall(CGM.getModule().getFunction("solidity.ecrecover"),
                         {HashValue, RecoveryIdValue, RValue, SValue});
  return Builder.CreateTrunc(Val, AddressTy);
}

llvm::Value *CodeGenFunction::emitCallBlockHash(const CallExpr *CE) {
  // blockhash function
  auto Arguments = CE->getArguments();
  llvm::Value *NumberValue = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Val = CGM.getEndianlessValue(CGM.emitGetBlockHash(NumberValue));
  return Val;
}

ExprValuePtr CodeGenFunction::emitCallAddressStaticcall(const CallExpr *CE,
                                                        const MemberExpr *ME) {
  // address_staticcall function
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(ME->getBase())->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(CGM.getEndianlessValue(Address), AddressPtr);
  llvm::Value *MemoryValue = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(MemoryValue, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(MemoryValue, {1}, "ptr");
  llvm::Value *Gas = CGM.emitGetGasLeft();

  llvm::Value *Val = CGM.emitCallStatic(Gas, AddressPtr, Ptr, Length);

  using namespace std::string_literals;
  static const std::string Message = "StaticCall Fail"s;
  llvm::Constant *MessageValue =
      createGlobalStringPtr(getLLVMContext(), CGM.getModule(), Message);

  llvm::Value *Cond = Builder.CreateICmpNE(Val, Builder.getInt32(2));
  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");
  Builder.CreateCondBr(Cond, Continue, Revert);

  Builder.SetInsertPoint(Revert);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();

  Builder.SetInsertPoint(Continue);
  llvm::Value *ReturnDataSize = CGM.emitReturnDataSize();
  llvm::Value *ReturnData =
      CGM.emitReturnDataCopyBytes(Builder.getInt32(0), ReturnDataSize);

  auto TupleTy = TupleType(std::vector<TypePtr>{std::make_shared<BooleanType>(),
                                                std::make_shared<BytesType>()});
  return ExprValueTuple::getRValue(
      &TupleTy,
      std::vector<llvm::Value *>{
          Builder.CreateNot(Builder.CreateTrunc(Val, BoolTy)), ReturnData});
}

ExprValuePtr
CodeGenFunction::emitCallAddressDelegatecall(const CallExpr *CE,
                                             const MemberExpr *ME) {
  // address_staticcall function
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(ME->getBase())->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(CGM.getEndianlessValue(Address), AddressPtr);
  llvm::Value *MemoryValue = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(MemoryValue, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(MemoryValue, {1}, "ptr");
  llvm::Value *Gas = CGM.emitGetGasLeft();

  llvm::Value *Val = CGM.emitCallDelegate(Gas, AddressPtr, Ptr, Length);

  llvm::Value *Cond;
  if (CGM.isEWASM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(0));
  } else if (CGM.isEVM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(1));
  } else {
    __builtin_unreachable();
  }

  llvm::Value *ReturnDataSize = CGM.emitReturnDataSize();
  llvm::Value *ReturnData =
      CGM.emitReturnDataCopyBytes(Builder.getInt32(0), ReturnDataSize);

  auto TupleTy = TupleType(std::vector<TypePtr>{std::make_shared<BooleanType>(),
                                                std::make_shared<BytesType>()});
  return ExprValueTuple::getRValue(
      &TupleTy, std::vector<llvm::Value *>{Builder.CreateTrunc(Cond, BoolTy),
                                           ReturnData});
}

ExprValuePtr CodeGenFunction::emitCallAddressCall(const CallExpr *CE,
                                                  const MemberExpr *ME) {
  // address_call function
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(ME->getBase())->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(CGM.getEndianlessValue(Address), AddressPtr);
  if (Arguments.empty()) {
    CGM.getDiags().Report(CE->getLocation().getBegin(),
                          diag::err_address_call_without_payload);
    assert(false && "address.call() should have a payload parameter.");
  }
  llvm::Value *MemoryValue = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Length = Builder.CreateTrunc(
      Builder.CreateExtractValue(MemoryValue, {0}), Int32Ty, "length");
  llvm::Value *Ptr = Builder.CreateExtractValue(MemoryValue, {1}, "ptr");
  llvm::Value *Gas = CGM.emitGetGasLeft();
  llvm::Value *ValuePtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(Builder.getIntN(128, 0), ValuePtr);

  llvm::Value *Val = CGM.emitCall(Gas, AddressPtr, ValuePtr, Ptr, Length);

  llvm::Value *Cond;
  if (CGM.isEWASM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(0));
  } else if (CGM.isEVM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(1));
  } else {
    __builtin_unreachable();
  }

  llvm::Value *ReturnDataSize = CGM.emitReturnDataSize();
  llvm::Value *ReturnData =
      CGM.emitReturnDataCopyBytes(Builder.getInt32(0), ReturnDataSize);

  auto TupleTy = TupleType(std::vector<TypePtr>{std::make_shared<BooleanType>(),
                                                std::make_shared<BytesType>()});
  return ExprValueTuple::getRValue(
      &TupleTy, std::vector<llvm::Value *>{Builder.CreateTrunc(Cond, BoolTy),
                                           ReturnData});
}

ExprValuePtr CodeGenFunction::emitLibraryCall(const CallExpr *CE,
                                              const MemberExpr *ME) {
  auto VarT =
      dynamic_cast<ExprValueTuple *>(emitCallAddressDelegatecall(CE, ME).get());
  auto Bytes = VarT->getValues().at(1);
  return emitAbiDecode(Bytes->load(Builder, CGM), CE->getType().get());
}

ExprValuePtr CodeGenFunction::emitExternalCall(const CallExpr *CE,
                                               const MemberExpr *ME) {
  auto VarT = dynamic_cast<ExprValueTuple *>(emitCallAddressCall(CE, ME).get());
  auto Bytes = VarT->getValues().at(1);
  return emitAbiDecode(Bytes->load(Builder, CGM), CE->getType().get());
}

llvm::Value *CodeGenFunction::emitCallAddressSend(const CallExpr *CE,
                                                  const MemberExpr *ME,
                                                  bool NeedRevert) {
  // address_call function
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(ME->getBase())->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(CGM.getEndianlessValue(Address), AddressPtr);
  llvm::Value *Amount = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *AmountPtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(Builder.CreateZExtOrTrunc(Amount, Int128Ty), AmountPtr);

  llvm::Value *Length = Builder.getInt32(0);
  llvm::Value *Ptr = Builder.CreateAlloca(Int8Ty, Length);
  llvm::Value *Gas = Builder.getInt64(0);

  llvm::Value *Val = CGM.emitCall(Gas, AddressPtr, AmountPtr, Ptr, Length);

  llvm::Value *Cond;
  if (CGM.isEWASM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(0));
  } else if (CGM.isEVM()) {
    Cond = Builder.CreateICmpEQ(Val, Builder.getInt32(1));
  } else {
    __builtin_unreachable();
  }

  if (!NeedRevert) {
    return Builder.CreateTrunc(Cond, BoolTy);
  }

  using namespace std::string_literals;
  static const std::string Message = "Transfer Fail"s;
  llvm::Constant *MessageValue =
      createGlobalStringPtr(getLLVMContext(), CGM.getModule(), Message);

  llvm::BasicBlock *Continue = createBasicBlock("continue");
  llvm::BasicBlock *Revert = createBasicBlock("revert");
  Builder.CreateCondBr(Cond, Continue, Revert);

  Builder.SetInsertPoint(Revert);
  CGM.emitRevert(MessageValue, Builder.getInt32(Message.size()));
  Builder.CreateUnreachable();
  Builder.SetInsertPoint(Continue);
  return nullptr;
}

llvm::Value *CodeGenFunction::emitAbiEncodePacked(const CallExpr *CE) {
  // abi_encodePacked
  auto Arguments = CE->getArguments();
  std::vector<std::pair<ExprValuePtr, bool>> Args;
  for (auto Arg : Arguments) {
    if (auto CastExprPtr = dynamic_cast<const CastExpr *>(Arg))
      Arg = CastExprPtr->getSubExpr();
    bool IsStateVariable = Arg->isStateVariable(); // for array index access
    Args.emplace_back(emitExpr(Arg), IsStateVariable);
  }
  AbiEmitter Emitter(*this);
  llvm::Value *ArrayLength = Emitter.getEncodePackedTupleSize(Args);
  // TODO: use memory allocate and memset
  llvm::Value *Array =
      Builder.CreateAlloca(Int8Ty, ArrayLength, "encodePacked");
  Emitter.emitEncodePackedTuple(Array, Args);
  llvm::Value *Bytes = llvm::ConstantAggregateZero::get(BytesTy);
  Bytes = Builder.CreateInsertValue(
      Bytes, Builder.CreateZExtOrTrunc(ArrayLength, Int256Ty), {0});
  Bytes = Builder.CreateInsertValue(Bytes, Array, {1});
  return Bytes;
}

llvm::Value *CodeGenFunction::emitAbiEncode(const CallExpr *CE) {
  // abi_encode
  auto Arguments = CE->getArguments();
  std::vector<std::pair<ExprValuePtr, bool>> Args;
  for (auto Arg : Arguments) {
    if (auto CastExprPtr = dynamic_cast<const CastExpr *>(Arg))
      Arg = CastExprPtr->getSubExpr();
    bool IsStateVariable = Arg->isStateVariable(); // for array index access
    Args.emplace_back(emitExpr(Arg), IsStateVariable);
  }
  AbiEmitter Emitter(*this);
  llvm::Value *ArrayLength = Emitter.getEncodeTupleSize(Args);
  // TODO: use memory allocate and memset
  llvm::Value *Array = Builder.CreateAlloca(Int8Ty, ArrayLength, "encode");
  Emitter.emitEncodeTuple(Array, Args);
  llvm::Value *Bytes = llvm::ConstantAggregateZero::get(BytesTy);
  Bytes = Builder.CreateInsertValue(
      Bytes, Builder.CreateZExtOrTrunc(ArrayLength, Int256Ty), {0});
  Bytes = Builder.CreateInsertValue(Bytes, Array, {1});
  return Bytes;
}

ExprValuePtr CodeGenFunction::emitAbiDecode(const CallExpr *CE) {
  // abi_decode
  auto Arguments = CE->getArguments();
  assert(Arguments.size() == 2);
  auto Arg = Arguments.at(0);
  if (auto CastExprPtr = dynamic_cast<const CastExpr *>(Arg))
    Arg = CastExprPtr->getSubExpr();
  auto BytesExprValue = emitExpr(Arg);
  auto Bytes = BytesExprValue->load(Builder, CGM);

  return emitAbiDecode(Bytes, CE->getType().get());
}

ExprValuePtr CodeGenFunction::emitAbiDecode(llvm::Value *Bytes,
                                            const Type *TupleTy) {
  llvm::Value *Length = Builder.CreateZExtOrTrunc(
      Builder.CreateExtractValue(Bytes, {0}), CGM.Int32Ty);
  llvm::Value *SrcBytes = Builder.CreateExtractValue(Bytes, {1});

  AbiEmitter Emitter(*this);
  auto ReturnValue = Emitter.getDecode(SrcBytes, TupleTy).first;
  if (auto TP = dynamic_cast<ExprValueTuple *>(ReturnValue.get())) {
    const auto &Values = TP->getValues();
    if (Values.size() == 1)
      ReturnValue = Values.front();
  }
  return ReturnValue;
}

ExprValuePtr CodeGenFunction::emitCallExpr(const CallExpr *CE) {
  auto Expr = CE->getCalleeExpr();
  auto ME = dynamic_cast<const MemberExpr *>(Expr);
  if (ME) {
    Expr = ME->getName();
  }
  const Decl *D = nullptr;
  if (auto *Callee = dynamic_cast<const Identifier *>(Expr)) {
    if (Callee->isSpecialIdentifier()) {
      return emitSpecialCallExpr(Callee, CE, ME);
    } else {
      D = Callee->getCorrespondDecl();
    }
  } else if (auto *Callee = dynamic_cast<const AsmIdentifier *>(Expr)) {
    if (Callee->isSpecialIdentifier()) {
      return emitAsmSpecialCallExpr(Callee, CE);
    } else {
      D = Callee->getCorrespondDecl();
    }
  } else {
    __builtin_unreachable();
  }
  std::vector<llvm::Value *> Args;
  for (auto Argument : CE->getArguments()) {
    Args.push_back(emitExpr(Argument)->load(Builder, CGM));
  }
  if (auto FD = dynamic_cast<const FunctionDecl *>(D)) {
    llvm::Function *F = CGM.createOrGetLLVMFunction(FD);
    assert(F != nullptr && "undefined function");
    llvm::Value *Result = Builder.CreateCall(F, Args);
    return ExprValue::getRValue(CE, Result);
  }
  if (auto ED = dynamic_cast<const EventDecl *>(D)) {
    auto Params = ED->getParams()->getParams();
    auto Arguments = CE->getArguments();
    std::vector<llvm::Value *> Data(
        1, llvm::ConstantPointerNull::get(Int256PtrTy));
    std::vector<llvm::Value *> Topics;
    uint DataCnt = 0;

    if (ED->isAnonymous() == false) {
      Topics.emplace_back(
          Builder.CreateBitCast(CGM.getModule().getNamedGlobal(
                                    "solidity.event." + CGM.getMangledName(ED)),
                                Int256PtrTy));
    }

    // XXX: Multiple args and complex data type encoding not implemented yet.
    for (size_t I = 0; I < Arguments.size(); I++) {
      llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
      Builder.CreateStore(
          CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(Args[I], Int256Ty)),
          ValPtr);
      if (dynamic_cast<const VarDecl *>(Params[I])->isIndexed()) {
        Topics.emplace_back(ValPtr);
      } else {
        Data[DataCnt++] = ValPtr;
      }
    }
    CGM.emitLog(Data[0], Builder.getInt32(32), Topics);
    return std::make_shared<ExprValue>();
  }
  if (auto AFD = dynamic_cast<const AsmFunctionDecl *>(D)) {
    llvm::Function *F = CGM.createOrGetLLVMFunction(AFD);
    assert(F != nullptr && "undefined function");
    llvm::Value *Result = Builder.CreateCall(F, Args);
    return ExprValue::getRValue(CE, Result);
  }
  assert(false && "Unhandle CallExprType CodeGen case.");
  __builtin_unreachable();
}

llvm::Value *CodeGenFunction::emitStructConstructor(const CallExpr *CE) {
  auto Callee = CE->getCalleeExpr();
  TypePtr ReturnType;
  if (auto funTy =
          dynamic_cast<const FunctionType *>(Callee->getType().get())) {
    ReturnType = funTy->getReturnTypes()[0];
  } else {
    assert(false && "Can not find struct constructor.");
    __builtin_unreachable();
  }
  const auto &Arguments = CE->getArguments();
  auto ReturnStructType = dynamic_cast<const StructType *>(ReturnType.get());
  llvm::Value *ReturnStruct =
      llvm::ConstantAggregateZero::get(ReturnStructType->getLLVMType());
  if (Arguments.size() != ReturnStructType->getElementSize()) {
    assert(false && "Wrong argument count for struct constructor.");
    __builtin_unreachable();
  }
  unsigned Index = 0;
  for (auto Argument : Arguments) {
    ReturnStruct = Builder.CreateInsertValue(
        ReturnStruct, emitExpr(Argument)->load(Builder, CGM), {Index++});
  }
  return ReturnStruct;
}

ExprValuePtr CodeGenFunction::emitSpecialCallExpr(const Identifier *SI,
                                                  const CallExpr *CE,
                                                  const MemberExpr *ME) {
  switch (SI->getSpecialIdentifier()) {
  case Identifier::SpecialIdentifier::require:
    emitCallRequire(CE);
    return std::make_shared<ExprValue>();
  case Identifier::SpecialIdentifier::assert_:
    emitCallAssert(CE);
    return std::make_shared<ExprValue>();
  case Identifier::SpecialIdentifier::revert:
    emitCallRevert(CE);
    return std::make_shared<ExprValue>();
  case Identifier::SpecialIdentifier::gasleft:
    assert(CE->getArguments().empty() && "gasleft require no arguments");
    return ExprValue::getRValue(CE, CGM.emitGetGasLeft());
  case Identifier::SpecialIdentifier::addmod:
    return ExprValue::getRValue(CE, emitCallAddmod(CE));
  case Identifier::SpecialIdentifier::mulmod:
    return ExprValue::getRValue(CE, emitCallMulmod(CE));
  case Identifier::SpecialIdentifier::keccak256:
    return ExprValue::getRValue(CE, emitCallKeccak256(CE));
  case Identifier::SpecialIdentifier::sha256:
    return ExprValue::getRValue(CE, emitCallSha256(CE));
  case Identifier::SpecialIdentifier::ripemd160:
    return ExprValue::getRValue(CE, emitCallRipemd160(CE));
  case Identifier::SpecialIdentifier::ecrecover:
    return ExprValue::getRValue(CE, emitCallEcrecover(CE));
  case Identifier::SpecialIdentifier::blockhash:
    return ExprValue::getRValue(CE, emitCallBlockHash(CE));
  case Identifier::SpecialIdentifier::address_staticcall:
    return emitCallAddressStaticcall(CE, ME);
  case Identifier::SpecialIdentifier::library_call:
    return emitLibraryCall(CE, ME);
  case Identifier::SpecialIdentifier::address_delegatecall:
    return emitCallAddressDelegatecall(CE, ME);
  case Identifier::SpecialIdentifier::external_call:
    return emitExternalCall(CE, ME);
  case Identifier::SpecialIdentifier::address_call:
    return emitCallAddressCall(CE, ME);
  case Identifier::SpecialIdentifier::address_transfer:
    emitCallAddressSend(CE, ME, true);
    return std::make_shared<ExprValue>();
  case Identifier::SpecialIdentifier::address_send:
    return ExprValue::getRValue(CE, emitCallAddressSend(CE, ME, false));
  case Identifier::SpecialIdentifier::abi_encode: {
    return ExprValue::getRValue(CE, emitAbiEncode(CE));
  }
  case Identifier::SpecialIdentifier::abi_encodePacked:
  case Identifier::SpecialIdentifier::abi_encodeWithSelector:
  case Identifier::SpecialIdentifier::abi_encodeWithSignature: {
    return ExprValue::getRValue(CE, emitAbiEncodePacked(CE));
  }
  case Identifier::SpecialIdentifier::abi_decode: {
    return emitAbiDecode(CE);
  }
  case Identifier::SpecialIdentifier::struct_constructor:
    return ExprValue::getRValue(CE, emitStructConstructor(CE));
  default:
    assert(false && "special function not supported yet");
    __builtin_unreachable();
  }
}

llvm::Value *CodeGenFunction::emitAsmCallDataSize(const CallExpr *CE) {
  if (auto *ICE =
          dynamic_cast<const ImplicitCastExpr *>(CE->getArguments()[0])) {
    if (auto *SL = dynamic_cast<const StringLiteral *>(ICE->getSubExpr())) {
      std::string Name = SL->getValue();
      auto ObjectOrData = CGM.lookupYulDataOrYulObject(Name);
      return std::visit(
          [this, &Name](auto Ptr) -> llvm::Value * {
            if constexpr (std::is_same_v<decltype(Ptr), const YulData *>) {
              llvm::GlobalVariable *Data = CGM.getYulDataAddr(Ptr);
              auto Size = Data->getType()
                              ->getPointerElementType()
                              ->getArrayNumElements();
              return Builder.getIntN(256, Size);
            } else if constexpr (std::is_same_v<decltype(Ptr),
                                                const YulObject *>) {
              llvm::Function *Object =
                  CGM.getModule().getFunction(Name + ".object");
              auto *ObjectBytes = Builder.CreateCall(Object);
              return Builder.CreateExtractValue(ObjectBytes, {0});
            } else {
              assert(false && "datasize needs a valid data name");
              return Builder.getIntN(256, 0);
            }
          },
          ObjectOrData);
    }
  }
  assert(false && "dataoffset() needs a string literal");
  __builtin_unreachable();
}

llvm::Value *CodeGenFunction::emitAsmCallDataOffset(const CallExpr *CE) {
  if (auto *ICE =
          dynamic_cast<const ImplicitCastExpr *>(CE->getArguments()[0])) {
    if (auto *SL = dynamic_cast<const StringLiteral *>(ICE->getSubExpr())) {
      std::string Name = SL->getValue();
      auto ObjectOrData = CGM.lookupYulDataOrYulObject(Name);
      return std::visit(
          [this, &Name](auto Ptr) -> llvm::Value * {
            if constexpr (std::is_same_v<decltype(Ptr), const YulData *>) {
              llvm::GlobalVariable *Data = CGM.getYulDataAddr(Ptr);
              return Builder.CreatePtrToInt(Data, Int256Ty);
            } else if constexpr (std::is_same_v<decltype(Ptr),
                                                const YulObject *>) {
              llvm::Function *Object =
                  CGM.getModule().getFunction(Name + ".object");
              auto *ObjectBytes = Builder.CreateCall(Object);
              auto *Data = Builder.CreateExtractValue(ObjectBytes, {1});
              return Builder.CreatePtrToInt(Data, Int256Ty);
            } else {
              assert(false && "dataoffset needs a valid data name");
              return Builder.getIntN(256, 0);
            }
          },
          ObjectOrData);
    }
  }
  assert(false && "dataoffset() needs a string literal");
  __builtin_unreachable();
}

llvm::Value *CodeGenFunction::emitAsmGetBalance(const CallExpr *CE,
                                                bool isSelf) {
  auto Arguments = CE->getArguments();
  llvm::Value *Address;
  if (isSelf) {
    Address = Builder.CreateZExtOrTrunc(CGM.emitGetAddress(), CGM.Int256Ty);
  } else {
    Address = emitExpr(Arguments[0])->load(Builder, CGM);
  }
  return CGM.emitGetExternalBalance(Address);
}

llvm::Value *CodeGenFunction::emitAsmCallMLoad(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *CPtr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *Ptr = Builder.CreateBitCast(CPtr, Int256PtrTy, "heap.ptr");
  llvm::Value *Value = CGM.getEndianlessValue(Builder.CreateLoad(Ptr));
  CGM.emitUpdateMemorySize(Pos, Builder.getIntN(256, 32));
  return Value;
}

void CodeGenFunction::emitAsmCallMStore(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *CPtr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *Ptr = Builder.CreateBitCast(CPtr, Int256PtrTy, "heap.ptr");
  Builder.CreateStore(
      CGM.getEndianlessValue(emitExpr(Arguments[1])->load(Builder, CGM)), Ptr);
  CGM.emitUpdateMemorySize(Pos, Builder.getIntN(256, 32));
}

void CodeGenFunction::emitAsmCallMStore8(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.ptr");
  llvm::Value *Value = Builder.CreateZExtOrTrunc(
      emitExpr(Arguments[1])->load(Builder, CGM), CGM.Int8Ty);
  Builder.CreateStore(Value, Ptr);
  CGM.emitUpdateMemorySize(Pos, Builder.getIntN(256, 1));
}

llvm::Value *CodeGenFunction::emitAsmCallMSize(const CallExpr *CE) {
  return Builder.CreateLoad(CGM.getMemorySize());
}

llvm::Value *CodeGenFunction::emitAsmCallSLoad(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  auto Val =
      CGM.emitStorageLoad(CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
          emitExpr(Arguments[0])->load(Builder, CGM), CGM.Int256Ty)));
  return CGM.getEndianlessValue(Val);
}

void CodeGenFunction::emitAsmCallSStore(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  CGM.emitStorageStore(
      CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
          emitExpr(Arguments[0])->load(Builder, CGM), CGM.Int256Ty)),
      CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
          emitExpr(Arguments[1])->load(Builder, CGM), CGM.Int256Ty)));
}

void CodeGenFunction::emitAsmCallReturn(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *Length = emitExpr(Arguments[1])->load(Builder, CGM);
  CGM.emitUpdateMemorySize(Pos, Length);
  CGM.emitFinish(Ptr, Builder.CreateZExtOrTrunc(Length, CGM.Int32Ty));
}

void CodeGenFunction::emitAsmCallRevert(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *Length = emitExpr(Arguments[1])->load(Builder, CGM);
  CGM.emitUpdateMemorySize(Pos, Length);
  CGM.emitRevert(Ptr, Builder.CreateZExtOrTrunc(Length, CGM.Int32Ty));
}

void CodeGenFunction::emitAsmSelfDestruct(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(Arguments[0])->load(Builder, CGM);
  CGM.emitSelfDestruct(Address);
}

void CodeGenFunction::emitAsmCallLog(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Data =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *DataLength = emitExpr(Arguments[1])->load(Builder, CGM);
  std::vector<llvm::Value *> Topics;
  for (size_t I = 2; I < Arguments.size(); I++) {
    llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
    Builder.CreateStore(
        CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
            emitExpr(Arguments[I])->load(Builder, CGM), CGM.Int256Ty)),
        ValPtr);
    Topics.emplace_back(ValPtr);
  }
  CGM.emitUpdateMemorySize(Pos, DataLength);
  CGM.emitLog(Data, Builder.CreateZExtOrTrunc(DataLength, CGM.Int32Ty), Topics);
}

llvm::Value *CodeGenFunction::emitAsmCallCallDataLoad(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  return CGM.getEndianlessValue(
      CGM.emitCallDataLoad(emitExpr(Arguments[0])->load(Builder, CGM)));
}

void CodeGenFunction::emitAsmCallCodeCopy(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.ptr");
  llvm::Value *Code = Builder.CreateIntToPtr(
      emitExpr(Arguments[1])->load(Builder, CGM), Int8PtrTy);
  llvm::Value *Length = emitExpr(Arguments[2])->load(Builder, CGM);
  CGM.emitUpdateMemorySize(Pos, Length);
  CGM.emitMemcpy(Ptr, Code, Builder.CreateZExtOrTrunc(Length, CGM.Int32Ty));
}

void CodeGenFunction::emitAsmCallDataCopy(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Code = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[2])->load(Builder, CGM);
  CGM.emitCallDataCopy(Pos, Code, Length);
}

void CodeGenFunction::emitAsmExternalCallCodeCopy(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Result = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *Code = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[3])->load(Builder, CGM);
  CGM.emitExternalCodeCopy(Address, Result, Code, Length);
}

llvm::Value *CodeGenFunction::emitAsmExternalCallCodeHash(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Address = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
  Builder.CreateStore(
      CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(Address, CGM.Int256Ty)),
      ValPtr);
  ValPtr = Builder.CreateBitCast(ValPtr, CGM.Int32PtrTy);
  llvm::Value *Length = Builder.CreateZExtOrTrunc(
      CGM.emitGetExternalCodeSize(ValPtr), CGM.Int256Ty);

  llvm::Value *CodePtr = Builder.CreateAlloca(Int8Ty, Length);
  CGM.emitUpdateMemorySize(Builder.CreatePtrToInt(CodePtr, CGM.Int256Ty),
                           Length);
  CGM.emitExternalCodeCopy(Address,
                           Builder.CreatePtrToInt(CodePtr, CGM.Int256Ty),
                           Builder.getIntN(256, 0), Length);
  return CGM.emitKeccak256(CodePtr, Length);
}

llvm::Value *CodeGenFunction::emitAsmExternalGetCodeSize(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *ValPtr = Builder.CreateAlloca(Int256Ty, nullptr);
  Builder.CreateStore(
      CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
          emitExpr(Arguments[0])->load(Builder, CGM), CGM.Int256Ty)),
      ValPtr);
  ValPtr = Builder.CreateBitCast(ValPtr, CGM.Int32PtrTy);
  return CGM.emitGetExternalCodeSize(ValPtr);
}

llvm::Value *CodeGenFunction::emitAsmCallkeccak256(const CallExpr *CE) {
  // keccak256 function
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr =
      Builder.CreateInBoundsGEP(CGM.getHeapBase(), {Pos}, "heap.cptr");
  llvm::Value *Length = emitExpr(Arguments[1])->load(Builder, CGM);
  CGM.emitUpdateMemorySize(Pos, Length);
  return CGM.emitKeccak256(Ptr, Length);
}

llvm::Value *CodeGenFunction::emitAsmGetBlockHash(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Number = emitExpr(Arguments[0])->load(Builder, CGM);
  return CGM.emitGetBlockHash(Number);
}

void CodeGenFunction::emitAsmReturnDataCopy(const CallExpr *CE) {
  auto Arguments = CE->getArguments();
  llvm::Value *Pos = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Ptr = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[2])->load(Builder, CGM);
  CGM.emitReturnDataCopy(Pos, Ptr, Length);
}

llvm::Value *CodeGenFunction::emitAsmCall(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Gas = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Address = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *TrAddress =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Address), AddressTy);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(TrAddress, AddressPtr);

  llvm::Value *Value = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *TrValue =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Value), Int128Ty);
  llvm::Value *ValuePtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(TrValue, ValuePtr);

  llvm::Value *Ptr = emitExpr(Arguments[3])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[4])->load(Builder, CGM);

  llvm::Value *OutPtr = emitExpr(Arguments[5])->load(Builder, CGM);
  [[maybe_unused]] llvm::Value *OutLength =
      emitExpr(Arguments[6])->load(Builder, CGM);

  llvm::Value *Val = CGM.emitCall(Gas, AddressPtr, ValuePtr, Ptr, Length);

  llvm::Value *RetDataSize = CGM.emitGetReturnDataSize();
  // TODO: RetDataSize > OutLength cause overflow
  CGM.emitReturnDataCopy(OutPtr, Builder.getInt32(0), RetDataSize);

  return Builder.CreateTrunc(Val, BoolTy);
}

llvm::Value *CodeGenFunction::emitAsmCallCode(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Gas = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Address = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *TrAddress =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Address), AddressTy);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(TrAddress, AddressPtr);

  llvm::Value *Value = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *TrValue =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Value), Int128Ty);
  llvm::Value *ValuePtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(TrValue, ValuePtr);
  llvm::Value *Ptr = emitExpr(Arguments[3])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[4])->load(Builder, CGM);

  llvm::Value *OutPtr = emitExpr(Arguments[5])->load(Builder, CGM);
  [[maybe_unused]] llvm::Value *OutLength =
      emitExpr(Arguments[6])->load(Builder, CGM);

  llvm::Value *Val = CGM.emitCallCode(Gas, AddressPtr, ValuePtr, Ptr, Length);

  llvm::Value *RetDataSize = CGM.emitGetReturnDataSize();
  // TODO: RetDataSize > OutLength cause overflow
  CGM.emitReturnDataCopy(OutPtr, Builder.getInt32(0), RetDataSize);

  return Builder.CreateTrunc(Val, BoolTy);
}

llvm::Value *CodeGenFunction::emitAsmDelegatecall(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Gas = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Address = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *TrAddress =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Address), AddressTy);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(TrAddress, AddressPtr);

  llvm::Value *Ptr = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[3])->load(Builder, CGM);

  llvm::Value *OutPtr = emitExpr(Arguments[4])->load(Builder, CGM);
  [[maybe_unused]] llvm::Value *OutLength =
      emitExpr(Arguments[5])->load(Builder, CGM);

  llvm::Value *Val = CGM.emitCallDelegate(Gas, AddressPtr, Ptr, Length);

  llvm::Value *RetDataSize = CGM.emitGetReturnDataSize();
  // TODO: RetDataSize > OutLength cause overflow
  CGM.emitReturnDataCopy(OutPtr, Builder.getInt32(0), RetDataSize);

  return Builder.CreateTrunc(Val, BoolTy);
}

llvm::Value *CodeGenFunction::emitAsmCallStaticcall(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Gas = emitExpr(Arguments[0])->load(Builder, CGM);

  llvm::Value *Address = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *TrAddress =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Address), AddressTy);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);
  Builder.CreateStore(TrAddress, AddressPtr);

  llvm::Value *Ptr = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[3])->load(Builder, CGM);

  llvm::Value *OutPtr = emitExpr(Arguments[4])->load(Builder, CGM);
  [[maybe_unused]] llvm::Value *OutLength =
      emitExpr(Arguments[5])->load(Builder, CGM);

  llvm::Value *Val = CGM.emitCallStatic(Gas, AddressPtr, Ptr, Length);

  llvm::Value *RetDataSize = CGM.emitGetReturnDataSize();
  // TODO: RetDataSize > OutLength cause overflow
  CGM.emitReturnDataCopy(OutPtr, Builder.getInt32(0), RetDataSize);

  return Builder.CreateTrunc(Val, BoolTy);
}

// create(v, p, n)	create new contract with code mem[p…(p+n))
// and send v wei and return the new address
llvm::Value *CodeGenFunction::emitAsmCreate(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Wei = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Weitrunc =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Wei), Int128Ty);
  llvm::Value *ValuePtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(Weitrunc, ValuePtr);
  llvm::Value *Ptr = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);

  CGM.emitCreate(ValuePtr, Ptr, Length, AddressPtr);
  return Builder.CreateLoad(AddressPtr, AddressTy);
}

llvm::Value *CodeGenFunction::emitAsmCreate2(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Wei = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Weitrunc =
      Builder.CreateZExtOrTrunc(CGM.getEndianlessValue(Wei), Int128Ty);
  llvm::Value *ValuePtr = Builder.CreateAlloca(Int128Ty);
  Builder.CreateStore(Weitrunc, ValuePtr);
  llvm::Value *Ptr = emitExpr(Arguments[1])->load(Builder, CGM);
  llvm::Value *Length = emitExpr(Arguments[2])->load(Builder, CGM);
  llvm::Value *Salt = emitExpr(Arguments[3])->load(Builder, CGM);
  llvm::Value *AddressPtr = Builder.CreateAlloca(AddressTy);

  CGM.emitCreate2(ValuePtr, Ptr, Length, Salt, AddressPtr);
  return Builder.CreateLoad(AddressPtr, AddressTy);
}

llvm::Value *CodeGenFunction::emitAsmByte(const CallExpr *CE) {
  auto Arguments = CE->getArguments();

  llvm::Value *Offset = emitExpr(Arguments[0])->load(Builder, CGM);
  llvm::Value *Value = emitExpr(Arguments[1])->load(Builder, CGM);

  return Builder.CreateAnd(Builder.getIntN(256, 0xFF),
                           Builder.CreateLShr(Value, Offset));
}

llvm::Value *CodeGenFunction::emitAsmChainId(const CallExpr *CE) {
  llvm::Value *ValPtr = Builder.CreateAlloca(Int128Ty, nullptr);
  CGM.emitGetChainId(ValPtr);
  return Builder.CreateLoad(ValPtr, Int128Ty);
}

ExprValuePtr CodeGenFunction::emitAsmSpecialCallExpr(const AsmIdentifier *SI,
                                                     const CallExpr *CE) {
  switch (SI->getSpecialIdentifier()) {
  /// logical
  case AsmIdentifier::SpecialIdentifier::not_: ///< should be handled in Sema
  case AsmIdentifier::SpecialIdentifier::and_:
  case AsmIdentifier::SpecialIdentifier::or_:
  case AsmIdentifier::SpecialIdentifier::xor_:
  /// arithmetic
  case AsmIdentifier::SpecialIdentifier::addu256:
  case AsmIdentifier::SpecialIdentifier::subu256:
  case AsmIdentifier::SpecialIdentifier::mulu256:
  case AsmIdentifier::SpecialIdentifier::divu256:
  case AsmIdentifier::SpecialIdentifier::divs256:
  case AsmIdentifier::SpecialIdentifier::modu256:
  case AsmIdentifier::SpecialIdentifier::mods256:
  case AsmIdentifier::SpecialIdentifier::expu256:
  case AsmIdentifier::SpecialIdentifier::ltu256:
  case AsmIdentifier::SpecialIdentifier::gtu256:
  case AsmIdentifier::SpecialIdentifier::lts256:
  case AsmIdentifier::SpecialIdentifier::gts256:
  case AsmIdentifier::SpecialIdentifier::equ256:
  case AsmIdentifier::SpecialIdentifier::notu256:
  case AsmIdentifier::SpecialIdentifier::andu256:
  case AsmIdentifier::SpecialIdentifier::oru256:
  case AsmIdentifier::SpecialIdentifier::xoru256:
  case AsmIdentifier::SpecialIdentifier::shlu256:
  case AsmIdentifier::SpecialIdentifier::shru256:
    assert(false && "unexpected special identifiers");
    __builtin_unreachable();
  case AsmIdentifier::SpecialIdentifier::addmodu256:
    return ExprValue::getRValue(CE, emitCallAddmod(CE));
  case AsmIdentifier::SpecialIdentifier::mulmodu256:
    return ExprValue::getRValue(CE, emitCallMulmod(CE));
  /// memory and storage
  case AsmIdentifier::SpecialIdentifier::mload:
    return ExprValue::getRValue(CE, emitAsmCallMLoad(CE));
  case AsmIdentifier::SpecialIdentifier::mstore:
    emitAsmCallMStore(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::mstore8:
    emitAsmCallMStore8(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::msize:
    return ExprValue::getRValue(CE, emitAsmCallMSize(CE));
  case AsmIdentifier::SpecialIdentifier::sload:
    return ExprValue::getRValue(CE, emitAsmCallSLoad(CE));
  case AsmIdentifier::SpecialIdentifier::sstore:
    emitAsmCallSStore(CE);
    return std::make_shared<ExprValue>();
  /// execution control
  case AsmIdentifier::SpecialIdentifier::pop:
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::invalid:
    CGM.emitTrap();
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::stop:
    CGM.emitFinish(llvm::ConstantPointerNull::get(Int8PtrTy),
                   Builder.getInt32(0));
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::return_:
    emitAsmCallReturn(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::revert:
    emitAsmCallRevert(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::log0:
  case AsmIdentifier::SpecialIdentifier::log1:
  case AsmIdentifier::SpecialIdentifier::log2:
  case AsmIdentifier::SpecialIdentifier::log3:
  case AsmIdentifier::SpecialIdentifier::log4:
    emitAsmCallLog(CE);
    return std::make_shared<ExprValue>();
  /// state
  case AsmIdentifier::SpecialIdentifier::blockcoinbase:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(
            CGM.getEndianlessValue(CGM.emitGetBlockCoinbase()), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::blockdifficulty:
    return ExprValue::getRValue(CE, CGM.emitGetBlockDifficulty());
  case AsmIdentifier::SpecialIdentifier::blockgaslimit:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(CGM.emitGetBlockGasLimit(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::blockhash:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmGetBlockHash(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::blocknumber:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(CGM.emitGetBlockNumber(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::blocktimestamp:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(CGM.emitGetBlockTimestamp(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::txorigin:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(
                CGM.getEndianlessValue(CGM.emitGetTxOrigin()), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::txgasprice:
    return ExprValue::getRValue(
        CE, CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
                CGM.emitGetTxGasPrice(), CGM.Int256Ty)));
  case AsmIdentifier::SpecialIdentifier::gasleft:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(CGM.emitGetGasLeft(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::balance:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(emitAsmGetBalance(CE, false), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::selfbalance:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(emitAsmGetBalance(CE, true), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::caller:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(
                CGM.getEndianlessValue(CGM.emitGetCaller()), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::callvalue:
    return ExprValue::getRValue(
        CE, CGM.getEndianlessValue(Builder.CreateZExtOrTrunc(
                CGM.emitGetCallValue(), CGM.Int256Ty)));
  case AsmIdentifier::SpecialIdentifier::calldataload:
    return ExprValue::getRValue(CE, emitAsmCallCallDataLoad(CE));
  case AsmIdentifier::SpecialIdentifier::calldatasize:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(CGM.emitGetCallDataSize(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::calldatacopy:
    emitAsmCallDataCopy(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::address:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(
                CGM.getEndianlessValue(CGM.emitGetAddress()), CGM.Int256Ty));
  /// object
  case AsmIdentifier::SpecialIdentifier::dataoffset:
    return ExprValue::getRValue(CE, emitAsmCallDataOffset(CE));
  case AsmIdentifier::SpecialIdentifier::datasize:
    return ExprValue::getRValue(CE, emitAsmCallDataSize(CE));
  case AsmIdentifier::SpecialIdentifier::datacopy:
  case AsmIdentifier::SpecialIdentifier::codecopy:
    emitAsmCallCodeCopy(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::extcodecopy:
    emitAsmExternalCallCodeCopy(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::codesize:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(CGM.emitGetCodeSize(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::extcodesize:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmExternalGetCodeSize(CE),
                                      CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::extcodehash:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmExternalCallCodeHash(CE),
                                      CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::returndatasize:
    return ExprValue::getRValue(
        CE,
        Builder.CreateZExtOrTrunc(CGM.emitGetReturnDataSize(), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::returndatacopy:
    emitAsmReturnDataCopy(CE);
    return std::make_shared<ExprValue>();
  /// misc
  case AsmIdentifier::SpecialIdentifier::keccak256:
    return ExprValue::getRValue(CE, emitAsmCallkeccak256(CE));
  case AsmIdentifier::SpecialIdentifier::create:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmCreate(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::call:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmCall(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::callcode:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmCallCode(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::delegatecall:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmDelegatecall(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::staticcall:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmCallStaticcall(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::byte:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmByte(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::selfdestruct:
    emitAsmSelfDestruct(CE);
    return std::make_shared<ExprValue>();
  case AsmIdentifier::SpecialIdentifier::signextendu256:
    return ExprValue::getRValue(
        CE,
        Builder.CreateSExtOrTrunc(
            emitExpr(CE->getArguments()[0])->load(Builder, CGM), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::create2:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmCreate2(CE), CGM.Int256Ty));
  case AsmIdentifier::SpecialIdentifier::chainid:
    return ExprValue::getRValue(
        CE, Builder.CreateZExtOrTrunc(emitAsmChainId(CE), CGM.Int256Ty));
  // TODO:
  // - implement special identifiers below and
  //   move implemented ones above this TODO.
  case AsmIdentifier::SpecialIdentifier::iszerou256:
  case AsmIdentifier::SpecialIdentifier::sars256:
  case AsmIdentifier::SpecialIdentifier::abort:
  case AsmIdentifier::SpecialIdentifier::this_:
  case AsmIdentifier::SpecialIdentifier::discard:
  case AsmIdentifier::SpecialIdentifier::discardu256:
  case AsmIdentifier::SpecialIdentifier::splitu256tou64:
  case AsmIdentifier::SpecialIdentifier::combineu64tou256:
  default:
    assert(false && "special function not supported yet");
    __builtin_unreachable();
  }
}

} // namespace soll::CodeGen
