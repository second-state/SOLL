// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/TargetOptions.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>

namespace soll {
class DiagnosticsEngine;

enum class BackendAction {
  EmitAssembly, ///< Emit native assembly files
  EmitBC,       ///< Emit LLVM bitcode files
  EmitLL,       ///< Emit human-readable LLVM assembly
  EmitNothing,  ///< Don't emit anything (benchmarking mode)
  EmitMCNull,   ///< Run CodeGen, but don't emit anything
  EmitObj       ///< Emit native object files
};

void EmitBackendOutput(DiagnosticsEngine &Diags,
                       const TargetOptions &TargetOpts,
                       const llvm::DataLayout &TDesc, llvm::Module *M,
                       BackendAction Action,
                       std::unique_ptr<llvm::raw_pwrite_stream> OS);

} // namespace soll
