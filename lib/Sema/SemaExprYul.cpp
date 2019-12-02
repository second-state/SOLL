// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Sema/Sema.h"

namespace soll {

std::unique_ptr<YulIdentifier> Sema::CreateYulIdentifier(llvm::StringRef Name) {
  static const llvm::StringMap<YulIdentifier::SpecialIdentifier> SpecialLookup{
      /// logical
      {"not", YulIdentifier::SpecialIdentifier::not_},
      {"and", YulIdentifier::SpecialIdentifier::and_},
      {"or", YulIdentifier::SpecialIdentifier::or_},
      {"xor", YulIdentifier::SpecialIdentifier::xor_},
      /// arithmetic
      {"addu256", YulIdentifier::SpecialIdentifier::addu256},
      {"subu256", YulIdentifier::SpecialIdentifier::subu256},
      {"mulu256", YulIdentifier::SpecialIdentifier::mulu256},
      {"divu256", YulIdentifier::SpecialIdentifier::divu256},
      {"divs256", YulIdentifier::SpecialIdentifier::divs256},
      {"modu256", YulIdentifier::SpecialIdentifier::modu256},
      {"mods256", YulIdentifier::SpecialIdentifier::mods256},
      {"signextendu256", YulIdentifier::SpecialIdentifier::signextendu256},
      {"expu256", YulIdentifier::SpecialIdentifier::expu256},
      {"addmodu256", YulIdentifier::SpecialIdentifier::addmodu256},
      {"mulmodu256", YulIdentifier::SpecialIdentifier::mulmodu256},
      {"ltu256", YulIdentifier::SpecialIdentifier::ltu256},
      {"gtu256", YulIdentifier::SpecialIdentifier::gtu256},
      {"lts256", YulIdentifier::SpecialIdentifier::lts256},
      {"gts256", YulIdentifier::SpecialIdentifier::gts256},
      {"equ256", YulIdentifier::SpecialIdentifier::equ256},
      {"iszerou256", YulIdentifier::SpecialIdentifier::iszerou256},
      {"notu256", YulIdentifier::SpecialIdentifier::notu256},
      {"andu256", YulIdentifier::SpecialIdentifier::andu256},
      {"oru256", YulIdentifier::SpecialIdentifier::oru256},
      {"xoru256", YulIdentifier::SpecialIdentifier::xoru256},
      {"shlu256", YulIdentifier::SpecialIdentifier::shlu256},
      {"shru256", YulIdentifier::SpecialIdentifier::shru256},
      {"sars256", YulIdentifier::SpecialIdentifier::sars256},
      // TODO: implement the rest identifiers
      /// memory and storage
      /// execution control
      {"gasleft", YulIdentifier::SpecialIdentifier::gasleft},
      /// state
      /// object
      /// misc
      {"keccak256", YulIdentifier::SpecialIdentifier::keccak256},
  };
  if (auto Iter = SpecialLookup.find(Name); Iter != SpecialLookup.end()) {
    TypePtr Ty;
    switch (Iter->second) {
    case YulIdentifier::SpecialIdentifier::not_: ///< (bool) -> bool
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
    case YulIdentifier::SpecialIdentifier::and_: ///< (bool, bool) -> bool
    case YulIdentifier::SpecialIdentifier::or_:
    case YulIdentifier::SpecialIdentifier::xor_:
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::addu256: ///< (u256, u256) -> u256
    case YulIdentifier::SpecialIdentifier::subu256:
    case YulIdentifier::SpecialIdentifier::mulu256:
    case YulIdentifier::SpecialIdentifier::divu256:
    case YulIdentifier::SpecialIdentifier::modu256:
    case YulIdentifier::SpecialIdentifier::expu256:
    case YulIdentifier::SpecialIdentifier::signextendu256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::divs256: ///< (s256, s256) -> s256
    case YulIdentifier::SpecialIdentifier::mods256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::addmodu256: ///< (u256, u256, u256)
    case YulIdentifier::SpecialIdentifier::mulmodu256: ///< -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::ltu256: ///< (u256, u256) -> bool
    case YulIdentifier::SpecialIdentifier::gtu256:
    case YulIdentifier::SpecialIdentifier::equ256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case YulIdentifier::SpecialIdentifier::lts256: ///< (s256, s256) -> bool
    case YulIdentifier::SpecialIdentifier::gts256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case YulIdentifier::SpecialIdentifier::iszerou256: ///< (u256) -> bool
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case YulIdentifier::SpecialIdentifier::notu256: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::andu256: ///< (u256, u256) -> u256
    case YulIdentifier::SpecialIdentifier::oru256:
    case YulIdentifier::SpecialIdentifier::xoru256:
    case YulIdentifier::SpecialIdentifier::shlu256:
    case YulIdentifier::SpecialIdentifier::shru256:
    case YulIdentifier::SpecialIdentifier::sars256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case YulIdentifier::SpecialIdentifier::gasleft: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case YulIdentifier::SpecialIdentifier::keccak256: ///< (u256, u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    // TODO: implement the rest identifiers
    default:
      assert(false && "unknown special identifier");
      __builtin_unreachable();
    }
    return std::make_unique<YulIdentifier>(Name, Iter->second, std::move(Ty));
  }
  Decl *D = lookupName(Name);
  assert(D != nullptr);
  return std::make_unique<YulIdentifier>(Name, D);
}

std::unique_ptr<Expr> Sema::CreateYulCallExpr(ExprPtr &&Callee,
                                                  std::vector<ExprPtr> &&Args) {
  // TODO: handle invalid FunctionType.
  FunctionType *FTy = dynamic_cast<FunctionType *>(Callee->getType().get());
  // TODO: handle the case that number of return types != 1.
  TypePtr ReturnTy = FTy->getReturnTypes()[0];
  for (ExprPtr &Arg : Args) {
    Arg = DefaultLvalueConversion(std::move(Arg));
  }

  std::unique_ptr<Expr> CE = nullptr;
  if (auto I = dynamic_cast<YulIdentifier *>(Callee.get())) {
    if (I->isSpecialIdentifier()) {
      if (CE = Sema::CreateYulBuiltinCallExpr(*I, std::move(Args), ReturnTy)) {
        return CE;
      }
    } else {
      if (!dynamic_cast<const FunctionDecl *>(I->getCorrespondDecl())) {
        assert(false && "callee is not FunctionDecl");
        __builtin_unreachable();
      }
    }
  } else {
    assert(false && "callee is not YulIdentifier");
    __builtin_unreachable();
  }

  return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                    std::move(ReturnTy));
}

