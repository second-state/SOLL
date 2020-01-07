// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"
#include <vector>

namespace soll {
namespace {

class TypeResolver : public StmtVisitor {
  TypePtr ReturnType;
  Sema &Actions;

public:
  TypeResolver(Sema &A) : Actions(A) {}
  void setReturnType(TypePtr Ty) { ReturnType = std::move(Ty); }
  Sema &getSema() { return Actions; }

  void visit(ReturnStmtType &IS) override {
    StmtVisitor::visit(IS);
    if (auto *IC = dynamic_cast<ImplicitCastExpr *>(IS.getRetValue())) {
      Actions.resolveImplicitCast(*IC, ReturnType, false);
    }
  }
  void visit(UnaryOperatorType &UO) override {
    StmtVisitor::visit(UO);
    UO.setType(UO.getSubExpr()->getType());
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
        if (LHSTy->isEqual(*RHSTy)) {
          LHS->setType(LHSTy);
          RHS->setType(LHSTy);
          BO.setType(LHSTy);
          return;
        }
        if (BO.isAssignmentOp() || BO.isCompoundAssignmentOp() ||
            RHSTy->isImplicitlyConvertibleTo(*LHSTy)) {
          LHS->setType(LHSTy);
          BO.setType(LHSTy);
          Actions.resolveImplicitCast(*RHS, LHSTy, false);
          return;
        }
        if (LHSTy->isImplicitlyConvertibleTo(*RHSTy)) {
          RHS->setType(RHSTy);
          BO.setType(RHSTy);
          Actions.resolveImplicitCast(*LHS, RHSTy, false);
          return;
        }
        Actions.Diag(BO.getLocation().getBegin(),
                     diag::err_typecheck_no_implicit_convertion)
            << LHSTy->getName() << RHSTy->getName();
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
          Ty = std::make_shared<AddressType>(StateMutability::Payable);
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
  void visit(AsmFunctionDeclStmtType &ADS) override {
    StmtVisitor::visit(ADS);
    if (auto *B = ADS.getBody()) {
      B->accept(*this);
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
    if (I->isSpecialIdentifier()) {
      FTy = dynamic_cast<FunctionType *>(I->getType().get());
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
  const auto &SrcTy = IC.getSubExpr()->getType();
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
