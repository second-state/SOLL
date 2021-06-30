// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <unordered_set>
#include <vector>

namespace soll {
namespace {

bool isAllowedTypeForUnary(UnaryOperatorKind UOK, const Type::Category TyC) {
  switch (TyC) {
  case Type::Category::Bool:
    return UOK == UO_LNot || UOK == UO_IsZero;
  case Type::Category::Integer:
    return UOK == UO_Not || UOK == UO_Plus || UOK == UO_Minus ||
           UOK == UO_IsZero || UnaryOperator::isIncrementOp(UOK) ||
           UnaryOperator::isDecrementOp(UOK);
  case Type::Category::FixedBytes:
    return UOK == UO_Not || UOK == UO_IsZero;
  default:
    return false;
  }
}

bool isAllowedTypeForBinary(BinaryOperatorKind BOK, const Type::Category TyC) {
  BOK = BinaryOperator::compoundAssignmentToNormal(BOK);
  switch (TyC) {
  case Type::Category::Bool:
    return BinaryOperator::isEqualityOp(BOK) || BOK == BO_LAnd ||
           BOK == BO_LOr || BOK == BO_Assign ||
           BinaryOperator::isAsmBitwiseOp(BOK);
  case Type::Category::Integer:
    return BinaryOperator::isComparisonOp(BOK) ||
           BinaryOperator::isBitwiseOp(BOK) ||
           BinaryOperator::isAsmBitwiseOp(BOK) ||
           BinaryOperator::isShiftOp(BOK) ||
           BinaryOperator::isAdditiveOp(BOK) ||
           BinaryOperator::isMultiplicativeOp(BOK) || BOK == BO_Exp ||
           BOK == BO_Assign;
  case Type::Category::RationalNumber:
    return BinaryOperator::isComparisonOp(BOK) ||
           BinaryOperator::isAdditiveOp(BOK) ||
           BinaryOperator::isMultiplicativeOp(BOK) || BOK == BO_Assign;
  case Type::Category::Address:
    return BinaryOperator::isComparisonOp(BOK) || BOK == BO_Assign;
  case Type::Category::FixedBytes:
    return BinaryOperator::isComparisonOp(BOK) ||
           BinaryOperator::isBitwiseOp(BOK) ||
           BinaryOperator::isAsmBitwiseOp(BOK) ||
           BinaryOperator::isShiftOp(BOK) || BOK == BO_Assign;
  case Type::Category::Array:
  case Type::Category::String:
  case Type::Category::Bytes:
  case Type::Category::Struct:
  case Type::Category::Tuple:
    return BOK == BO_Assign;
  default:
    return false;
  }
}

bool isAllowedForTypecast(const Type *In, const Type *Out, bool IsLiteral,
                          const Expr *SE) {
  const Type::Category InC = In->getCategory();
  const Type::Category OutC = Out->getCategory();

  const std::array<Type::Category, 5> NumericType({
      Type::Category::Bool,
      Type::Category::Integer,
      Type::Category::RationalNumber,
      Type::Category::FixedBytes,
  });

  const bool InIsNumeric = std::find(NumericType.begin(), NumericType.end(),
                                     InC) != NumericType.end();
  const bool OutIsNumeric = std::find(NumericType.begin(), NumericType.end(),
                                      OutC) != NumericType.end();

  if (InIsNumeric && OutIsNumeric) {
    return true;
  }
  if (OutC == Type::Category::Address && InIsNumeric &&
      In->getBitNum() == 160) {
    return true;
  }
  if (InC == Type::Category::Address && OutIsNumeric &&
      Out->getBitNum() == 160) {
    return true;
  }
  if (InC == Type::Category::String && IsLiteral) {
    return true;
  }
  if (InC == Type::Category::Tuple && OutC == Type::Category::Tuple) {
    auto InT = dynamic_cast<const TupleType *>(In);
    auto OutT = dynamic_cast<const TupleType *>(Out);
    auto TupleE = dynamic_cast<const TupleExpr *>(SE);
    assert(TupleE && "expect SE is a TupleExpr");
    if (InT->getElementTypes().size() != OutT->getElementTypes().size()) {
      return false;
    }

    size_t Size = InT->getElementTypes().size();
    bool Result = true;
    for (size_t Idx = 0; Idx < Size; ++Idx) {
      if (InT->getElementTypes()[Idx]) {
        if (OutT->getElementTypes()[Idx]) {
          auto ICExpr = dynamic_cast<const ImplicitCastExpr *>(
              TupleE->getComponents()[Idx]);
          assert(ICExpr);
          auto CompExpr = ICExpr->getSubExpr();
          const bool IsLiteral =
              dynamic_cast<const NumberLiteral *>(CompExpr) ||
              dynamic_cast<const StringLiteral *>(CompExpr);
          Result &= isAllowedForTypecast(InT->getElementTypes()[Idx].get(),
                                         OutT->getElementTypes()[Idx].get(),
                                         IsLiteral, CompExpr);
        }
      } else {
        Result &= OutT->getElementTypes()[Idx] == nullptr;
      }
    }

    return Result;
  }
  return false;
}

bool isImplementedForTypecast(const Type *In, const Type *Out, bool IsLiteral) {
  if (In->getCategory() == Type::Category::String) {
    return false;
  }

  return true;
}

void setTypeForBinary(Sema &Actions, BinaryOperator &BO, ImplicitCastExpr *LHS,
                      ImplicitCastExpr *RHS, TypePtr LHSTy, TypePtr RHSTy,
                      TypePtr Ty) {
  if (!isAllowedTypeForBinary(BO.getOpcode(), Ty->getCategory())) {
    Actions.Diag(BO.getLocation().getBegin(),
                 diag::err_typecheck_invalid_operands)
        << LHSTy->getName() << RHSTy->getName();
    return;
  }
  if (BO.getOpcode() == BO_Exp) {
    auto *RHSIntTy = dynamic_cast<IntegerType *>(RHSTy.get());
    if (RHSIntTy->isSigned()) {
      Actions.Diag(BO.getLocation().getBegin(), diag::err_typecheck_exp_signed);
      return;
    }
    LHS->setType(LHSTy);
    RHS->setType(RHSTy);
    BO.setType(LHSTy);
    return;
  }
  if (BinaryOperator::isShiftOp(BO.getOpcode())) {
    LHS->setType(LHSTy);
    RHS->setType(RHSTy);
    BO.setType(LHSTy);
    return;
  }
  if (!LHSTy->isEqual(*Ty)) {
    LHS->setType(Ty);
    Actions.resolveImplicitCast(*LHS, Ty, BO.isAssignmentOp());
  } else if (!RHSTy->isEqual(*Ty)) {
    RHS->setType(Ty);
    Actions.resolveImplicitCast(*RHS, Ty, false);
  }
  BO.setType(BO.isComparisonOp() ? Actions.getContext().BooleanTypePtr : Ty);
}

class TypeResolver : public StmtVisitor {
  TypePtr ReturnType;
  Sema &Actions;
  ContractDecl *&CurrentContract;
  std::unordered_set<Expr *> Visited;

public:
  TypeResolver(Sema &A, ContractDecl *&CurrentContract)
      : Actions(A), CurrentContract(CurrentContract) {}
  void setReturnType(TypePtr Ty) { ReturnType = std::move(Ty); }
  Sema &getSema() { return Actions; }

