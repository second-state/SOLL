// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

namespace soll {

class ASTConsumer;
std::unique_ptr<ASTConsumer>
CreateASTPrinter(llvm::raw_ostream &Out = llvm::outs());

std::unique_ptr<ASTConsumer>
CreateFuncSigPrinter(llvm::raw_ostream &Out = llvm::outs());

std::unique_ptr<ASTConsumer>
CreateABIPrinter(llvm::raw_ostream &Out = llvm::outs());

} // namespace soll
