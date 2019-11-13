// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Sema.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Lex/Lexer.h"
#include "soll/Sema/Scope.h"

namespace soll {

/// handle int arithemetic converisons
/// helper function of UsualArithmeticConversions
static TypePtr handleIntegerConversion(Sema &S, ExprPtr &LHS, ExprPtr &RHS,
                                       bool isCompAssign) {
  assert(LHS->getType() != nullptr);
  assert(RHS->getType() != nullptr);
  auto LTy = dynamic_cast<IntegerType *>(LHS->getType().get());
  auto RTy = dynamic_cast<IntegerType *>(RHS->getType().get());

  if (LTy->getKind() == RTy->getKind())
    return LHS->getType();
  auto commonTy = IntegerType::common(*LTy, *RTy);

  if (commonTy->getKind() != LTy->getKind())
    LHS = std::make_unique<ImplicitCastExpr>(std::move(LHS),
                                             CastKind::IntegralCast, commonTy);
  if (commonTy->getKind() != RTy->getKind())
    RHS = std::make_unique<ImplicitCastExpr>(std::move(RHS),
                                             CastKind::IntegralCast, commonTy);
  return commonTy;
}

static void TryImplicitCast(TypePtr DstTy, ExprPtr &Vaule) {
  auto SrcTy = Vaule->getType();
  if (SrcTy->isImplicitlyConvertibleTo(*DstTy)) {
    if (DstTy->getCategory() == Type::Category::Integer &&
        SrcTy->getCategory() == Type::Category::Integer) {
      auto DITy = static_cast<IntegerType *>(DstTy.get());
      auto SITy = static_cast<IntegerType *>(SrcTy.get());
      if (SITy->getKind() != DITy->getKind())
        Vaule = std::make_unique<ImplicitCastExpr>(
            std::move(Vaule), CastKind::IntegralCast, DstTy);
      // TODO: signed/unsigned check
    }
    // TODO: other type conversion such as address->int
  } else {
    assert(false && "no implicit conversion");
  }
}

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()) {}

StmtPtr Sema::CreateReturnStmt() { return std::make_unique<ReturnStmt>(); }
StmtPtr Sema::CreateReturnStmt(ExprPtr &&Value) {
  TryImplicitCast(FunRtnTys.front(), Value);
  return std::make_unique<ReturnStmt>(
      Sema::DefaultLvalueConversion(std::move(Value)));
}

std::unique_ptr<FunctionDecl> Sema::CreateFunctionDecl(
    llvm::StringRef Name, FunctionDecl::Visibility Vis, StateMutability SM,
    bool IsConstructor, bool IsFallback, std::unique_ptr<ParamList> &&Params,
    std::vector<std::unique_ptr<ModifierInvocation>> &&Modifiers,
    std::unique_ptr<ParamList> &&ReturnParams, std::unique_ptr<Block> &&Body) {
  return std::make_unique<FunctionDecl>(
      Name, Vis, SM, IsConstructor, IsFallback, std::move(Params),
      std::move(Modifiers), std::move(ReturnParams), std::move(Body));
}

std::unique_ptr<EventDecl>
Sema::CreateEventDecl(llvm::StringRef Name, std::unique_ptr<ParamList> &&Params,
                      bool Anonymous) {
  auto ED = std::make_unique<EventDecl>(Name, std::move(Params), Anonymous);
  return ED;
}

ExprPtr Sema::CreateBinOp(BinaryOperatorKind Opc, ExprPtr &&LHS,
                          ExprPtr &&RHS) {
  TypePtr ResultTy;
  if (BinaryOperator::isAdditiveOp(Opc)) {
    ResultTy = CheckAdditiveOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isMultiplicativeOp(Opc)) {
    ResultTy = CheckMultiplicativeOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isComparisonOp(Opc)) {
    ResultTy = CheckCompareOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isLogicalOp(Opc)) {
    ResultTy = CheckLogicalOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isBitwiseOp(Opc)) {
    ResultTy = CheckBitwiseOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isShiftOp(Opc)) {
    ResultTy = CheckShiftOperands(LHS, RHS, Opc);
  } else if (BinaryOperator::isAssignmentOp(Opc)) {
    // TODO: get common type
    ResultTy = LHS->getType();
    switch (Opc) {
    case BinaryOperatorKind::BO_AddAssign: {
      CheckAdditiveOperands(LHS, RHS, Opc, ResultTy);
      break;
    }
    default:;
    }
    CheckAssignmentOperands(LHS, RHS, ResultTy);
  }
  // TODO: a lot of binary operations
  return std::move(std::make_unique<BinaryOperator>(
      std::move(LHS), std::move(RHS), Opc, ResultTy));
}

