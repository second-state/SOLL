// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/TargetOptions.h"
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>

namespace soll {
class DiagnosticsEngine;

void EmitBackendOutput(DiagnosticsEngine &Diags, const TargetOptions &TargetOpts,
                       const llvm::DataLayout &TDesc,
                       llvm::Module *M,
                       std::unique_ptr<llvm::raw_pwrite_stream> OS);

void EmbedBitcode(llvm::Module *M, llvm::MemoryBufferRef Buf);

} // namespace soll
