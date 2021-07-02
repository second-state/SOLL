// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "CodeGenFunction.h"
#include "CodeGenModule.h"
#include <llvm/IR/CFG.h>
#include <llvm/IR/Value.h>

namespace soll::CodeGen {

class ExprEmitter {
public:
  CodeGenFunction &CGF;
  CodeGenModule &CGM;
  llvm::IRBuilder<llvm::ConstantFolder> &Builder;
  llvm::LLVMContext &VMContext;
  ExprEmitter(CodeGenFunction &CGF)
      : CGF(CGF), CGM(CGF.getCodeGenModule()), Builder(CGF.getBuilder()),
        VMContext(CGF.getLLVMContext()) {}
  ExprValuePtr visit(const Expr *E);

  ExprValuePtr structIndexAccess(const ExprValuePtr StructValue,
                                 unsigned ElementIndex, const StructType *STy);

  ExprValuePtr arrayIndexAccess(const ExprValuePtr &Base,
                                llvm::Value *IndexValue, const Type *Ty,
                                const ArrayType *ArrTy, bool isStateVariable);

private:
  ExprValuePtr visit(const UnaryOperator *UO);

  static bool isSigned(const Type *Ty);

  ExprValuePtr visit(const BinaryOperator *BO);

  ExprValuePtr visit(const CastExpr *CE);

  ExprValuePtr visit(const TupleExpr *TE);

  ExprValuePtr visit(const DirectValueExpr *DVE);

  ExprValuePtr visit(const ReturnTupleExpr *RTE);

  ExprValuePtr visit(const ParenExpr *PE);

  ExprValuePtr visit(const Identifier *ID);

  ExprValuePtr visit(const CallExpr *CE);

  void emitCheckArrayOutOfBound(llvm::Value *ArrSz, llvm::Value *Index);

  ExprValuePtr visit(const IndexAccess *IA);

  ExprValuePtr visit(const MemberExpr *ME);

  ExprValuePtr visit(const BooleanLiteral *BL);

  ExprValuePtr visit(const StringLiteral *SL);

  ExprValuePtr visit(const NumberLiteral *NL);

  ExprValuePtr visit(const AsmIdentifier *YI);

  const Identifier *resolveIdentifier(const Expr *E);
};

} // namespace soll::CodeGen