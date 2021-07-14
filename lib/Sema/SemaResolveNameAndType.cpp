// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <iostream>
using std::cerr;
using std::endl;

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
  BO.setType(BO.isComparisonOp() ? std::make_shared<BooleanType>() : Ty);
}

class DeclRegsiterHelper : public DeclVisitor {
  Sema &Actions;

public:
  DeclRegsiterHelper(Sema &SE) : Actions(SE) {}

  void Register(ContractDeclType &CD, bool IncludeInheritNodes) {
    cerr << "Add" << CD.getName().str() << '\n';
    Actions.addDecl(&CD);

    for (auto F : CD.getUsingForNodes()) {
      F->accept(*this);
    }
    if (IncludeInheritNodes)
      for (auto F : CD.getInheritNodes()) {
        F->accept(*this);
      }
    for (auto F : CD.getSubNodes()) {
      F->accept(*this);
    }
  }
};

class NameTypeResolverWarper;
class NameTypeResolver : public StmtVisitor {
  TypePtr ReturnType;
  Sema &Actions;
  ContractDecl *CurrentContract;
  NameTypeResolverWarper &NTRW;

public:
  NameTypeResolver(Sema &A, NameTypeResolverWarper &NTRW)
      : Actions(A), CurrentContract(nullptr), NTRW(NTRW) {}
  void setReturnType(TypePtr Ty) { ReturnType = std::move(Ty); }
  Sema &getSema() { return Actions; }

  void visit(BlockType &B) override {
    std::unique_ptr<Sema::SemaScope> BlockScope;
    if (!B.hasScope()) {
      BlockScope = std::make_unique<Sema::SemaScope>(&Actions, 0);
    }
    StmtVisitor::visit(B);
  }

  void visit(WhileStmtType &W) override {
    W.getCond()->accept(*this);
    {
      Sema::SemaScope WhileScope{&Actions,
                                 Scope::BreakScope | Scope::ContinueScope};
      W.getBody()->accept(*this);
    }
  }

  void visit(ForStmtType &F) override {
    if (F.getInit())
      F.getInit()->accept(*this);
    if (F.getCond())
      F.getCond()->accept(*this);
    if (F.getLoop())
      F.getLoop()->accept(*this);
    {
      Sema::SemaScope ForScope{&Actions,
                               Scope::BreakScope | Scope::ContinueScope};
      F.getBody()->accept(*this);
    }
  }

  void visit(DeclStmtType &DS) override;
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

  void visit(IdentifierType &I) override {
    if (I.isResolved())
      return;
    Decl *D = Actions.lookupName(I.getName());
    if (D == nullptr) {
      Actions.CurrentScope()->addUnresolvedExternal(&I);
      return;
    }
    if (auto SD = dynamic_cast<StructDecl *>(D)) {
      auto Ty = SD->getConstructorType();
      I.setType(Ty);
      I.setSpecialIdentifier(Identifier::SpecialIdentifier::struct_constructor);
      return;
    }
    I.setCorrespondDecl(D);
  }

  void visit(MemberExprType &ME) override ;

  void visit(AsmForStmtType &S) override {
    Sema::SemaScope ForScope{&Actions, 0};
    S.getInit()->accept(*this);
    S.getCond()->accept(*this);
    S.getLoop()->accept(*this);
    {
      Sema::SemaScope ForScope{&Actions,
                               Scope::BreakScope | Scope::ContinueScope};
      S.getBody()->accept(*this);
    }
  }

  void visit(AsmSwitchStmtType &AS) override {
    Sema::SemaScope SwitchScope{&Actions, 0};
    StmtVisitor::visit(AS);
  }

  void visit(AsmFunctionDeclStmtType &FDS) override;

  void visit(AsmIdentifierType &AI) override {
    StmtVisitor::visit(AI);
    if (AI.isResolved())
      return;
    Decl *D = Actions.lookupName(AI.getName());
    if (D == nullptr) {
      if (AI.isCall()) {
        Actions.CurrentScope()->addUnresolved(&AI);
      } else {
        Actions.CurrentScope()->addUnresolvedExternal(&AI);
      }
      return;
    }
    AI.setCorrespondDecl(D);
  }
};

class NameTypeResolverWarper : public DeclVisitor {
  Sema &Actions;
  DeclRegsiterHelper Register;
  NameTypeResolver NTR;

