// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Frontend/TextDiagnostic.h"
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/SourceManager.h"
#include "soll/Lex/Lexer.h"

namespace {
static const constexpr enum llvm::raw_ostream::Colors noteColor =
    llvm::raw_ostream::WHITE;
static const constexpr enum llvm::raw_ostream::Colors remarkColor =
    llvm::raw_ostream::BLUE;
// -Wunused-const-variable
// static const constexpr enum llvm::raw_ostream::Colors fixitColor =
//     llvm::raw_ostream::GREEN;
// static const constexpr enum llvm::raw_ostream::Colors caretColor =
//     llvm::raw_ostream::GREEN;
static const constexpr enum llvm::raw_ostream::Colors warningColor =
    llvm::raw_ostream::MAGENTA;
static const constexpr enum llvm::raw_ostream::Colors templateColor =
    llvm::raw_ostream::CYAN;
static const constexpr enum llvm::raw_ostream::Colors errorColor =
    llvm::raw_ostream::RED;
static const constexpr enum llvm::raw_ostream::Colors fatalColor =
    llvm::raw_ostream::RED;
static const constexpr enum llvm::raw_ostream::Colors savedColor =
    llvm::raw_ostream::SAVEDCOLOR;

static void applyTemplateHighlighting(llvm::raw_ostream &OS,
                                      llvm::StringRef Str, bool &Normal,
                                      bool Bold) {
  while (true) {
    size_t Pos = Str.find(soll::ToggleHighlight);
    OS << Str.slice(0, Pos);
    if (Pos == llvm::StringRef::npos)
      break;

    Str = Str.substr(Pos + 1);
    if (Normal)
      OS.changeColor(templateColor, true);
    else {
      OS.resetColor();
      if (Bold)
        OS.changeColor(savedColor, true);
    }
    Normal = !Normal;
  }
}

} // namespace

namespace soll {

/*static*/ void TextDiagnostic::printDiagnosticLevel(
    llvm::raw_ostream &OS, DiagnosticsEngine::Level Level, bool ShowColors) {
  if (ShowColors) {
    switch (Level) {
    case DiagnosticsEngine::Level::Ignored:
      llvm_unreachable("Invalid diagnostic type");
    case DiagnosticsEngine::Level::Note:
      OS.changeColor(noteColor, true);
      break;
    case DiagnosticsEngine::Level::Remark:
      OS.changeColor(remarkColor, true);
      break;
    case DiagnosticsEngine::Level::Warning:
      OS.changeColor(warningColor, true);
      break;
    case DiagnosticsEngine::Level::Error:
      OS.changeColor(errorColor, true);
      break;
    case DiagnosticsEngine::Level::Fatal:
      OS.changeColor(fatalColor, true);
      break;
    }
  }

  switch (Level) {
  case DiagnosticsEngine::Level::Ignored:
    llvm_unreachable("Invalid diagnostic type");
  case DiagnosticsEngine::Level::Note:
    OS << "note";
    break;
  case DiagnosticsEngine::Level::Remark:
    OS << "remark";
    break;
  case DiagnosticsEngine::Level::Warning:
    OS << "warning";
    break;
  case DiagnosticsEngine::Level::Error:
    OS << "error";
    break;
  case DiagnosticsEngine::Level::Fatal:
    OS << "fatal error";
    break;
  }

  OS << ": ";

  if (ShowColors)
    OS.resetColor();
}

/*static*/
void TextDiagnostic::printDiagnosticMessage(llvm::raw_ostream &OS,
                                            bool IsSupplemental,
                                            llvm::StringRef Message,
                                            unsigned CurrentColumn,
                                            bool ShowColors) {
  bool Bold = false;
  if (ShowColors && !IsSupplemental) {
    OS.changeColor(savedColor, true);
    Bold = true;
  }

  bool Normal = true;
  applyTemplateHighlighting(OS, Message, Normal, Bold);
  assert(Normal && "Formatting should have returned to normal");

  if (ShowColors)
    OS.resetColor();
  OS << '\n';
}

void TextDiagnostic::emitDiagnosticMessage(
    FullSourceLoc Loc, PresumedLoc PLoc, DiagnosticsEngine::Level Level,
    llvm::StringRef Message, llvm::ArrayRef<CharSourceRange> Ranges) {
  uint64_t StartOfLocationInfo = OS.tell();

  if (Loc.isValid())
    emitDiagnosticLoc(Loc, PLoc, Level, Ranges);

  if (DiagOpts.ShowColors)
    OS.resetColor();

  printDiagnosticLevel(OS, Level, DiagOpts.ShowColors);

  printDiagnosticMessage(
      OS,
      /*IsSupplemental*/ Level == DiagnosticsEngine::Level::Note, Message,
      OS.tell() - StartOfLocationInfo, DiagOpts.ShowColors);
}

void TextDiagnostic::emitDiagnosticLoc(FullSourceLoc Loc, PresumedLoc PLoc,
                                       DiagnosticsEngine::Level Level,
                                       llvm::ArrayRef<CharSourceRange> Ranges) {
  if (PLoc.isInvalid()) {
    // At least print the file name if available:
    FileID FID = Loc.getFileID();
    if (FID.isValid()) {
      const FileEntry *FE = Loc.getFileEntry();
      if (FE && FE->isValid()) {
        emitFilename(FE->getName(), Loc.getManager());
        OS << ": ";
      }
    }
    return;
  }
  unsigned LineNo = PLoc.getLine();

  if (DiagOpts.ShowColors)
    OS.changeColor(savedColor, true);

  emitFilename(PLoc.getFilename(), Loc.getManager());

  OS << ':' << LineNo;
  if (unsigned ColNo = PLoc.getColumn()) {
    OS << ':' << ColNo;
  }

  OS << ':';

  if (!Ranges.empty()) {
    FileID CaretFileID = Loc.getFileID();
    bool PrintedRange = false;

    for (llvm::ArrayRef<CharSourceRange>::const_iterator RI = Ranges.begin(),
                                                         RE = Ranges.end();
         RI != RE; ++RI) {
      if (!RI->isValid())
        continue;

      auto &SM = Loc.getManager();
      SourceLocation B = RI->getBegin();
      CharSourceRange ERange = SM.getExpansionRange(RI->getEnd());
      SourceLocation E = ERange.getEnd();
      bool IsTokenRange = ERange.isTokenRange();

      std::pair<FileID, unsigned> BInfo = SM.getDecomposedLoc(B);
      std::pair<FileID, unsigned> EInfo = SM.getDecomposedLoc(E);

      if (BInfo.first != CaretFileID || EInfo.first != CaretFileID)
        continue;

      unsigned TokSize = 0;
      if (IsTokenRange)
        TokSize = Lexer::MeasureTokenLength(E, SM);

      FullSourceLoc BF(B, SM), EF(E, SM);
      OS << '{' << BF.getLineNumber().getValueOr(0) << ':'
         << BF.getColumnNumber().getValueOr(0) << '-'
         << EF.getLineNumber().getValueOr(0) << ':'
         << (EF.getColumnNumber().getValueOr(0) + TokSize) << '}';
      PrintedRange = true;
    }

    if (PrintedRange)
      OS << ':';
  }
  OS << ' ';
}

void TextDiagnostic::emitFilename(llvm::StringRef Filename,
                                  const SourceManager &SM) {
  OS << Filename;
}

} // namespace soll
