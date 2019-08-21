// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/Sema/BreakableVisitor.h"

namespace soll {

class Lexer;
class ASTContext;
class ASTConsumer;
class DiagnosticsEngine;
class SourceManager;

class Sema {
  Sema(const Sema &) = delete;
  Sema &operator=(const Sema &) = delete;

public:
  Lexer &Lex;
  ASTContext &Context;
  ASTConsumer &Consumer;
  DiagnosticsEngine &Diags;
  SourceManager &SourceMgr;

  Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer);

  void resolveBreak(FunctionDecl &);
};

} // namespace soll