  void visit(TupleExprType &TE) override {
    StmtVisitor::visit(TE);
    std::vector<TypePtr> Types;
    // Notes : All element are warpped by ImplicitCastExpr
    for (const auto &Comp : TE.getComponents()) {
      if (Comp) {
        auto CastR = dynamic_cast<const ImplicitCastExpr *>(Comp);
        Types.emplace_back(CastR->getSubExpr()->getType());
      } else {
        Types.emplace_back(nullptr);
      }
    }
    TE.setType(std::make_shared<TupleType>(std::move(Types)));
  }

  void visit(ImplicitCastExprType &ICE) override {
    StmtVisitor::visit(ICE);
    if (ICE.getCastKind() == CastKind::TypeCast) {
      const Expr *SE = ICE.getSubExpr();
      const auto &InType = SE->getType();
      const auto &OutType = ICE.getType();

      if (InType->isEqual(*OutType)) {
        if (InType->getCategory() == Type::Category::Bool) {
          ICE.setCastKind(CastKind::None);
        } else {
          ICE.setCastKind(CastKind::IntegralCast);
        }
        return;
      }

      const bool IsLiteral = dynamic_cast<const NumberLiteralType *>(SE) ||
                             dynamic_cast<const StringLiteralType *>(SE);
      if (!isAllowedForTypecast(InType.get(), OutType.get(), IsLiteral, SE)) {
        Actions.Diag(ICE.getLocation().getBegin(),
                     diag::err_typecheck_invalid_cast)
            << InType->getName() << OutType->getName();
        return;
      }
      if (!isImplementedForTypecast(InType.get(), OutType.get(), IsLiteral)) {
        Actions.Diag(ICE.getLocation().getBegin(),
                     diag::err_typecheck_unimplemented_cast)
            << InType->getName() << OutType->getName();
        return;
      }
      return;
    }
    if (ICE.getType() || !ICE.getSubExpr()) {
      return;
    }
    ICE.setType(ICE.getSubExpr()->getType());
  }
  void visit(ReturnStmtType &IS) override {
    StmtVisitor::visit(IS);
    if (auto *IC = dynamic_cast<ImplicitCastExpr *>(IS.getRetValue())) {
      Actions.resolveImplicitCast(*IC, ReturnType, false);
    }
  }
  void visit(UnaryOperatorType &UO) override {
    StmtVisitor::visit(UO);
    auto Ty = UO.getSubExpr()->getType();
    if (!isAllowedTypeForUnary(UO.getOpcode(), Ty->getCategory())) {
      Actions.Diag(UO.getLocation().getBegin(), diag::err_typecheck_unary_expr)
          << Ty->getName();
      return;
    }

    UO.setType(UO.getOpcode() == UO_IsZero ? Actions.getContext().BooleanTypePtr
                                           : Ty);
  }
  void visit(BinaryOperatorType &BO) override {
    StmtVisitor::visit(BO); // This is Strange.
    if (auto *LHS = dynamic_cast<ImplicitCastExpr *>(BO.getLHS())) {
      if (auto *RHS = dynamic_cast<ImplicitCastExpr *>(BO.getRHS())) {
        auto LHSTy = LHS->getSubExpr()->getType();
        auto RHSTy = RHS->getSubExpr()->getType();
        if (!LHSTy || !RHSTy) {
          return;
        }
        if (LHSTy->isEqual(*RHSTy) || BO.isAssignmentOp() || BO.isShiftOp() ||
            BO.getOpcode() == BO_Exp ||
            RHSTy->isImplicitlyConvertibleTo(*LHSTy)) {
          setTypeForBinary(Actions, BO, LHS, RHS, LHSTy, RHSTy, LHSTy);
          StmtVisitor::visit(BO);
          return;
        }
        if (LHSTy->isImplicitlyConvertibleTo(*RHSTy)) {
          setTypeForBinary(Actions, BO, LHS, RHS, LHSTy, RHSTy, RHSTy);
          StmtVisitor::visit(BO);
          return;
        }
        Actions.Diag(BO.getLocation().getBegin(),
                     diag::err_typecheck_no_implicit_convertion)
            << LHSTy->getName() << RHSTy->getName();
        return;
      }
    }
    assert(false);
  }
  void visit(CallExprType &CE) override;
  void visit(IndexAccessType &IA) override {
    StmtVisitor::visit(IA);
    bool NeedIntegerSubscript = true;
    const Type *BaseTy = IA.getBase()->getType().get();
    if (auto MT = dynamic_cast<const MappingType *>(BaseTy)) {
      IA.setType(MT->getValueType());
      NeedIntegerSubscript = false;
    } else if (auto AT = dynamic_cast<const ArrayType *>(BaseTy)) {
      IA.setType(AT->getElementType());
    } else if (dynamic_cast<const StringType *>(BaseTy) ||
               dynamic_cast<const BytesType *>(BaseTy)) {
      IA.setType(Actions.getContext().FixedBytesTypeB1Ptr);
    } else {
      Actions.Diag(IA.getBase()->getLocation().getBegin(),
                   diag::err_typecheck_subscript_value);
    }
    if (NeedIntegerSubscript &&
        IA.getIndex()->getType()->getCategory() != Type::Category::Integer) {
      Actions.Diag(IA.getIndex()->getLocation().getBegin(),
                   diag::err_typecheck_subscript_not_integer);
    }
  }
  void visit(MemberExprType &ME) override {
    StmtVisitor::visit(ME);
    ME.setType(ME.getName()->getType());
    if (ME.getType()) {
      return;
    }

    static const llvm::StringMap<Identifier::SpecialIdentifier> ArrayLookup{
        {"length", Identifier::SpecialIdentifier::array_length},
        {"push", Identifier::SpecialIdentifier::array_push},
        {"pop", Identifier::SpecialIdentifier::array_pop},
    };
    static const llvm::StringMap<Identifier::SpecialIdentifier> AddressLookup{
        {"balance", Identifier::SpecialIdentifier::address_balance},
        {"transfer", Identifier::SpecialIdentifier::address_transfer},
        {"send", Identifier::SpecialIdentifier::address_send},
        {"call", Identifier::SpecialIdentifier::address_call},
        {"delegatecall", Identifier::SpecialIdentifier::address_delegatecall},
        {"staticcall", Identifier::SpecialIdentifier::address_staticcall}};

    Token Tok = ME.getName()->getToken();
    llvm::StringRef Name = ME.getName()->getName();
    switch (ME.getBase()->getType()->getCategory()) {
    case Type::Category::Array:
      if (auto Iter = ArrayLookup.find(Name); Iter != ArrayLookup.end()) {
        std::shared_ptr<Type> Ty;
        switch (Iter->second) {
        case Identifier::SpecialIdentifier::array_length:
          Ty = Actions.getContext().IntegerTypeU256Ptr;
          break;
        case Identifier::SpecialIdentifier::array_push:
          Actions.Diag(Tok.getLocation(), diag::err_unimplemented_identifier)
              << Name;
          break;
        case Identifier::SpecialIdentifier::array_pop:
          Actions.Diag(Tok.getLocation(), diag::err_unimplemented_identifier)
              << Name;
          break;
        default:
          assert(false && "unknown member");
          __builtin_unreachable();
        }
        ME.setName(std::make_unique<Identifier>(Tok, Iter->second, Ty));
        return;
      }
      break;
    case Type::Category::Address:
      if (auto Iter = AddressLookup.find(Name); Iter != AddressLookup.end()) {
        std::shared_ptr<Type> Ty;
        switch (Iter->second) {
        case Identifier::SpecialIdentifier::address_balance:
          Ty = Actions.getContext().IntegerTypeU256Ptr;
          break;
        case Identifier::SpecialIdentifier::address_transfer:
          Ty = std::make_shared<FunctionType>(
              std::vector<std::reference_wrapper<const TypePtr>>{
                  Actions.getContext().IntegerTypeU256Ptr},
              std::vector<std::reference_wrapper<const TypePtr>>{});
          break;
        case Identifier::SpecialIdentifier::address_send:
          Ty = std::make_shared<FunctionType>(
              std::vector<std::reference_wrapper<const TypePtr>>{
                  Actions.getContext().IntegerTypeU256Ptr},
              std::vector<std::reference_wrapper<const TypePtr>>{
                  Actions.getContext().BooleanTypePtr});
          break;
        case Identifier::SpecialIdentifier::address_call:
        case Identifier::SpecialIdentifier::address_delegatecall:
        case Identifier::SpecialIdentifier::address_staticcall:
          Ty = std::make_shared<FunctionType>(
              std::vector<std::reference_wrapper<const TypePtr>>{
                  Actions.getContext().BytesTypePtr},
              std::vector<std::reference_wrapper<const TypePtr>>{
                  Actions.getContext().BooleanTypePtr,
                  Actions.getContext().BytesTypePtr});
          break;
        default:
          assert(false && "unknown member");
          __builtin_unreachable();
        }
        ME.setName(std::make_unique<Identifier>(Tok, Iter->second, Ty));
        return;
      }
      break;
    case Type::Category::Struct:
      if (auto *ST =
              dynamic_cast<const StructType *>(ME.getBase()->getType().get())) {
        ME.setName(std::make_unique<Identifier>(
            Tok, ST->getElementTypes()[ST->getElementIndex(Name.str())]));
        return;
      }
      break;
    case Type::Category::Contract:
      if (auto CT = dynamic_cast<const ContractType *>(
              ME.getBase()->getType().get())) {
        for (auto FD : CT->getDecl()->getFuncs()) {
          if (FD->getName() == Name) {
            auto Ty = FD->getType();
            ME.setName(std::make_unique<Identifier>(
                Tok, Identifier::SpecialIdentifier::external_call, Ty));
            return;
          }
        }
      }
      break;
    default:
      break;
    }
    auto &Map = CurrentContract->getTypeMemberMap();
    FunctionDecl *FD = nullptr;
    ContractDecl *CD = nullptr;
    if (Map.count(ME.getBase()->getType()->getName())) {
      auto &MemberMap = Map[ME.getBase()->getType()->getName()];
      if (MemberMap.count(Name)) {
        std::tie(CD, FD) = MemberMap.lookup(Name);
      }
    }
    if (Map.count("")) {
      auto &MemberMap = Map[""];
      if (MemberMap.count(Name)) {
        std::tie(CD, FD) = MemberMap.lookup(Name);
      }
    }
    if (FD && CD) {
      auto Ty = FD->getType();
      ME.setName(std::make_unique<Identifier>(
          Tok, Identifier::SpecialIdentifier::library_call, Ty));
      auto &Map = *Actions.getLibrariesAddressMap();
      auto Address = llvm::APInt(160, 0);
      if (Map.count(CD->getName())) {
        Address = Map.lookup(CD->getName());
      }
      ME.setLibraryAddress(Address);
      return;
    }
    Actions.Diag(ME.getName()->getLocation().getBegin(), diag::err_no_member)
        << Name << ME.getBase()->getType()->getName();
  }
  void visit(ParenExprType &PE) override {
    StmtVisitor::visit(PE);
    PE.setType(PE.getSubExpr()->getType());
  }
  void visit(DeclStmtType &DS) override {
    StmtVisitor::visit(DS);
    if (DS.getValue()) {
      DS.getValue()->accept(*this);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(DS.getValue())) {
        IC->accept(*this);
        Actions.resolveImplicitCast(*IC, DS.getVarDecls().front()->getType(),
                                    false);
      }
    }
  }
  void visit(AsmFunctionDeclStmtType &FDS) override {
    StmtVisitor::visit(FDS);
    if (auto *D = FDS.getDecl()) {
      D->getReturnParams()->createParamsTy();
      if (auto *B = D->getBody()) {
        B->accept(*this);
      }
    }
  }
  void visit(AsmAssignmentStmt &AS) override {
    StmtVisitor::visit(AS);
    AS.getRHS()->accept(*this);
    if (auto *IC = dynamic_cast<ImplicitCastExpr *>(AS.getRHS())) {
      IC->accept(*this);
      Actions.resolveImplicitCast(
          *IC, AS.getLHS()->getIdentifiers().front()->getType(), false);
    }
  }
};

class DeclTypeResolver : public DeclVisitor {
  TypeResolver TR;
  Sema &Actions;
  ContractDecl *CurrentContract;

public:
  DeclTypeResolver(Sema &S)
      : TR(S, CurrentContract), Actions(S), CurrentContract(nullptr) {}
  void visit(UsingForType &UF) {
    auto &Map = CurrentContract->getTypeMemberMap();
    std::string TypeName = "";
    if (UF.getType())
      TypeName = UF.getType()->getUniqueName();
    for (auto Lib : UF.getLibraries()) {
      for (auto Func : Lib->getFuncs()) {
        Map[TypeName][Func->getName()] = {Lib, Func};
      }
    }
  }
  void visit(ContractDeclType &CD) override {
    CurrentContract = &CD;
    DeclVisitor::visit(CD);
    CurrentContract = nullptr;
  }
  void visit(ParamListType &PL) override {
    DeclVisitor::visit(PL);
    PL.createParamsTy();
  }
  void visit(FunctionDecl &FD) override {
    DeclVisitor::visit(FD);
    if (auto *Ty = dynamic_cast<FunctionType *>(FD.getType().get())) {
      if (Ty->getReturnTypes().empty()) {
        TR.setReturnType(nullptr);
      } else {
        /// TODO: implement multiple return type
        assert(Ty->getReturnTypes().size() == 1 &&
               "multiple return type not implemented");
        TR.setReturnType(Ty->getReturnTypes().front());
      }
      if (FD.getBody() != nullptr)
        FD.getBody()->accept(TR);
      TR.setReturnType(nullptr);
    }
  }
  void visit(VarDecl &VD) override {
    if (VD.getValue()) {
      VD.getValue()->accept(TR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.getValue())) {
        TR.getSema().resolveImplicitCast(*IC, VD.getType(), false);
      }
    }
    DeclVisitor::visit(VD);
  }
  void visit(AsmVarDeclType &VD) override {
    if (VD.getValue()) {
      VD.getValue()->accept(TR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.getValue())) {
        TR.getSema().resolveImplicitCast(*IC, VD.getType(), false);
      }
    }
    DeclVisitor::visit(VD);
  }
  void visit(YulCodeType &C) override {
    C.getBody()->accept(TR);
    DeclVisitor::visit(C);
  }
};

