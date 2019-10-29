// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Frontend/DiagnosticRenderer.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

namespace soll {

class SourceManager;

class TextDiagnostic : public DiagnosticRenderer {
public:
  TextDiagnostic(llvm::raw_ostream &OS, DiagnosticOptions &DiagOpts)
      : DiagnosticRenderer(), OS(OS), DiagOpts(DiagOpts) {}
  virtual ~TextDiagnostic() override = default;

  static void printDiagnosticLevel(llvm::raw_ostream &OS,
                                   DiagnosticsEngine::Level Level,
                                   bool ShowColors);

  static void printDiagnosticMessage(llvm::raw_ostream &OS, bool IsSupplemental,
                                     llvm::StringRef Message,
                                     unsigned CurrentColumn, bool ShowColors);

protected:
  void emitDiagnosticMessage(FullSourceLoc Loc, PresumedLoc PLoc,
                             DiagnosticsEngine::Level Level,
                             llvm::StringRef Message,
                             llvm::ArrayRef<CharSourceRange> Ranges) override;

  void emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
                         DiagnosticsEngine::Level Level,
                         llvm::ArrayRef<CharSourceRange> Ranges) override;

private:
  void emitFilename(llvm::StringRef Filename, const SourceManager &SM);

  llvm::raw_ostream &OS;
  DiagnosticOptions &DiagOpts;
};

} // namespace soll
