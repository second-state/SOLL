// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Expr.h"
#include "catch.hpp"

TEST_CASE("Expr", "[ast][stmt][expr]") {
  soll::BooleanLiteral literal(soll::Token(), true);
  REQUIRE(literal.isRValue());

  SECTION("getValue") { CHECK(literal.getValue()); }

  SECTION("setValue") {
    literal.setValue(false);
    CHECK_FALSE(literal.getValue());
  }
}
