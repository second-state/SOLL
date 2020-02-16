// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/TextDiagnosticPrinter.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Frontend/TextDiagnostic.h"
#include <llvm/ADT/SmallString.h>

namespace soll {

TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &os,
                                             DiagnosticOptions *diags,
                                             bool _OwnsOutputStream)
    : OS(os), DiagOpts(diags), OwnsOutputStream(_OwnsOutputStream) {}

TextDiagnosticPrinter::~TextDiagnosticPrinter() {
  if (OwnsOutputStream)
    delete &OS;
}

void TextDiagnosticPrinter::BeginSourceFile() {
  TextDiag.reset(new TextDiagnostic(OS, *DiagOpts));
}

void TextDiagnosticPrinter::EndSourceFile() { TextDiag.reset(); }

void TextDiagnosticPrinter::HandleDiagnostic(DiagnosticsEngine::Level Level,
                                             const Diagnostic &Info) {
  DiagnosticConsumer::HandleDiagnostic(Level, Info);

  llvm::SmallString<100> OutStr;
  Info.FormatDiagnostic(OutStr);

  llvm::raw_svector_ostream DiagMessageStream(OutStr);

  uint64_t StartOfLocationInfo = OS.tell();

  if (!Info.getLocation().isValid()) {
    TextDiagnostic::printDiagnosticLevel(OS, Level, DiagOpts->ShowColors);
    TextDiagnostic::printDiagnosticMessage(
        OS, Level != DiagnosticsEngine::Level::Ignored, DiagMessageStream.str(),
        OS.tell() - StartOfLocationInfo, DiagOpts->ShowColors);
    OS.flush();
    return;
  }

  // Assert that the rest of our infrastructure is setup properly.
  assert(DiagOpts && "Unexpected diagnostic without options set");
  assert(Info.hasSourceManager() &&
         "Unexpected diagnostic with no source manager");
  assert(TextDiag && "Unexpected diagnostic outside source file processing");

  TextDiag->emitDiagnostic(
      FullSourceLoc(Info.getLocation(), Info.getSourceManager()), Level,
      DiagMessageStream.str(), Info.getRanges(), Info.getFixItHints());

  OS.flush();
}

} // namespace soll