  ContractDecl *CurrentContract;

public:
  NameTypeResolverWarper(Sema &SE)
      : Actions(SE), Register(SE), NTR(SE, *this), CurrentContract(nullptr) {}
  ContractDeclType *getCurrentContract() const { return CurrentContract; }
  TypePtr handleUnresolveType(UnresolveType *UT) {
    Decl *D = Actions.lookupName(UT->getIdentifierName());
    if (auto *SD = dynamic_cast<StructDecl *>(D))
      return SD->getType();
    else if (auto *CD = dynamic_cast<ContractDecl *>(D)) {
      return CD->getType();
    }
    if (auto *CD = Actions.lookupContractDeclName(UT->getIdentifierName())) {
      return CD->getType();
    }
    __builtin_unreachable();
  }

  void visit(SourceUnitType &SU) override {
    Sema::SemaScope SourceUnitScope{&Actions, 0};
    DeclVisitor::visit(SU);
  }

  void visit(UsingForType &UF) override {
    auto LibraryIdentifierPath = UF.getLibraryName();
    for (auto LibraryIdentifier : LibraryIdentifierPath.getPath()) {
      auto Lib = Actions.lookupContractDeclName(LibraryIdentifier);
      assert(Lib->getKind() == ContractDecl::ContractKind::Library &&
             "Not a Library!");
      UF.addLibrary(Lib);
    }
    if (auto *UT = dynamic_cast<UnresolveType *>(UF.getType().get())) {
      UF.setType(handleUnresolveType(UT));
    }
  }

  void visit(ContractDeclType &CD) override {
    Sema::SemaScope ContractScope{&Actions, 0};
    CurrentContract = &CD;
    Register.Register(CD, true);
    DeclVisitor::visit(CD);
    CurrentContract = nullptr;
  }

  void visit(FunctionDecl &FD) override {
    Actions.addDecl(&FD);
    {
      Sema::SemaScope ArgumentScope{&Actions, 0};
      DeclVisitor::visit(FD);
      {
        Sema::SemaScope FunctionScope{&Actions, Scope::FunctionScope};

        if (auto *Ty = dynamic_cast<FunctionType *>(FD.getType().get())) {
          if (Ty->getReturnTypes().empty()) {
            NTR.setReturnType(nullptr);
          } else {
            /// TODO: implement multiple return type
            assert(Ty->getReturnTypes().size() == 1 &&
                   "multiple return type not implemented");
            NTR.setReturnType(Ty->getReturnTypes().front());
          }
        }

        // abstract function do not have body
        if (FD.getBody())
          FD.getBody()->accept(NTR);

        NTR.setReturnType(nullptr);
      }
    }
  }

  void visit(EventDeclType &ED) override {
    Actions.addDecl(&ED);
    DeclVisitor::visit(ED);
  }

  void visit(VarDecl &VD) override {
    Actions.addDecl(&VD);
    if (auto *UT = dynamic_cast<UnresolveType *>(VD.getType().get())) {
      VD.setType(handleUnresolveType(UT));
    }
    if (VD.getValue()) {
      VD.getValue()->accept(NTR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.getValue())) {
        NTR.getSema().resolveImplicitCast(*IC, VD.getType(), false);
      }
    }
    DeclVisitor::visit(VD);
  }

  void visit(AsmVarDeclType &VD) override {
    if (VD.getValue()) {
      VD.getValue()->accept(NTR);
      if (auto *IC = dynamic_cast<ImplicitCastExpr *>(VD.getValue())) {
        NTR.getSema().resolveImplicitCast(*IC, VD.getType(), false);
      }
    }
    DeclVisitor::visit(VD);
  }

  void visit(YulCodeType &C) override {
    C.getBody()->accept(NTR);
    DeclVisitor::visit(C);
  }
};

void NameTypeResolver::visit(DeclStmtType &DS) {
  StmtVisitor::visit(DS);
  auto VarDecls = DS.getVarDecls();
  for (auto VD : VarDecls) {
    VD->accept(NTRW);
  }
  if (DS.getValue()) {
    DS.getValue()->accept(*this);
    if (auto *IC = dynamic_cast<ImplicitCastExpr *>(DS.getValue())) {
      IC->accept(*this);
      Actions.resolveImplicitCast(*IC, DS.getVarDecls().front()->getType(),
                                  false);
    }
  }
}

