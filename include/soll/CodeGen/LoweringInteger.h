// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

namespace soll {

class LoweringInteger : public llvm::PassInfoMixin<LoweringInteger> {
  llvm::Module *TheModule = nullptr;
  llvm::SmallVector<llvm::Instruction *, 256> Worklist;

  void checkInstruction(llvm::Instruction *I);

  llvm::DenseMap<unsigned int, llvm::Function *> MulFunction;
  llvm::Function *GetMulFunction(const unsigned int BitWidth);

  llvm::DenseMap<unsigned int, llvm::Function *> UDivFunction;
  llvm::Function *GetUDivFunction(const unsigned int BitWidth);

  llvm::DenseMap<unsigned int, llvm::Function *> URemFunction;
  llvm::Function *GetURemFunction(const unsigned int BitWidth);

  llvm::DenseMap<unsigned int, llvm::Function *> ShlFunction;
  llvm::Function *GetShlFunction(const unsigned int BitWidth);

  llvm::DenseMap<unsigned int, llvm::Function *> LShrFunction;
  llvm::Function *GetLShrFunction(const unsigned int BitWidth);

  llvm::DenseMap<unsigned int, llvm::Function *> AShrFunction;
  llvm::Function *GetAShrFunction(const unsigned int BitWidth);

public:
  llvm::PreservedAnalyses run(llvm::Module &M,
                              llvm::ModuleAnalysisManager &MAM);
  void checkWorllist();
};
} // namespace soll