std::unique_ptr<Expr>
Sema::CreateYulBuiltinCallExpr(const YulIdentifier &Callee,
                               std::vector<ExprPtr> &&Args, TypePtr ReturnTy) {
  switch (Callee.getSpecialIdentifier()) {
  case YulIdentifier::SpecialIdentifier::not_:
    return std::make_unique<YulUnaryOperator>(std::move(Args[0]), ReturnTy,
                                              UO_LNot);
  case YulIdentifier::SpecialIdentifier::and_:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LAnd);
  case YulIdentifier::SpecialIdentifier::or_:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LOr);
  case YulIdentifier::SpecialIdentifier::xor_:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LXor);
  case YulIdentifier::SpecialIdentifier::addu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Add);
  case YulIdentifier::SpecialIdentifier::subu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Sub);
  case YulIdentifier::SpecialIdentifier::mulu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Mul);
  case YulIdentifier::SpecialIdentifier::divu256:
  case YulIdentifier::SpecialIdentifier::divs256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Div);
  case YulIdentifier::SpecialIdentifier::modu256:
  case YulIdentifier::SpecialIdentifier::mods256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Rem);
  // TODO: case YulIdentifier::SpecialIdentifier::signextendu256:
  case YulIdentifier::SpecialIdentifier::expu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Exp);
  case YulIdentifier::SpecialIdentifier::ltu256:
  case YulIdentifier::SpecialIdentifier::lts256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_LT);
  case YulIdentifier::SpecialIdentifier::gtu256:
  case YulIdentifier::SpecialIdentifier::gts256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_GT);
  case YulIdentifier::SpecialIdentifier::equ256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_EQ);
  case YulIdentifier::SpecialIdentifier::iszerou256:
    return std::make_unique<YulIsZeroOperator>(std::move(Args[0]), ReturnTy);
  case YulIdentifier::SpecialIdentifier::notu256:
    return std::make_unique<YulUnaryOperator>(std::move(Args[0]), ReturnTy,
                                              UO_Not);
  case YulIdentifier::SpecialIdentifier::andu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_And);
  case YulIdentifier::SpecialIdentifier::oru256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Or);
  case YulIdentifier::SpecialIdentifier::xoru256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Xor);
  case YulIdentifier::SpecialIdentifier::shlu256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Shl);
  case YulIdentifier::SpecialIdentifier::shru256:
    return std::make_unique<YulBinaryOperator>(
        std::move(Args[0]), std::move(Args[1]), ReturnTy, BO_Shr);
  // TODO: case YulIdentifier::SpecialIdentifier::sars256:
  default: ///< treated as normal CallExpr
    break;
  }
  return nullptr;
}

} // namespace soll