void NameTypeResolver::visit(CallExprType &CE) {
  StmtVisitor::visit(CE);

  Expr *E = CE.getCalleeExpr(), *Base = nullptr;
  MemberExpr *ME = nullptr;
  std::string FunctionSignature;
  if (ME = dynamic_cast<MemberExpr *>(E)) {
    auto Name = ME->getName();
    E = Name;
    Base = ME->getBase();
    auto calculateFunctionSignature = [&]() {
      FunctionSignature = ME->getName()->getName().str() + "(";
      auto FTy = dynamic_cast<FunctionType *>(ME->getType().get());
      assert(FTy->getReturnTypes().empty() &&
             "Only support functions without return value now!");
      auto &ParamTypes = FTy->getParamTypes();
      bool First = true;
      for (auto PTy : ParamTypes) {
        if (!First)
          FunctionSignature += ",";
        First = false;
        FunctionSignature += PTy->getSignatureEncoding();
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
  }

  CE.resolveNamedCall();

  FunctionType *FTy = nullptr;
  if (auto I = dynamic_cast<Identifier *>(E)) {
    if (!I->isResolved()) {
      return;
    }
    if (I->isSpecialIdentifier()) {
      std::vector<TypePtr> ArgTypes;
      for (const auto &arg : CE.getArguments()) {
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(arg)) {
          ArgTypes.emplace_back(IC->getSubExpr()->getType());
        }
      }
      switch (I->getSpecialIdentifier()) {
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
              *IC, std::make_shared<AddressType>(StateMutability::Payable),
              false);
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
        ArgTypes.insert(ArgTypes.begin(), std::make_shared<StringType>());
        SignatureStringLiteral->setType(ArgTypes.at(0));
        auto Signature = Actions.CreateDummy(std::move(SignatureStringLiteral));
        auto &RawArguments = CE.getRawArguments();
        RawArguments.insert(RawArguments.begin(), std::move(Signature));
        [[fallthrough]];
      }
      /* abi.encodeWithSignature(string signature, ...) ==
       * abi.encodeWithSelector(bytes4(keccak256(bytes(signature))), ...) */
      case Identifier::SpecialIdentifier::abi_encodeWithSignature: {
        auto &Signature = CE.getRawArguments().at(0);
        if (auto *IC = dynamic_cast<ImplicitCastExpr *>(Signature.get())) {
          Actions.resolveImplicitCast(*IC, std::make_shared<BytesType>(),
                                      false);
        }
        std::vector<ExprPtr> Keccak256Arguments;
        Keccak256Arguments.emplace_back(std::move(Signature));
        auto CallKeccak256 = std::make_unique<CallExpr>(
            SourceRange(),
            std::move(std::make_unique<Identifier>(
                Token(), Identifier::SpecialIdentifier::keccak256, nullptr)),
            std::move(Keccak256Arguments));
        CallKeccak256->setType(
            std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32));
        Signature = Actions.CreateDummy(std::move(CallKeccak256));
        [[fallthrough]];
      }
      /* abi.encodeWithSelector(bytes4 selector, ...) ==
       * abi.encodePacked(selector, abi.encode(...)); */
      case Identifier::SpecialIdentifier::abi_encodeWithSelector: {
        ArgTypes.resize(2);
        ArgTypes.at(0) =
            std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B4);
        ArgTypes.at(1) = std::make_shared<BytesType>();
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
        ArgTypes.at(0) = ArgTypes.at(1);
        ArgTypes.resize(1);
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
            std::vector<TypePtr>(ArgTypes),
            std::vector<TypePtr>{std::make_shared<BytesType>()}));
        FTy = dynamic_cast<FunctionType *>(I->getType().get());
      }
    } else {
      if (auto MI = dynamic_cast<Identifier *>(Base)) {
        if (MI && MI->isSpecialIdentifier() &&
            MI->getSpecialIdentifier() !=
                Identifier::SpecialIdentifier::this_ &&
            MI->getSpecialIdentifier() !=
                Identifier::SpecialIdentifier::super_) {
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
}

void NameTypeResolver::visit(MemberExprType &M) {
  // NameCheckByPass prevent raise a alert when M.name() is unsolved
  // It is usually used when M.base is special case.

  bool NameCheckByPass = false;
  bool BaseCheckByPass = false;
  bool IsLibraryAccess = false;

  auto Base = M.getBase();
  Base->accept(*this);

  {
    Sema::SemaScope MemberAccessScope{&Actions, 0, false};

    if (Base->getType() == nullptr) {
      if (auto I = dynamic_cast<Identifier *>(Base)) {
        auto BaseName = I->getName().str();
        const std::vector<std::string> GlobalPassList{"block", "msg", "tx"};

        if (std::find(GlobalPassList.begin(), GlobalPassList.end(), BaseName) !=
            GlobalPassList.end()) {
          BaseCheckByPass = true;
        } else {
          // TODO: we should handle `address` type as base.
          BaseCheckByPass = true;
        }
      }
      if (!BaseCheckByPass) {
        assert(false && "Base can not be unsolved!");
      }
      return;
    }

    switch (Base->getType()->getCategory()) {
    case Type::Category::Contract: {
      auto I = dynamic_cast<Identifier *>(Base);
      assert(I && "Contract name must be a Identifier");

      if (I->isSpecialIdentifier()) {
        switch (I->getSpecialIdentifier()) {
        case Identifier::SpecialIdentifier::this_:
          for (auto F : NTRW.getCurrentContract()->getFuncs())
            Actions.addDecl(F);
          break;
        case Identifier::SpecialIdentifier::super_: {
          auto Bases = NTRW.getCurrentContract()->getResolvedBaseContracts();
          if (Bases.size() > 1) {
            Actions.Diag(Base->getLocation().getBegin(),
                         diag::err_super_in_noninheritance_contract);
          }
          for (auto F : Bases[1]->getFuncs())
            Actions.addDecl(F);
        } break;
        default:
          assert(I && "Unimplement SpecialIdentifier Base");
        }
      } else if (auto C =
                     dynamic_cast<ContractDecl *>(I->getCorrespondDecl())) {
        // Inheritance call with Contract name
        for (auto F : C->getFuncs())
          Actions.addDecl(F);
      } else if (auto CT = dynamic_cast<ContractType *>(I->getType().get())) {
        // Contract external call
        if (CT->getDecl()) {
          for (auto F : CT->getDecl()->getFuncs())
            Actions.addDecl(F);
        } else if (I->getName() == "this") {
          for (auto F : NTRW.getCurrentContract()->getFuncs())
            Actions.addDecl(F);
        }
      }
    } break;
    case Type::Category::Struct: {
      if (auto ST = dynamic_cast<StructType *>(Base->getType().get())) {
        auto Types = ST->getElementTypes();
        auto ElementName = M.getName()->getName().str();

        if (ST->hasElement(ElementName)) {
          // Note: struct do not have sub-decl yet. check name only.
          M.setType(Types[ST->getElementIndex(ElementName)]);
          NameCheckByPass = true;
          return;
        } else {
          Actions.Diag(M.getName()->getLocation().getBegin(),
                       diag::err_no_member)
              << ElementName << M.getBase()->getType()->getName();
        }
      }
    } break;
    case Type::Category::Address: {
      // TODO: Handle Address case
      // TODO: Address.send(x), should bypass send and check x.
      NameCheckByPass = true;
      // XXX: Fix alert for a.send()
      return;
    } break;
    default:
      IsLibraryAccess = true;
      break;
    }

    if (!(IsLibraryAccess || NameCheckByPass))
      M.getName()->accept(*this);
  }

  if (IsLibraryAccess) {
    // consider as a normal function call
    M.getName()->accept(*this);
  } else if (!NameCheckByPass && !M.getName()->isResolved()) {
    // Note: MemberExpr do not allow resolve in outside scope
    Actions.Diag(M.getName()->getLocation().getBegin(),
                 diag::err_undeclared_var_use)
        << M.getName()->getName();
  }
}

void NameTypeResolver::visit(AsmFunctionDeclStmtType &FDS) {
  StmtVisitor::visit(FDS);
  if (auto *D = FDS.getDecl()) {
    FDS.getDecl()->accept(NTRW);
    if (auto *B = D->getBody()) {
      B->accept(*this);
    }
  }
}

} // namespace

void Sema::resolveNameAndType(SourceUnit &SU) {
  NameTypeResolverWarper NTRW(*this);
  SU.accept(NTRW);
  return;
}
} // namespace soll