// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

class ASTNode {
public:
  enum class ASTNodeType { DECL, STMT };
  ASTNode() = default;
  virtual ~ASTNode() = default;
  virtual ASTNodeType getASTType() = 0;
};

} // namespace soll
