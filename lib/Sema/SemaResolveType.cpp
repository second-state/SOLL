// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
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

bool isAllowedForTypecast(const Type *In, const Type *Out, bool IsLiteral, const Expr *SE) {
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
    assert (TupleE);
    if (InT->getElementTypes().size() != OutT->getElementTypes().size()) {
      return false;
    }

    size_t Size = InT->getElementTypes().size();
    bool Result = true;
    for (size_t Idx = 0; Idx < Size; ++Idx) {
      if (InT->getElementTypes()[Idx]) {
        if (OutT->getElementTypes()[Idx]) {
          auto ICExpr = dynamic_cast<const ImplicitCastExpr *>(TupleE->getComponents()[Idx]);
          assert(ICExpr);
          auto CompExpr = ICExpr->getSubExpr();
          const bool IsLiteral = dynamic_cast<const NumberLiteral *>(CompExpr) ||
                                 dynamic_cast<const StringLiteral *>(CompExpr);
          Result &=
              isAllowedForTypecast(InT->getElementTypes()[Idx].get(),
                                   OutT->getElementTypes()[Idx].get(), IsLiteral, CompExpr);
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
  BO.setType(BO.isComparisonOp() ? std::make_shared<BooleanType>() : Ty);
}

class TypeResolver : public StmtVisitor {
  TypePtr ReturnType;
  Sema &Actions;

public:
  TypeResolver(Sema &A) : Actions(A) {}
  void setReturnType(TypePtr Ty) { ReturnType = std::move(Ty); }
  Sema &getSema() { return Actions; }

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

    UO.setType(UO.getOpcode() == UO_IsZero ? std::make_shared<BooleanType>()
                                           : Ty);
  }
  void visit(BinaryOperatorType &BO) override {
    StmtVisitor::visit(BO);
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
      IA.setType(
          std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B1));
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
          Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
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
          Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
          break;
        case Identifier::SpecialIdentifier::address_transfer:
          Ty = std::make_shared<FunctionType>(
              std::vector<TypePtr>{
                  std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
              std::vector<TypePtr>{});
          break;
        case Identifier::SpecialIdentifier::address_send:
          Ty = std::make_shared<FunctionType>(
              std::vector<TypePtr>{
                  std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
              std::vector<TypePtr>{std::make_shared<BooleanType>()});
          break;
        case Identifier::SpecialIdentifier::address_call:
        case Identifier::SpecialIdentifier::address_delegatecall:
        case Identifier::SpecialIdentifier::address_staticcall:
          Ty = std::make_shared<FunctionType>(
              std::vector<TypePtr>{std::make_shared<BytesType>()},
              std::vector<TypePtr>{std::make_shared<BooleanType>(),
                                   std::make_shared<BytesType>()});
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
    default:
      break;
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
        Actions.resolveImplicitCast(*IC, DS.getVarDecls().front()->GetType(),
                                    false);
      }
    }
  }
  void visit(AsmFunctionDeclStmtType &FDS) override {
    StmtVisitor::visit(FDS);
    if (auto *D = FDS.getDecl()) {
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

public:
  DeclTypeResolver(Sema &S) : TR(S) {}
  void visit(FunctionDecl &FD) override {
    if (auto *Ty = dynamic_cast<FunctionType *>(FD.getType().get())) {
      if (Ty->getReturnTypes().empty()) {
        TR.setReturnType(nullptr);
      } else {
        /// TODO: implement multiple return type
        assert(Ty->getReturnTypes().size() == 1 &&
               "multiple return type not implemented");
        TR.setReturnType(Ty->getReturnTypes().front());
      }
      FD.getBody()->accept(TR);
      TR.setReturnType(nullptr);
    }
    DeclVisitor::visit(FD);
  }
  void visit(VarDecl &VD) override {
    if (VD.GetValue()) {
      VD.GetValue()->accept(TR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.GetValue())) {
        TR.getSema().resolveImplicitCast(*IC, VD.GetType(), false);
      }
    }
    DeclVisitor::visit(VD);
  }
  void visit(AsmVarDeclType &VD) override {
    if (VD.GetValue()) {
      VD.GetValue()->accept(TR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.GetValue())) {
        TR.getSema().resolveImplicitCast(*IC, VD.GetType(), false);
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
  StmtVisitor::visit(CE);

  Expr *E = CE.getCalleeExpr(), *Base = nullptr;
  if (auto M = dynamic_cast<MemberExpr *>(E)) {
    E = M->getName();
    Base = M->getBase();
  }

  FunctionType *FTy = nullptr;
  if (auto I = dynamic_cast<Identifier *>(E)) {
    if (!I->isResolved()) {
      return;
    }
    if (I->isSpecialIdentifier()) {
      std::vector<TypePtr> argTypes;
      for (const auto &arg : CE.getArguments()) {
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(arg)) {
          argTypes.emplace_back(IC->getSubExpr()->getType());
        }
      }
      switch (I->getSpecialIdentifier()) {
      case Identifier::SpecialIdentifier::abi_encodePacked:
      case Identifier::SpecialIdentifier::abi_encode:
        break;
      case Identifier::SpecialIdentifier::abi_encodeWithSelector:
        argTypes.at(0) =
            std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B4);
        break;
      case Identifier::SpecialIdentifier::abi_encodeWithSignature:
        argTypes.at(0) = std::make_shared<StringType>();
        break;
      default:
        FTy = dynamic_cast<FunctionType *>(I->getType().get());
      }
      if (!FTy) {
        I->setType(std::make_shared<FunctionType>(
            std::vector<TypePtr>(argTypes),
            std::vector<TypePtr>{std::make_shared<BytesType>()}));
        FTy = dynamic_cast<FunctionType *>(I->getType().get());
      }
    } else {
      if (auto MI = dynamic_cast<Identifier *>(Base)) {
        if (MI && MI->getSpecialIdentifier() !=
                      Identifier::SpecialIdentifier::this_) {
          // TODO: we need to support other type with member call
          assert(false &&
                 "only support SpecialIdentifier::this member call now!");
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
}

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
    assert(DstTy->getCategory() == Type::Category::Tuple);
    auto SrcTup = dynamic_cast<TupleExpr *>(IC.getSubExpr());
    auto DstTupTy = dynamic_cast<const TupleType *>(DstTy.get());
    std::size_t Num = SrcTup->getComponents().size();
    for (std::size_t Idx = 0; Idx < Num; ++Idx) {
      auto TIC = dynamic_cast<ImplicitCastExpr *>(SrcTup->getComponents()[Idx]);
      if (TIC)
        resolveImplicitCast(*TIC, DstTupTy->getElementTypes()[Idx],
                            PrefereLValue);
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
  } else {
    IC.setCastKind(CastKind::TypeCast);
  }
}

void Sema::resolveType(SourceUnit &SU) {
  DeclTypeResolver DTR(*this);
  SU.accept(DTR);
}

} // namespace soll
