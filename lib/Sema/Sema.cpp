#include "soll/Sema/Sema.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/AST/ASTContext.h"
#include "soll/Lex/Lexer.h"

namespace soll {

Sema::Sema(Lexer &lexer, ASTContext &ctxt, ASTConsumer &consumer)
    : Lex(lexer), Context(ctxt), Consumer(consumer),
      Diags(Lex.getDiagnostics()), SourceMgr(Lex.getSourceManager()) {}

} // namespace soll