ExprPtr Sema::CreateIndexAccess(ExprPtr &&LHS, ExprPtr &&RHS) {
  // TODO check whether LHS is LValue
  RHS = DefaultLvalueConversion(std::move(RHS));

  TypePtr ResultTy;
  const Type *BaseTy = LHS->getType().get();
  if (auto MT = dynamic_cast<const MappingType *>(BaseTy)) {
    TryImplicitCast(MT->getKeyType(), RHS);
    ResultTy = MT->getValueType();
  } else if (auto AT = dynamic_cast<const ArrayType *>(BaseTy)) {
    // TODO: replace this with Diagonistic
    assert(RHS->getType()->getCategory() == Type::Category::Integer);
    TryImplicitCast(std::make_shared<IntegerType>(IntegerType::IntKind::U256),
                    RHS);
    ResultTy = AT->getElementType();
  } else {
    assert(false);
  }
  return std::make_unique<IndexAccess>(std::move(LHS), std::move(RHS),
                                       ResultTy);
}

std::unique_ptr<CallExpr> Sema::CreateCallExpr(ExprPtr &&Callee,
                                               std::vector<ExprPtr> &&Args) {
  TypePtr ResultTy = Callee->getType();
  FunctionType *FTy = nullptr;
  Expr *expr = Callee.get();
  if (auto M = dynamic_cast<MemberExpr *>(Callee.get())) {
    expr = M->getName();
  }
  if (auto I = dynamic_cast<Identifier *>(expr)) {
    if (I->isSpecialIdentifier()) {
      FTy = dynamic_cast<FunctionType *>(I->getType().get());
    } else {
      const Decl *D = I->getCorrespondDecl();
      if (dynamic_cast<const EventDecl *>(D)) {
        return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                          nullptr);
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

  // TODO: refactor this
  // This assumes function only returns one value,
  // because tuple type not impl. yet.
  if (auto ReturnTy = FTy->getReturnTypes(); ReturnTy.size() == 1) {
    ResultTy = ReturnTy[0];
  }

  for (ExprPtr &Arg : Args) {
    Arg = DefaultLvalueConversion(std::move(Arg));
  }
  return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                    std::move(ResultTy));
}

std::unique_ptr<Identifier> Sema::CreateIdentifier(Token Tok) {
  static const llvm::StringMap<Identifier::SpecialIdentifier> SpecialLookup{
      {"abi", Identifier::SpecialIdentifier::abi},
      {"block", Identifier::SpecialIdentifier::block},
      {"blockhash", Identifier::SpecialIdentifier::blockhash},
      {"gasleft", Identifier::SpecialIdentifier::gasleft},
      {"msg", Identifier::SpecialIdentifier::msg},
      {"now", Identifier::SpecialIdentifier::now},
      {"tx", Identifier::SpecialIdentifier::tx},
      {"assert", Identifier::SpecialIdentifier::assert_},
      {"require", Identifier::SpecialIdentifier::require},
      {"revert", Identifier::SpecialIdentifier::revert},
      {"addmod", Identifier::SpecialIdentifier::addmod},
      {"mulmod", Identifier::SpecialIdentifier::mulmod},
      {"keccak256", Identifier::SpecialIdentifier::keccak256},
      {"sha256", Identifier::SpecialIdentifier::sha256},
      {"ripemd160", Identifier::SpecialIdentifier::ripemd160},
      {"ecrecover", Identifier::SpecialIdentifier::ecrecover},
  };
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  if (auto Iter = SpecialLookup.find(Name); Iter != SpecialLookup.end()) {
    TypePtr Ty;
    switch (Iter->second) {
    case Identifier::SpecialIdentifier::block:
    case Identifier::SpecialIdentifier::msg:
    case Identifier::SpecialIdentifier::tx:
    case Identifier::SpecialIdentifier::abi:
      Ty = std::make_shared<ContractType>();
      break;
    case Identifier::SpecialIdentifier::now:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      break;
    case Identifier::SpecialIdentifier::blockhash:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
          std::vector<TypePtr>{
              std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32)});
      break;
    case Identifier::SpecialIdentifier::gasleft:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case Identifier::SpecialIdentifier::assert_:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<BooleanType>()},
          std::vector<TypePtr>{});
      break;
    case Identifier::SpecialIdentifier::require:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<BooleanType>(),
                               std::make_shared<StringType>()},
          std::vector<TypePtr>{});
      break;
    case Identifier::SpecialIdentifier::revert:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<StringType>()},
          std::vector<TypePtr>{});
      break;
    case Identifier::SpecialIdentifier::addmod:
    case Identifier::SpecialIdentifier::mulmod:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case Identifier::SpecialIdentifier::keccak256:
    case Identifier::SpecialIdentifier::sha256:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<BytesType>()},
          std::vector<TypePtr>{
              std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32)});
      break;
    case Identifier::SpecialIdentifier::ripemd160:
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<BytesType>()},
          std::vector<TypePtr>{
              std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B20)});
      break;
    case Identifier::SpecialIdentifier::ecrecover:
      Ty = std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B32);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{
              Ty, std::make_shared<IntegerType>(IntegerType::IntKind::U256), Ty,
              Ty},
          std::vector<TypePtr>{
              std::make_shared<AddressType>(StateMutability::NonPayable)});
      break;
    default:
      assert(false && "unknown special identifier");
      __builtin_unreachable();
    }
    return std::make_unique<Identifier>(Name, Iter->second, Ty);
  }
  Decl *D = lookupName(Name);
  if (D == nullptr) {
    Diag(Tok.getLocation(), diag::err_undefined_variable);
    return nullptr;
  }
  return std::make_unique<Identifier>(Name, D);
}

