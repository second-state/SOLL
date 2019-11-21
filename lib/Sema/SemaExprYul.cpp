// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Sema/Sema.h"

namespace soll {

std::unique_ptr<YulIdentifier> Sema::CreateYulIdentifier(llvm::StringRef Name) {
  static const llvm::StringMap<YulIdentifier::SpecialIdentifier> SpecialLookup{
      {"not", YulIdentifier::SpecialIdentifier::not_},
      {"and", YulIdentifier::SpecialIdentifier::and_},
      {"or", YulIdentifier::SpecialIdentifier::or_},
      {"xor", YulIdentifier::SpecialIdentifier::xor_},
      // TODO: implement the rest identifiers
  };
  if (auto Iter = SpecialLookup.find(Name); Iter != SpecialLookup.end()) {
    TypePtr Ty;
    switch (Iter->second) {
    case YulIdentifier::SpecialIdentifier::not_:
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
    case YulIdentifier::SpecialIdentifier::and_:
    case YulIdentifier::SpecialIdentifier::or_:
    case YulIdentifier::SpecialIdentifier::xor_:
      Ty = std::make_shared<BooleanType>();
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

std::unique_ptr<CallExpr> Sema::CreateYulCallExpr(ExprPtr &&Callee,
                                                  std::vector<ExprPtr> &&Args) {
  FunctionType *FTy = nullptr;
  if (auto I = dynamic_cast<YulIdentifier *>(Callee.get())) {
    if (I->isSpecialIdentifier()) {
      FTy = dynamic_cast<FunctionType *>(I->getType().get());
    } else {
      if (auto D = dynamic_cast<const FunctionDecl *>(I->getCorrespondDecl())) {
        FTy = dynamic_cast<FunctionType *>(D->getType().get());
      } else {
        assert(false && "callee is not FunctionDecl");
        __builtin_unreachable();
      }
    }
  } else {
    assert(false && "callee is not YulIdentifier");
    __builtin_unreachable();
  }

  TypePtr ReturnTy = Callee->getType();
  for (ExprPtr &Arg : Args) {
    Arg = DefaultLvalueConversion(std::move(Arg));
  }

  // TODO: create subclass like YulLogicalCallExpr
  return std::make_unique<CallExpr>(std::move(Callee), std::move(Args),
                                    std::move(ReturnTy));
}

} // namespace soll
