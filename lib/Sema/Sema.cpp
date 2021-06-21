// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Sema/Sema.h"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Lex/Lexer.h"
#include "soll/Sema/Scope.h"

namespace soll {

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()),
      LibrariesAddressMap(nullptr) {}

std::unique_ptr<FunctionDecl> Sema::CreateFunctionDecl(
    SourceRange L, llvm::StringRef Name, FunctionDecl::Visibility Vis,
    StateMutability SM, bool IsConstructor, bool IsFallback,
    std::unique_ptr<ParamList> &&Params,
    std::vector<std::unique_ptr<ModifierInvocation>> &&Modifiers,
    std::unique_ptr<ParamList> &&ReturnParams, std::unique_ptr<Block> &&Body,
    bool IsVirtual, std::unique_ptr<OverrideSpecifier> &&Overrides) {
  if (IsConstructor and !Params->getParams().empty()) {
    Diag(L.getBegin(), diag::err_unimplemented_constructor_parameter);
  }
  return std::make_unique<FunctionDecl>(
      L, Name, Vis, SM, IsConstructor, IsFallback, std::move(Params),
      std::move(Modifiers), std::move(ReturnParams), std::move(Body), IsVirtual,
      std::move(Overrides));
}

std::unique_ptr<EventDecl>
Sema::CreateEventDecl(SourceRange L, llvm::StringRef Name,
                      std::unique_ptr<ParamList> &&Params, bool Anonymous) {
  auto ED = std::make_unique<EventDecl>(L, Name, std::move(Params), Anonymous);
  return ED;
}

std::unique_ptr<ImplicitCastExpr>
Sema::CreateDummy(std::unique_ptr<Expr> &&Base) {
  return std::make_unique<ImplicitCastExpr>(std::move(Base));
}

ExprPtr Sema::CreateBinOp(SourceRange L, BinaryOperatorKind Opc, ExprPtr &&LHS,
                          ExprPtr &&RHS) {
  if (!LHS || !RHS) {
    return nullptr;
  }

  LHS = CreateDummy(std::move(LHS));
  RHS = CreateDummy(std::move(RHS));

  return std::make_unique<BinaryOperator>(L, std::move(LHS), std::move(RHS),
                                          Opc);
}

ExprPtr Sema::CreateIndexAccess(SourceLocation EndPos, ExprPtr &&LHS,
                                ExprPtr &&RHS) {
  const SourceRange L(LHS->getLocation().getBegin(), EndPos);

  return std::make_unique<IndexAccess>(L, std::move(LHS), std::move(RHS));
}

std::unique_ptr<CallExpr> Sema::CreateCallExpr(SourceRange L, ExprPtr &&Callee,
                                               std::vector<ExprPtr> &&Args) {
  for (ExprPtr &Arg : Args) {
    Arg = CreateDummy(std::move(Arg));
  }

  return std::make_unique<CallExpr>(L, std::move(Callee), std::move(Args));
}

std::unique_ptr<CallExpr>
Sema::CreateNamedCallExpr(SourceRange L, ExprPtr &&Callee,
                          std::vector<ExprPtr> &&Args,
                          std::vector<llvm::StringRef> &&Names) {
  for (ExprPtr &Arg : Args) {
    Arg = CreateDummy(std::move(Arg));
  }

  std::vector<std::string> NamesStr;
  for (auto Ref : Names)
    NamesStr.emplace_back(Ref.str());

  return std::make_unique<CallExpr>(L, std::move(Callee), std::move(Args),
                                    std::move(NamesStr));
}

std::unique_ptr<Identifier> Sema::CreateIdentifier(const Token &Tok) {
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
      {"this", Identifier::SpecialIdentifier::this_},
      {"super", Identifier::SpecialIdentifier::super_},
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
    case Identifier::SpecialIdentifier::this_:
    case Identifier::SpecialIdentifier::super_:
      Ty = std::make_shared<ContractType>();
      break;
    default:
      assert(false && "unknown special identifier");
      __builtin_unreachable();
    }
    return std::make_unique<Identifier>(Tok, Iter->second, Ty);
  }
  return std::make_unique<Identifier>(Tok);
}

void Sema::resolveIdentifiers() {
  for (const auto &I : CurrentScope()->resolveIdentifiers()) {
    std::visit(
        [this](const auto &I) {
          Diag(I->getToken().getLocation(), diag::err_undeclared_var_use)
              << I->getName();
        },
        I);
  }
}

std::unique_ptr<MemberExpr>
Sema::CreateMemberExpr(std::unique_ptr<Expr> &&BaseExpr, Token Tok) {
  const SourceRange L(BaseExpr->getLocation().getBegin(), Tok.getEndLoc());
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
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  const Expr *Base = BaseExpr.get();

  if (auto *I = dynamic_cast<const Identifier *>(Base)) {
    if (I->isSpecialIdentifier()) {
      if (I->getSpecialIdentifier() == Identifier::SpecialIdentifier::this_) {
        return std::make_unique<MemberExpr>(L, std::move(BaseExpr),
                                            std::make_unique<Identifier>(Tok));
      }
      if (I->getSpecialIdentifier() == Identifier::SpecialIdentifier::super_) {
        return std::make_unique<MemberExpr>(L, std::move(BaseExpr),
                                            std::make_unique<Identifier>(Tok));
      }
      if (auto Iter = Lookup.find(Name); Iter != Lookup.end()) {
        std::shared_ptr<Type> Ty;
        switch (I->getSpecialIdentifier()) {
        case Identifier::SpecialIdentifier::abi:
          switch (Iter->second) {
          case Identifier::SpecialIdentifier::abi_encodePacked:
          case Identifier::SpecialIdentifier::abi_encode:
          case Identifier::SpecialIdentifier::abi_encodeWithSelector:
          case Identifier::SpecialIdentifier::abi_encodeWithSignature:
            Ty = std::make_shared<BytesType>();
            break;
          case Identifier::SpecialIdentifier::abi_decode:
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
            L, std::move(BaseExpr),
            std::make_unique<Identifier>(Tok, Iter->second, Ty));
      }
    }
  }
  // unresolvable now
  return std::make_unique<MemberExpr>(
      L, std::move(BaseExpr), std::make_unique<Identifier>(Tok));
}

DiagnosticBuilder Sema::Diag(SourceLocation Loc, unsigned DiagID) {
  return Diags.Report(Loc, DiagID);
}

} // namespace soll