std::unique_ptr<MemberExpr>
Sema::CreateMemberExpr(std::unique_ptr<Expr> &&BaseExpr, Token Tok) {
  static const llvm::StringMap<Identifier::SpecialIdentifier> Lookup{
      {"decode", Identifier::SpecialIdentifier::abi_decode},
      {"encode", Identifier::SpecialIdentifier::abi_encode},
      {"encodePacked", Identifier::SpecialIdentifier::abi_encodePacked},
      {"encodeWithSelector",
       Identifier::SpecialIdentifier::abi_encodeWithSelector},
      {"encodeWithSignature",
       Identifier::SpecialIdentifier::abi_encodeWithSignature},
      {"coinbase", Identifier::SpecialIdentifier::block_coinbase},
      {"difficulty", Identifier::SpecialIdentifier::block_difficulty},
      {"gaslimit", Identifier::SpecialIdentifier::block_gaslimit},
      {"number", Identifier::SpecialIdentifier::block_number},
      {"timestamp", Identifier::SpecialIdentifier::block_timestamp},
      {"data", Identifier::SpecialIdentifier::msg_data},
      {"sender", Identifier::SpecialIdentifier::msg_sender},
      {"sig", Identifier::SpecialIdentifier::msg_sig},
      {"value", Identifier::SpecialIdentifier::msg_value},
      {"gasprice", Identifier::SpecialIdentifier::tx_gasprice},
      {"origin", Identifier::SpecialIdentifier::tx_origin},
  };
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
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  const Expr *Base = BaseExpr.get();
  if (auto *I = dynamic_cast<const Identifier *>(Base)) {
    if (I->isSpecialIdentifier()) {
      if (auto Iter = Lookup.find(Name); Iter != Lookup.end()) {
        std::shared_ptr<Type> Ty;
        switch (I->getSpecialIdentifier()) {
        case Identifier::SpecialIdentifier::abi:
          switch (Iter->second) {
          case Identifier::SpecialIdentifier::abi_decode:
          case Identifier::SpecialIdentifier::block_difficulty:
          case Identifier::SpecialIdentifier::block_gaslimit:
          case Identifier::SpecialIdentifier::block_number:
          case Identifier::SpecialIdentifier::block_timestamp:
            Diag(Tok.getLocation(), diag::err_unimplemented_identifier)
                << Tok.getIdentifierInfo();
            return nullptr;
            break;
          default:
            assert(false && "unknown member");
            __builtin_unreachable();
          }
          break;
        case Identifier::SpecialIdentifier::block:
          switch (Iter->second) {
          case Identifier::SpecialIdentifier::block_coinbase:
            Ty = std::make_shared<AddressType>(StateMutability::Payable);
            break;
          case Identifier::SpecialIdentifier::block_difficulty:
          case Identifier::SpecialIdentifier::block_gaslimit:
          case Identifier::SpecialIdentifier::block_number:
          case Identifier::SpecialIdentifier::block_timestamp:
            Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
            break;
          default:
            assert(false && "unknown member");
            __builtin_unreachable();
          }
          break;
        case Identifier::SpecialIdentifier::msg:
          switch (Iter->second) {
          case Identifier::SpecialIdentifier::msg_data:
            Ty = std::make_shared<BytesType>();
            break;
          case Identifier::SpecialIdentifier::msg_sender:
            Ty = std::make_shared<AddressType>(StateMutability::Payable);
            break;
          case Identifier::SpecialIdentifier::msg_sig:
            Ty = std::make_shared<FixedBytesType>(FixedBytesType::ByteKind::B4);
            break;
          case Identifier::SpecialIdentifier::msg_value:
            Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
            break;
          default:
            assert(false && "unknown member");
            __builtin_unreachable();
          }
          break;
        case Identifier::SpecialIdentifier::tx:
          switch (Iter->second) {
          case Identifier::SpecialIdentifier::tx_gasprice:
            Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
            break;
          case Identifier::SpecialIdentifier::tx_origin:
            Ty = std::make_shared<AddressType>(StateMutability::Payable);
            break;
          default:
            assert(false && "unknown member");
            __builtin_unreachable();
          }
          break;
        default:
          __builtin_unreachable();
        }
        return std::make_unique<MemberExpr>(
            std::move(BaseExpr),
            std::make_unique<Identifier>(Name, Iter->second, Ty));
      }
    } else {
      if (auto VD = dynamic_cast<const VarDecl *>(I->getCorrespondDecl())) {
        switch (VD->GetType()->getCategory()) {
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
            return std::make_unique<MemberExpr>(
                std::move(BaseExpr),
                std::make_unique<Identifier>(Name, Iter->second, Ty));
          } else {
          }
          break;
        case Type::Category::Address:
          if (auto Iter = AddressLookup.find(Name); Iter != Lookup.end()) {
            std::shared_ptr<Type> Ty;
            switch (Iter->second) {
            case Identifier::SpecialIdentifier::address_balance:
              Ty = std::make_shared<AddressType>(StateMutability::Payable);
              break;
            case Identifier::SpecialIdentifier::address_transfer:
              Ty = std::make_shared<FunctionType>(
                  std::vector<TypePtr>{std::make_shared<IntegerType>(
                      IntegerType::IntKind::U256)},
                  std::vector<TypePtr>{});
              break;
            case Identifier::SpecialIdentifier::address_send:
              Ty = std::make_shared<FunctionType>(
                  std::vector<TypePtr>{std::make_shared<IntegerType>(
                      IntegerType::IntKind::U256)},
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
            return std::make_unique<MemberExpr>(
                std::move(BaseExpr),
                std::make_unique<Identifier>(Name, Iter->second, Ty));
          } else {
          }
          break;
        default:;
        }
      }
    }
  }
  assert(false && "normal member is not supported yet");
  __builtin_unreachable();
}

TypePtr Sema::CheckAdditiveOperands(ExprPtr &LHS, ExprPtr &RHS,
                                    BinaryOperatorKind Opc, TypePtr CompLHSTy) {
  TypePtr compType = UsualArithmeticConversions(LHS, RHS, CompLHSTy != nullptr);
  // TODO: replace this, should impl. Additive Diagonostic
  assert(LHS->getType()->getCategory() == Type::Category::Integer);
  assert(RHS->getType()->getCategory() == Type::Category::Integer);

  return compType;
}

TypePtr Sema::CheckMultiplicativeOperands(ExprPtr &LHS, ExprPtr &RHS,
                                          BinaryOperatorKind Opc,
                                          TypePtr CompLHSTy) {
  TypePtr compType = UsualArithmeticConversions(LHS, RHS, CompLHSTy != nullptr);
  // TODO: replace this, should impl. Multiplicative Diagonostic
  assert(LHS->getType()->getCategory() == Type::Category::Integer);
  assert(RHS->getType()->getCategory() == Type::Category::Integer);

  return compType;
}

TypePtr Sema::CheckShiftOperands(ExprPtr &LHS, ExprPtr &RHS,
                                 BinaryOperatorKind Opc, bool IsCompAssign) {
  if (!IsCompAssign)
    LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Integer &&
         "LHS is not int.");
  assert(RHS->getType()->getCategory() == Type::Category::Integer &&
         "RHS is not int.");
  return LHS->getType();
}

