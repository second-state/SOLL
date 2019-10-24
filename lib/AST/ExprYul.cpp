// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/ExprYul.h"
#include "soll/AST/Decl.h"

namespace soll {

YulIdentifier::YulIdentifier(const std::string &Name, SpecialIdentifier D,
                             TypePtr Ty)
    : Expr(ValueKind::VK_LValue, std::move(Ty)), Name(Name), D(D) {}

YulIdentifier::YulIdentifier(const std::string &Name, Decl *D)
    : Expr(ValueKind::VK_LValue, nullptr), Name(Name), D(D) {}

} // namespace soll
