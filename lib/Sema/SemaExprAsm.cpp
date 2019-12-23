// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Sema/Sema.h"

namespace soll {

std::unique_ptr<AsmIdentifier> Sema::CreateAsmIdentifier(llvm::StringRef Name) {
  static const llvm::StringMap<AsmIdentifier::SpecialIdentifier> SpecialLookup{
      /// Mark comment below logic identifiers because
      /// currently solidity only generate them for bitwise op.
      /// (meet solidity inline assembly usage)
      /// logical
      // {"not", AsmIdentifier::SpecialIdentifier::not_},
      // {"and", AsmIdentifier::SpecialIdentifier::and_},
      // {"or", AsmIdentifier::SpecialIdentifier::or_},
      // {"xor", AsmIdentifier::SpecialIdentifier::xor_},
      /// arithmetic
      {"addu256", AsmIdentifier::SpecialIdentifier::addu256},
      {"add", AsmIdentifier::SpecialIdentifier::addu256},
      {"subu256", AsmIdentifier::SpecialIdentifier::subu256},
      {"sub", AsmIdentifier::SpecialIdentifier::subu256},
      {"mulu256", AsmIdentifier::SpecialIdentifier::mulu256},
      {"mul", AsmIdentifier::SpecialIdentifier::mulu256},
      {"divu256", AsmIdentifier::SpecialIdentifier::divu256},
      {"div", AsmIdentifier::SpecialIdentifier::divu256},
      {"divs256", AsmIdentifier::SpecialIdentifier::divs256},
      {"modu256", AsmIdentifier::SpecialIdentifier::modu256},
      {"mod", AsmIdentifier::SpecialIdentifier::modu256},
      {"mods256", AsmIdentifier::SpecialIdentifier::mods256},
      {"signextendu256", AsmIdentifier::SpecialIdentifier::signextendu256},
      {"signextend", AsmIdentifier::SpecialIdentifier::signextendu256},
      {"expu256", AsmIdentifier::SpecialIdentifier::expu256},
      {"exp", AsmIdentifier::SpecialIdentifier::expu256},
      {"addmodu256", AsmIdentifier::SpecialIdentifier::addmodu256},
      {"addmod", AsmIdentifier::SpecialIdentifier::addmodu256},
      {"mulmodu256", AsmIdentifier::SpecialIdentifier::mulmodu256},
      {"mulmod", AsmIdentifier::SpecialIdentifier::mulmodu256},
      {"ltu256", AsmIdentifier::SpecialIdentifier::ltu256},
      {"lt", AsmIdentifier::SpecialIdentifier::ltu256},
      {"gtu256", AsmIdentifier::SpecialIdentifier::gtu256},
      {"gt", AsmIdentifier::SpecialIdentifier::gtu256},
      {"lts256", AsmIdentifier::SpecialIdentifier::lts256},
      {"gts256", AsmIdentifier::SpecialIdentifier::gts256},
      {"equ256", AsmIdentifier::SpecialIdentifier::equ256},
      {"eq", AsmIdentifier::SpecialIdentifier::equ256},
      {"iszerou256", AsmIdentifier::SpecialIdentifier::iszerou256},
      {"iszero", AsmIdentifier::SpecialIdentifier::iszerou256},
      {"notu256", AsmIdentifier::SpecialIdentifier::notu256},
      {"not", AsmIdentifier::SpecialIdentifier::notu256},
      {"andu256", AsmIdentifier::SpecialIdentifier::andu256},
      {"and", AsmIdentifier::SpecialIdentifier::andu256},
      {"oru256", AsmIdentifier::SpecialIdentifier::oru256},
      {"or", AsmIdentifier::SpecialIdentifier::oru256},
      {"xoru256", AsmIdentifier::SpecialIdentifier::xoru256},
      {"xor", AsmIdentifier::SpecialIdentifier::xoru256},
      {"shlu256", AsmIdentifier::SpecialIdentifier::shlu256},
      {"shl", AsmIdentifier::SpecialIdentifier::shlu256},
      {"shru256", AsmIdentifier::SpecialIdentifier::shru256},
      {"shr", AsmIdentifier::SpecialIdentifier::shru256},
      {"sars256", AsmIdentifier::SpecialIdentifier::sars256},
      // TODO: implement the rest identifiers
      /// memory and storage
      {"sload", AsmIdentifier::SpecialIdentifier::sload},
      {"sstore", AsmIdentifier::SpecialIdentifier::sstore},
      /// execution control
      {"gasleft", AsmIdentifier::SpecialIdentifier::gasleft},
      /// state
      /// object
      /// misc
      {"keccak256", AsmIdentifier::SpecialIdentifier::keccak256},
  };
  if (auto Iter = SpecialLookup.find(Name); Iter != SpecialLookup.end()) {
    TypePtr Ty;
    switch (Iter->second) {
    case AsmIdentifier::SpecialIdentifier::not_: ///< (bool) -> bool
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
    case AsmIdentifier::SpecialIdentifier::and_: ///< (bool, bool) -> bool
    case AsmIdentifier::SpecialIdentifier::or_:
    case AsmIdentifier::SpecialIdentifier::xor_:
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::addu256: ///< (u256, u256) -> u256
    case AsmIdentifier::SpecialIdentifier::subu256:
    case AsmIdentifier::SpecialIdentifier::mulu256:
    case AsmIdentifier::SpecialIdentifier::divu256:
    case AsmIdentifier::SpecialIdentifier::modu256:
    case AsmIdentifier::SpecialIdentifier::expu256:
    case AsmIdentifier::SpecialIdentifier::signextendu256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::divs256: ///< (s256, s256) -> s256
    case AsmIdentifier::SpecialIdentifier::mods256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::addmodu256: ///< (u256, u256, u256)
    case AsmIdentifier::SpecialIdentifier::mulmodu256: ///< -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::ltu256: ///< (u256, u256) -> bool
    case AsmIdentifier::SpecialIdentifier::gtu256:
    case AsmIdentifier::SpecialIdentifier::equ256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::lts256: ///< (s256, s256) -> bool
    case AsmIdentifier::SpecialIdentifier::gts256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::iszerou256: ///< (u256) -> bool
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::notu256: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::andu256: ///< (u256, u256) -> u256
    case AsmIdentifier::SpecialIdentifier::oru256:
    case AsmIdentifier::SpecialIdentifier::xoru256:
    case AsmIdentifier::SpecialIdentifier::shlu256:
    case AsmIdentifier::SpecialIdentifier::shru256:
    case AsmIdentifier::SpecialIdentifier::sars256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::sload: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
    case AsmIdentifier::SpecialIdentifier::sstore: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
    case AsmIdentifier::SpecialIdentifier::gasleft: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::keccak256: ///< (u256, u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    // TODO: implement the rest identifiers
    default:
      assert(false && "unknown special identifier");
      __builtin_unreachable();
    }
    return std::make_unique<AsmIdentifier>(Name, Iter->second, std::move(Ty));
  }
  Decl *D = lookupName(Name);
  assert(D != nullptr);
  return std::make_unique<AsmIdentifier>(Name, D);
}

std::unique_ptr<Expr> Sema::CreateAsmCallExpr(ExprPtr &&Callee,
                                              std::vector<ExprPtr> &&Args) {
  // TODO: handle invalid FunctionType.
  FunctionType *FTy = dynamic_cast<FunctionType *>(Callee->getType().get());
  // TODO: handle the case that number of return types != 1.
  TypePtr ReturnTy = FTy->getReturnTypes()[0];
  for (ExprPtr &Arg : Args) {
    Arg = DefaultLvalueConversion(std::move(Arg));
  }

  std::unique_ptr<Expr> CE = nullptr;
  if (auto I = dynamic_cast<AsmIdentifier *>(Callee.get())) {
    if (I->isSpecialIdentifier()) {
      if (CE = Sema::CreateAsmBuiltinCallExpr(*I, std::move(Args), ReturnTy)) {
        return CE;
      }
    } else {
      if (!dynamic_cast<const FunctionDecl *>(I->getCorrespondDecl())) {
        assert(false && "callee is not FunctionDecl");
        __builtin_unreachable();
      }
    }
  } else {
    assert(false && "callee is not AsmIdentifier");
    __builtin_unreachable();
  }

  return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                    std::move(ReturnTy));
}