TypePtr Sema::CheckCompareOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  auto LTy = LHS->getType();
  auto RTy = RHS->getType();

  if (LTy->getCategory() == Type::Category::Integer &&
      RTy->getCategory() == Type::Category::Integer)
    UsualArithmeticConversions(LHS, RHS, false);
  else if (LTy->getCategory() == Type::Category::Address &&
           RTy->getCategory() == Type::Category::Address) {
    LHS = UsualUnaryConversions(std::move(LHS));
    RHS = UsualUnaryConversions(std::move(RHS));
  } else
    // TODO: replace this, should impl. Compare Diagonostic
    assert(false && "cannot compare");

  return std::make_shared<BooleanType>();
}

TypePtr Sema::CheckLogicalOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Bool &&
         "LHS is not boolean.");
  assert(RHS->getType()->getCategory() == Type::Category::Bool &&
         "RHS is not boolean.");
  return std::make_shared<BooleanType>();
}

TypePtr Sema::CheckBitwiseOperands(ExprPtr &LHS, ExprPtr &RHS,
                                   BinaryOperatorKind Opc) {
  LHS = DefaultLvalueConversion(std::move(LHS));
  RHS = DefaultLvalueConversion(std::move(RHS));
  // TODO: replace assert with error msg
  assert(LHS->getType()->getCategory() == Type::Category::Integer &&
         "LHS is not int.");
  assert(RHS->getType()->getCategory() == Type::Category::Integer &&
         "RHS is not int.");
  return LHS->getType();
}