void TypeResolver::visit(CallExprType &CE) {
  if (Visited.count(&CE))
    return;
  Visited.emplace(&CE);
  StmtVisitor::visit(CE);

  Expr *CalleeExpr = CE.getCalleeExpr(), *E = nullptr, *Base = nullptr;
  MemberExpr *ME = dynamic_cast<MemberExpr *>(CalleeExpr);
  std::string FunctionSignature;
  TypePtr const *ReturnTy = &Actions.getContext().BytesTypePtr;
  if (ME) {
    auto Name = ME->getName();
    E = Name;
    Base = ME->getBase();
    auto calculateFunctionSignature = [&]() {
      FunctionSignature = ME->getName()->getName().str() + "(";
      auto FTy = dynamic_cast<FunctionType *>(ME->getType().get());
      auto &ParamTypes = FTy->getParamTypes();
      if (FTy->getReturnTypes().size() == 1) {
        ReturnTy = &FTy->getReturnTypes().at(0).get();
      } else if (FTy->getReturnTypes().size() == 0) {
        ReturnTy = &Actions.getContext().NullPtr;
      } else {
        assert(false && "multiple return value is not support yet!");
      }
      bool First = true;
      for (auto PTy : ParamTypes) {
        if (!First)
          FunctionSignature += ",";
        First = false;
        FunctionSignature += PTy.get()->getSignatureEncoding();
      }
      FunctionSignature += ")";
    };
    if (auto CT = dynamic_cast<const ContractType *>(Base->getType().get());
        CT && CT->getDecl()) {
      // A ContractType without Decl is solidity reserved word.
      calculateFunctionSignature();
    } else if (Name->isSpecialIdentifier()) {
      switch (Name->getSpecialIdentifier()) {
      case Identifier::SpecialIdentifier::external_call:
      case Identifier::SpecialIdentifier::library_call: {
        calculateFunctionSignature();
        break;
      }
      default: {
        break;
      }
      }
    }
  } else {
    E = CalleeExpr;
  }

  CE.resolveNamedCall();

  FunctionType *FTy = nullptr;
  if (auto I = dynamic_cast<Identifier *>(E)) {
    if (!I->isResolved()) {
      return;
    }
    if (I->isSpecialIdentifier()) {
      std::vector<std::reference_wrapper<const TypePtr>> ArgTypes;
      for (const auto &arg : CE.getArguments()) {
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(arg)) {
          ArgTypes.emplace_back(std::cref(IC->getSubExpr()->getType()));
        }
      }
      switch (I->getSpecialIdentifier()) {
      case Identifier::SpecialIdentifier::abi_decode: {
        if (ME) {
          if (auto I = dynamic_cast<Identifier *>(ME->getName())) {
            I->setType(ArgTypes.at(1));
            ME->setType(ArgTypes.at(1));
            ReturnTy = &ArgTypes.at(1).get();
            if (auto TP = dynamic_cast<TupleType *>(ReturnTy->get())) {
              const auto &ElementTypes = TP->getElementTypes();
              if (ElementTypes.size() == 1)
                ReturnTy = &ElementTypes.front();
            }
          }
        }
        ArgTypes.at(0) = Actions.getContext().BytesTypePtr;
        [[fallthrough]];
      }
      case Identifier::SpecialIdentifier::abi_encodePacked:
      case Identifier::SpecialIdentifier::abi_encode:
        break;
      case Identifier::SpecialIdentifier::library_call: {
        auto &RawArguments = CE.getRawArguments();
        auto First = Actions.CreateDummy(ME->moveBase());
        RawArguments.insert(RawArguments.begin(), std::move(First));
        auto LibraryAddressLiteral = std::make_unique<NumberLiteral>(
            SourceRange(), false, ME->getLibraryAddress());
        auto LibraryAddress =
            Actions.CreateDummy(std::move(LibraryAddressLiteral));
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(LibraryAddress.get())) {
          Actions.resolveImplicitCast(
              *IC, Actions.getContext().AddressTypePayablePtr, false);
        }
        ME->setBase(std::move(LibraryAddress));
        Base = ME->getBase();
        [[fallthrough]];
      }
      /*<Contract>.func(...) ==
       * address(<Contract>).call(abi.encodeWithSignature(signature(func),...))
       */
      case Identifier::SpecialIdentifier::external_call: {
        auto SignatureStringLiteral = std::make_unique<StringLiteral>(
            Token(), std::move(FunctionSignature));
        ArgTypes.emplace(ArgTypes.begin(),
                         std::cref(Actions.getContext().StringTypePtr));
        SignatureStringLiteral->setType(ArgTypes.at(0));
        auto Signature = Actions.CreateDummy(std::move(SignatureStringLiteral));
        auto &RawArguments = CE.getRawArguments();
        RawArguments.emplace(RawArguments.begin(), std::move(Signature));
        [[fallthrough]];
      }
      /* abi.encodeWithSignature(string signature, ...) ==
       * abi.encodeWithSelector(bytes4(keccak256(bytes(signature))), ...) */
      case Identifier::SpecialIdentifier::abi_encodeWithSignature: {
        auto &Signature = CE.getRawArguments().at(0);
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(Signature.get())) {
          Actions.resolveImplicitCast(*IC, Actions.getContext().StringTypePtr,
                                      false);
        }
        auto SignatureBytes = std::make_unique<ExplicitCastExpr>(
            SourceRange(), std::move(Signature), CastKind::TypeCast,
            Actions.getContext().BytesTypePtr);
        std::vector<ExprPtr> Keccak256Arguments;
        Signature = Actions.CreateDummy(std::move(SignatureBytes));
        Keccak256Arguments.emplace_back(std::move(Signature));
        auto CallKeccak256 = std::make_unique<CallExpr>(
            SourceRange(),
            std::move(std::make_unique<Identifier>(
                Token(), Identifier::SpecialIdentifier::keccak256, nullptr)),
            std::move(Keccak256Arguments));
        CallKeccak256->setType(Actions.getContext().FixedBytesTypeB32Ptr);
        Signature = Actions.CreateDummy(std::move(CallKeccak256));
        [[fallthrough]];
      }
      /* abi.encodeWithSelector(bytes4 selector, ...) ==
       * abi.encodePacked(selector, abi.encode(...)); */
      case Identifier::SpecialIdentifier::abi_encodeWithSelector: {
        ArgTypes.clear();
        ArgTypes.emplace_back(
            std::cref(Actions.getContext().FixedBytesTypeB4Ptr));
        ArgTypes.emplace_back(std::cref(Actions.getContext().BytesTypePtr));
        auto &RawArguments = CE.getRawArguments();
        auto &Selector = RawArguments.at(0);
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(Selector.get())) {
          Actions.resolveImplicitCast(*IC, ArgTypes.at(0), false);
        }
        Selector = Actions.CreateDummy(
            std::move(Selector)); // this Dummy will be remove in AbiEmitter
        std::vector<ExprPtr> AbiEncodeArguments;
        std::move(RawArguments.begin() + 1, RawArguments.end(),
                  std::back_inserter(AbiEncodeArguments));
        RawArguments.resize(2);
        auto CallAbiEncode = std::make_unique<CallExpr>(
            SourceRange(),
            std::move(std::make_unique<Identifier>(
                Token(), Identifier::SpecialIdentifier::abi_encode, nullptr)),
            std::move(AbiEncodeArguments));
        CallAbiEncode->setType(ArgTypes.at(1));
        RawArguments.at(1) = Actions.CreateDummy(std::move(CallAbiEncode));
        break;
      }
      default:
        FTy = dynamic_cast<FunctionType *>(I->getType().get());
      }
      switch (I->getSpecialIdentifier()) {
      case Identifier::SpecialIdentifier::external_call:
      case Identifier::SpecialIdentifier::library_call: {
        auto &RawArguments = CE.getRawArguments();
        auto CallAbiEncodeWithSelector = std::make_unique<CallExpr>(
            SourceRange(),
            std::move(std::make_unique<Identifier>(
                Token(), Identifier::SpecialIdentifier::abi_encodeWithSelector,
                nullptr)),
            std::move(RawArguments));
        CallAbiEncodeWithSelector->setType(ArgTypes.at(1));
        auto ArgTypesAt1 = std::move(ArgTypes.at(1));
        ArgTypes.clear();
        ArgTypes.emplace_back(std::move(ArgTypesAt1));
        RawArguments.resize(1);
        RawArguments.at(0) = std::move(CallAbiEncodeWithSelector);
        break;
      }
      default: {
        break;
      }
      }
      if (!FTy) {
        I->setType(std::make_shared<FunctionType>(
            std::move(ArgTypes),
            std::vector<std::reference_wrapper<const TypePtr>>{*ReturnTy}));
        FTy = dynamic_cast<FunctionType *>(I->getType().get());
      }
    } else {
      if (auto MI = dynamic_cast<Identifier *>(Base)) {
        if (MI && MI->getSpecialIdentifier() !=
                      Identifier::SpecialIdentifier::this_) {
          assert(false && "only support external call and "
                          "SpecialIdentifier::this member call!");
          __builtin_unreachable();
        }
      }
      const Decl *D = I->getCorrespondDecl();
      if (auto ED = dynamic_cast<const EventDecl *>(D)) {
        FTy = dynamic_cast<FunctionType *>(ED->getType().get());
      } else if (auto FD = dynamic_cast<const FunctionDecl *>(D)) {
        FTy = dynamic_cast<FunctionType *>(FD->getType().get());
      } else if (dynamic_cast<const CallableVarDecl *>(D)) {
        // TODO: implement
        assert(false && "calleevar not supported yet");
        __builtin_unreachable();
      } else {
        assert(false && "callee is not FuncDecl");
        __builtin_unreachable();
      }
    }
  } else if (auto I = dynamic_cast<AsmIdentifier *>(E)) {
    if (!I->isResolved()) {
      return;
    }
    if (I->isSpecialIdentifier()) {
      FTy = dynamic_cast<FunctionType *>(I->getType().get());
    } else {
      const Decl *D = I->getCorrespondDecl();
      if (auto AFD = dynamic_cast<const AsmFunctionDecl *>(D)) {
        FTy = dynamic_cast<FunctionType *>(AFD->getType().get());
      } else {
        assert(false && "callee is not AsmFunctionDecl");
        __builtin_unreachable();
      }
    }
  } else {
    assert(false && "only support Identifier, member callee");
    __builtin_unreachable();
  }

  auto Args = CE.getArguments();
  auto &ArgTypes = FTy->getParamTypes();

  // TODO: implement overloading function
  assert(Args.size() <= ArgTypes.size());

  for (size_t I = 0; I < Args.size(); ++I) {
    if (auto *IC = dynamic_cast<ImplicitCastExpr *>(Args[I])) {
      Actions.resolveImplicitCast(*IC, ArgTypes[I], false);
    }
  }

  // TODO: refactor this
  // This assumes function only returns one value,
  // because tuple type not impl. yet.
  if (auto ReturnTy = FTy->getReturnTypes(); ReturnTy.size() == 1) {
    CE.setType(ReturnTy[0]);
  }
} // namespace

} // namespace