std::unique_ptr<Expr>
Sema::CreateAsmBuiltinCallExpr(const AsmIdentifier &Callee,
                               std::vector<ExprPtr> &&Args, TypePtr ReturnTy) {
  switch (Callee.getSpecialIdentifier()) {
  case AsmIdentifier::SpecialIdentifier::not_:
    return std::make_unique<AsmUnaryOperator>(std::move(Args[0]), ReturnTy,
                                              UO_LNot);
  case AsmIdentifier::SpecialIdentifier::and_:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LAnd);
  case AsmIdentifier::SpecialIdentifier::or_:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LOr);
  case AsmIdentifier::SpecialIdentifier::xor_:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LXor);
  case AsmIdentifier::SpecialIdentifier::addu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Add);
  case AsmIdentifier::SpecialIdentifier::subu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Sub);
  case AsmIdentifier::SpecialIdentifier::mulu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Mul);
  case AsmIdentifier::SpecialIdentifier::divu256:
  case AsmIdentifier::SpecialIdentifier::divs256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Div);
  case AsmIdentifier::SpecialIdentifier::modu256:
  case AsmIdentifier::SpecialIdentifier::mods256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Rem);
  // TODO: case AsmIdentifier::SpecialIdentifier::signextendu256:
  case AsmIdentifier::SpecialIdentifier::expu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Exp);
  case AsmIdentifier::SpecialIdentifier::ltu256:
  case AsmIdentifier::SpecialIdentifier::lts256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LT);
  case AsmIdentifier::SpecialIdentifier::gtu256:
  case AsmIdentifier::SpecialIdentifier::gts256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_GT);
  case AsmIdentifier::SpecialIdentifier::equ256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_EQ);
  case AsmIdentifier::SpecialIdentifier::iszerou256:
    return std::make_unique<AsmIsZeroOperator>(std::move(Args[0]), ReturnTy);
  case AsmIdentifier::SpecialIdentifier::notu256:
    return std::make_unique<AsmUnaryOperator>(std::move(Args[0]), ReturnTy,
                                              UO_Not);
  case AsmIdentifier::SpecialIdentifier::andu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_And);
  case AsmIdentifier::SpecialIdentifier::oru256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Or);
  case AsmIdentifier::SpecialIdentifier::xoru256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Xor);
  case AsmIdentifier::SpecialIdentifier::shlu256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Shl);
  case AsmIdentifier::SpecialIdentifier::shru256:
    return std::make_unique<AsmBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Shr);
  // TODO: case AsmIdentifier::SpecialIdentifier::sars256:
  default: ///< treated as normal CallExpr
    break;
  }
  return nullptr;
}

} // namespace soll