TypePtr Sema::CheckAssignmentOperands(ExprPtr &LHS, ExprPtr &RHS,
                                      TypePtr CompoundType) {
  RHS = UsualUnaryConversions(std::move(RHS));
  auto LTy = LHS->getType();
  TryImplicitCast(LHS->getType(), RHS);
  return LHS->getType();
}

TypePtr Sema::UsualArithmeticConversions(ExprPtr &LHS, ExprPtr &RHS,
                                         bool IsCompAssign) {
  if (!IsCompAssign) {
    LHS = UsualUnaryConversions(std::move(LHS));
  }
  RHS = UsualUnaryConversions(std::move(RHS));

  return handleIntegerConversion(*this, LHS, RHS, IsCompAssign);
}

ExprPtr Sema::UsualUnaryConversions(ExprPtr &&E) {
  return Sema::DefaultLvalueConversion(std::move(E));
}

ExprPtr Sema::DefaultLvalueConversion(ExprPtr &&E) {
  if (E->isRValue())
    return std::move(E);

  return std::make_unique<ImplicitCastExpr>(
      std::move(E), CastKind::LValueToRValue, E->getType());
}

DiagnosticBuilder Sema::Diag(SourceLocation Loc, unsigned DiagID) {
  return Diags.Report(Loc, DiagID);
}

} // namespace soll