void Sema::resolveImplicitCast(ImplicitCastExpr &IC, TypePtr DstTy,
                               bool PrefereLValue) {
  if (IC.getCastKind() != CastKind::None) {
    return;
  }
  if (!DstTy) {
    return;
  }
  const auto &SrcTy = IC.getSubExpr()->getType();
  if (!SrcTy) {
    return;
  }
  if (SrcTy->getCategory() == Type::Category::Tuple) {
    assert(DstTy->getCategory() == Type::Category::Tuple ||
           DstTy->getCategory() == Type::Category::ReturnTuple);
    if (auto SrcTup = dynamic_cast<TupleExpr *>(IC.getSubExpr())) {
      auto DstTupTy = dynamic_cast<const TupleType *>(DstTy.get());
      std::size_t Num = SrcTup->getComponents().size();
      for (std::size_t Idx = 0; Idx < Num; ++Idx) {
        auto TIC =
            dynamic_cast<ImplicitCastExpr *>(SrcTup->getComponents()[Idx]);
        if (TIC)
          resolveImplicitCast(*TIC, DstTupTy->getElementTypes()[Idx],
                              PrefereLValue);
      }
    } else {
      assert(dynamic_cast<TypesTupleExpr *>(IC.getSubExpr()));
      // do nothing for TypesTupleExpr.
    }
  }
  if (DstTy->isEqual(*SrcTy)) {
    IC.setType(SrcTy);
    if (!PrefereLValue) {
      IC.setCastKind(CastKind::LValueToRValue);
    }
    return;
  }
  const bool Lossless = SrcTy->isImplicitlyConvertibleTo(*DstTy);
  IC.setType(DstTy);
  if (DstTy->getCategory() == Type::Category::Integer &&
      SrcTy->getCategory() == Type::Category::Integer) {
    if (!Lossless) {
      Diag(IC.getLocation().getBegin(), diag::warn_impcast_integer_precision)
          << SrcTy->getName() << DstTy->getName();
    }
    IC.setCastKind(CastKind::IntegralCast);
  } else if (DstTy->getCategory() == Type::Category::FixedBytes &&
             SrcTy->getCategory() == Type::Category::FixedBytes) {
    if (!Lossless) {
      Diag(IC.getLocation().getBegin(), diag::warn_impcast_integer_precision)
          << SrcTy->getName() << DstTy->getName();
    }
    IC.setCastKind(CastKind::FixedBytesCast);
  } else {
    IC.setCastKind(CastKind::TypeCast);
  }
}

void Sema::resolveType(SourceUnit &SU) {
  DeclTypeResolver DTR(*this);
  SU.accept(DTR);
}

} // namespace soll
