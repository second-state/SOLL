// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/Diagnostic.h"
#include "soll/Basic/CharInfo.h"
#include <cstring>
#include <limits>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/Locale.h>
#include <utility>

namespace {

template <std::size_t StrLen>
static bool ModifierIs(const char *Modifier, unsigned ModifierLen,
                       const char (&Str)[StrLen]) {
  return StrLen - 1 == ModifierLen &&
         std::memcmp(Modifier, Str, StrLen - 1) == 0;
}

static const char *ScanFormat(const char *I, const char *E, char Target) {
  unsigned Depth = 0;

  for (; I != E; ++I) {
    if (Depth == 0 && *I == Target)
      return I;
    if (Depth != 0 && *I == '}')
      Depth--;

    if (*I == '%') {
      I++;
      if (I == E)
        break;

      if (!soll::isDigit(*I) && !soll::isPunctuation(*I)) {
        for (I++; I != E && !soll::isDigit(*I) && *I != '{'; I++)
          ;
        if (I == E)
          break;
        if (*I == '{')
          Depth++;
      }
    }
  }
  return E;
}

static void HandleSelectModifier(const soll::Diagnostic &DInfo, unsigned ValNo,
                                 const char *Argument, unsigned ArgumentLen,
                                 llvm::SmallVectorImpl<char> &OutStr) {
  const char *ArgumentEnd = Argument + ArgumentLen;

  while (ValNo) {
    const char *NextVal = ScanFormat(Argument, ArgumentEnd, '|');
    assert(NextVal != ArgumentEnd &&
           "Value for integer select modifier was"
           " larger than the number of options in the diagnostic string!");
    Argument = NextVal + 1; // Skip this string.
    --ValNo;
  }

  const char *EndPtr = ScanFormat(Argument, ArgumentEnd, '|');

  DInfo.FormatDiagnostic(Argument, EndPtr, OutStr);
}

static void HandleIntegerSModifier(unsigned ValNo,
                                   llvm::SmallVectorImpl<char> &OutStr) {
  if (ValNo != 1)
    OutStr.push_back('s');
}

static void HandleOrdinalModifier(unsigned ValNo,
                                  llvm::SmallVectorImpl<char> &OutStr) {
  assert(ValNo != 0 && "ValNo must be strictly positive!");

  llvm::raw_svector_ostream Out(OutStr);

  Out << ValNo << llvm::getOrdinalSuffix(ValNo);
}

static unsigned PluralNumber(const char *&Start, const char *End) {
  // Programming 101: Parse a decimal number :-)
  unsigned Val = 0;
  while (Start != End && *Start >= '0' && *Start <= '9') {
    Val *= 10;
    Val += *Start - '0';
    ++Start;
  }
  return Val;
}

static bool TestPluralRange(unsigned Val, const char *&Start, const char *End) {
  if (*Start != '[') {
    unsigned Ref = PluralNumber(Start, End);
    return Ref == Val;
  }

  ++Start;
  unsigned Low = PluralNumber(Start, End);
  assert(*Start == ',' && "Bad plural expression syntax: expected ,");
  ++Start;
  unsigned High = PluralNumber(Start, End);
  assert(*Start == ']' && "Bad plural expression syntax: expected )");
  ++Start;
  return Low <= Val && Val <= High;
}

static bool EvalPluralExpr(unsigned ValNo, const char *Start, const char *End) {
  if (*Start == ':')
    return true;

  while (true) {
    char C = *Start;
    if (C == '%') {
      ++Start;
      unsigned Arg = PluralNumber(Start, End);
      assert(*Start == '=' && "Bad plural expression syntax: expected =");
      ++Start;
      unsigned ValMod = ValNo % Arg;
      if (TestPluralRange(ValMod, Start, End))
        return true;
    } else {
      assert((C == '[' || (C >= '0' && C <= '9')) &&
             "Bad plural expression syntax: unexpected character");
      if (TestPluralRange(ValNo, Start, End))
        return true;
    }

    Start = std::find(Start, End, ',');
    if (Start == End)
      break;
    ++Start;
  }
  return false;
}

static void HandlePluralModifier(const soll::Diagnostic &DInfo, unsigned ValNo,
                                 const char *Argument, unsigned ArgumentLen,
                                 llvm::SmallVectorImpl<char> &OutStr) {
  const char *ArgumentEnd = Argument + ArgumentLen;
  while (true) {
    assert(Argument < ArgumentEnd && "Plural expression didn't match.");
    const char *ExprEnd = Argument;
    while (*ExprEnd != ':') {
      assert(ExprEnd != ArgumentEnd && "Plural missing expression end");
      ++ExprEnd;
    }
    if (EvalPluralExpr(ValNo, Argument, ExprEnd)) {
      Argument = ExprEnd + 1;
      ExprEnd = ScanFormat(Argument, ArgumentEnd, '|');

      DInfo.FormatDiagnostic(Argument, ExprEnd, OutStr);
      return;
    }
    Argument = ScanFormat(Argument, ArgumentEnd - 1, '|') + 1;
  }
}

static const char *getTokenDescForDiagnostic(soll::tok::TokenKind Kind) {
  switch (Kind) {
  case soll::tok::identifier:
    return "identifier";
  default:
    return nullptr;
  }
}

} // namespace

namespace soll {

DiagnosticsEngine::DiagnosticsEngine(
    llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags,
    llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts,
    DiagnosticConsumer *client, bool ShouldOwnClient)
    : Diags(Diags), DiagOpts(DiagOpts), Client(client),
      Owner(ShouldOwnClient ? client : nullptr) {}

bool DiagnosticsEngine::EmitCurrentDiagnostic(bool Force) {
  assert(getClient() && "DiagnosticClient not set!");

  bool Emitted;
  if (Force) {
    Diagnostic Info(this);

    DiagnosticIDs::Level DiagLevel =
        Diags->getDiagnosticLevel(Info.getID(), Info.getLocation(), *this);

    Emitted = (DiagLevel != DiagnosticIDs::Ignored);
    if (Emitted) {
      Diags->EmitDiag(*this, DiagLevel);
    }
  } else {
    Emitted = Diags->ProcessDiag(*this);
  }

  Clear();

  if (!Force && DelayedDiagID)
    ReportDelayed();

  return Emitted;
}

void DiagnosticsEngine::setClient(DiagnosticConsumer *client,
                                  bool ShouldOwnClient) {
  Owner.reset(ShouldOwnClient ? client : nullptr);
  Client = client;
}

void DiagnosticsEngine::Report(const StoredDiagnostic &storedDiag) {
  assert(CurDiagID == std::numeric_limits<unsigned>::max() &&
         "Multiple diagnostics in flight at once!");

  CurDiagLoc = storedDiag.getLocation();
  CurDiagID = storedDiag.getID();
  NumDiagArgs = 0;

  DiagRanges.clear();
  DiagRanges.append(storedDiag.range_begin(), storedDiag.range_end());

  DiagFixItHints.clear();
  DiagFixItHints.append(storedDiag.fixit_begin(), storedDiag.fixit_end());

  assert(Client && "DiagnosticConsumer not set!");
  Level DiagLevel = storedDiag.getLevel();
  Diagnostic Info(this, storedDiag.getMessage());
  Client->HandleDiagnostic(DiagLevel, Info);
  if (Client->IncludeInDiagnosticCounts()) {
    if (DiagLevel == DiagnosticsEngine::Level::Warning)
      ++NumWarnings;
  }

  CurDiagID = std::numeric_limits<unsigned>::max();
}

void DiagnosticsEngine::ReportDelayed() {
  unsigned ID = DelayedDiagID;
  DelayedDiagID = 0;
  Report(ID) << DelayedDiagArg1 << DelayedDiagArg2;
}

void Diagnostic::FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const {
  if (!StoredDiagMessage.empty()) {
    OutStr.append(StoredDiagMessage.begin(), StoredDiagMessage.end());
    return;
  }

  llvm::StringRef Diag =
      getDiags()->getDiagnosticIDs()->getDescription(getID());

  FormatDiagnostic(Diag.begin(), Diag.end(), OutStr);
}

void Diagnostic::FormatDiagnostic(const char *DiagStr, const char *DiagEnd,
                                  llvm::SmallVectorImpl<char> &OutStr) const {
  if (DiagEnd - DiagStr == 2 &&
      llvm::StringRef(DiagStr, DiagEnd - DiagStr).equals("%0") &&
      getArgKind(0) == DiagnosticsEngine::ArgumentKind::ak_std_string) {
    const std::string &S = getArgStdStr(0);
    for (char c : S) {
      if (llvm::sys::locale::isPrint(c) || c == '\t') {
        OutStr.push_back(c);
      }
    }
    return;
  }

  llvm::SmallVector<DiagnosticsEngine::ArgumentValue, 8> FormattedArgs;

  llvm::SmallVector<intptr_t, 2> QualTypeVals;
  llvm::SmallVector<char, 64> Tree;

  for (unsigned I = 0, e = getNumArgs(); I < e; ++I)
    if (getArgKind(I) == DiagnosticsEngine::ArgumentKind::ak_qualtype)
      QualTypeVals.push_back(getRawArg(I));

  while (DiagStr != DiagEnd) {
    if (DiagStr[0] != '%') {
      const char *StrEnd = std::find(DiagStr, DiagEnd, '%');
      OutStr.append(DiagStr, StrEnd);
      DiagStr = StrEnd;
      continue;
    } else if (isPunctuation(DiagStr[1])) {
      OutStr.push_back(DiagStr[1]); // %% -> %.
      DiagStr += 2;
      continue;
    }

    ++DiagStr;

    const char *Modifier = nullptr, *Argument = nullptr;
    unsigned ModifierLen = 0, ArgumentLen = 0;

    if (!isDigit(DiagStr[0])) {
      Modifier = DiagStr;
      while (DiagStr[0] == '-' || (DiagStr[0] >= 'a' && DiagStr[0] <= 'z'))
        ++DiagStr;
      ModifierLen = DiagStr - Modifier;

      if (DiagStr[0] == '{') {
        ++DiagStr; // Skip {.
        Argument = DiagStr;

        DiagStr = ScanFormat(DiagStr, DiagEnd, '}');
        assert(DiagStr != DiagEnd && "Mismatched {}'s in diagnostic string!");
        ArgumentLen = DiagStr - Argument;
        ++DiagStr; // Skip }.
      }
    }

    assert(isDigit(*DiagStr) && "Invalid format for argument in diagnostic");
    unsigned ArgNo = *DiagStr++ - '0';

    // Only used for type diffing.
    unsigned ArgNo2 = ArgNo;

    DiagnosticsEngine::ArgumentKind Kind = getArgKind(ArgNo);
    if (ModifierIs(Modifier, ModifierLen, "diff")) {
      assert(*DiagStr == ',' && isDigit(*(DiagStr + 1)) &&
             "Invalid format for diff modifier");
      ++DiagStr; // Comma.
      ArgNo2 = *DiagStr++ - '0';
      DiagnosticsEngine::ArgumentKind Kind2 = getArgKind(ArgNo2);
      if (Kind == DiagnosticsEngine::ArgumentKind::ak_qualtype &&
          Kind2 == DiagnosticsEngine::ArgumentKind::ak_qualtype)
        Kind = DiagnosticsEngine::ArgumentKind::ak_qualtype_pair;
      else {
        // %diff only supports QualTypes.  For other kinds of arguments,
        // use the default printing.  For example, if the modifier is:
        //   "%diff{compare $ to $|other text}1,2"
        // treat it as:
        //   "compare %1 to %2"
        const char *ArgumentEnd = Argument + ArgumentLen;
        const char *Pipe = ScanFormat(Argument, ArgumentEnd, '|');
        assert(ScanFormat(Pipe + 1, ArgumentEnd, '|') == ArgumentEnd &&
               "Found too many '|'s in a %diff modifier!");
        const char *FirstDollar = ScanFormat(Argument, Pipe, '$');
        const char *SecondDollar = ScanFormat(FirstDollar + 1, Pipe, '$');
        const char ArgStr1[] = {'%', static_cast<char>('0' + ArgNo)};
        const char ArgStr2[] = {'%', static_cast<char>('0' + ArgNo2)};
        FormatDiagnostic(Argument, FirstDollar, OutStr);
        FormatDiagnostic(ArgStr1, ArgStr1 + 2, OutStr);
        FormatDiagnostic(FirstDollar + 1, SecondDollar, OutStr);
        FormatDiagnostic(ArgStr2, ArgStr2 + 2, OutStr);
        FormatDiagnostic(SecondDollar + 1, Pipe, OutStr);
        continue;
      }
    }

    switch (Kind) {
    // ---- STRINGS ----
    case DiagnosticsEngine::ArgumentKind::ak_std_string: {
      const std::string &S = getArgStdStr(ArgNo);
      assert(ModifierLen == 0 && "No modifiers for strings yet");
      OutStr.append(S.begin(), S.end());
      break;
    }
    case DiagnosticsEngine::ArgumentKind::ak_c_string: {
      const char *S = getArgCStr(ArgNo);
      assert(ModifierLen == 0 && "No modifiers for strings yet");

      // Don't crash if get passed a null pointer by accident.
      if (!S)
        S = "(null)";

      OutStr.append(S, S + strlen(S));
      break;
    }
    // ---- INTEGERS ----
    case DiagnosticsEngine::ArgumentKind::ak_sint: {
      int Val = getArgSInt(ArgNo);

      if (ModifierIs(Modifier, ModifierLen, "select")) {
        HandleSelectModifier(*this, (unsigned)Val, Argument, ArgumentLen,
                             OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "s")) {
        HandleIntegerSModifier(Val, OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "plural")) {
        HandlePluralModifier(*this, (unsigned)Val, Argument, ArgumentLen,
                             OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "ordinal")) {
        HandleOrdinalModifier((unsigned)Val, OutStr);
      } else {
        assert(ModifierLen == 0 && "Unknown integer modifier");
        llvm::raw_svector_ostream(OutStr) << Val;
      }
      break;
    }
    case DiagnosticsEngine::ArgumentKind::ak_uint: {
      unsigned Val = getArgUInt(ArgNo);

      if (ModifierIs(Modifier, ModifierLen, "select")) {
        HandleSelectModifier(*this, Val, Argument, ArgumentLen, OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "s")) {
        HandleIntegerSModifier(Val, OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "plural")) {
        HandlePluralModifier(*this, (unsigned)Val, Argument, ArgumentLen,
                             OutStr);
      } else if (ModifierIs(Modifier, ModifierLen, "ordinal")) {
        HandleOrdinalModifier(Val, OutStr);
      } else {
        assert(ModifierLen == 0 && "Unknown integer modifier");
        llvm::raw_svector_ostream(OutStr) << Val;
      }
      break;
    }
    // ---- TOKEN SPELLINGS ----
    case DiagnosticsEngine::ArgumentKind::ak_tokenkind: {
      tok::TokenKind Kind = static_cast<tok::TokenKind>(getRawArg(ArgNo));
      assert(ModifierLen == 0 && "No modifiers for token kinds yet");

      llvm::raw_svector_ostream Out(OutStr);
      if (const char *S = tok::getPunctuatorSpelling(Kind))
        // Quoted token spelling for punctuators.
        Out << '\'' << S << '\'';
      else if (const char *S = tok::getKeywordSpelling(Kind))
        // Unquoted token spelling for keywords.
        Out << S;
      else if (const char *S = getTokenDescForDiagnostic(Kind))
        // Unquoted translatable token name.
        Out << S;
      else if (const char *S = tok::getTokenName(Kind))
        // Debug name, shouldn't appear in user-facing diagnostics.
        Out << '<' << S << '>';
      else
        Out << "(null)";
      break;
    }
    // ---- NAMES and TYPES ----
    case DiagnosticsEngine::ArgumentKind::ak_identifierinfo: {
      const IdentifierInfo *II = getArgIdentifier(ArgNo);
      assert(ModifierLen == 0 && "No modifiers for strings yet");

      // Don't crash if get passed a null pointer by accident.
      if (!II) {
        const char *S = "(null)";
        OutStr.append(S, S + strlen(S));
        continue;
      }

      llvm::raw_svector_ostream(OutStr) << '\'' << II->getName() << '\'';
      break;
    }
    case DiagnosticsEngine::ArgumentKind::ak_qual:
    case DiagnosticsEngine::ArgumentKind::ak_qualtype:
    case DiagnosticsEngine::ArgumentKind::ak_declarationname:
    case DiagnosticsEngine::ArgumentKind::ak_nameddecl:
    case DiagnosticsEngine::ArgumentKind::ak_nestednamespec:
    case DiagnosticsEngine::ArgumentKind::ak_attr:
      break;
    case DiagnosticsEngine::ArgumentKind::ak_qualtype_pair:
      break;
    }

    // Remember this argument info for subsequent formatting operations.  Turn
    // std::strings into a null terminated string to make it be the same case as
    // all the other ones.
    if (Kind == DiagnosticsEngine::ArgumentKind::ak_qualtype_pair)
      continue;
    else if (Kind != DiagnosticsEngine::ArgumentKind::ak_std_string)
      FormattedArgs.push_back(std::make_pair(Kind, getRawArg(ArgNo)));
    else
      FormattedArgs.push_back(
          std::make_pair(DiagnosticsEngine::ArgumentKind::ak_c_string,
                         (intptr_t)getArgStdStr(ArgNo).c_str()));
  }

  // Append the type tree to the end of the diagnostics.
  OutStr.append(Tree.begin(), Tree.end());
}

void DiagnosticConsumer::HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                          const Diagnostic &Info) {
  if (!IncludeInDiagnosticCounts())
    return;

  if (DiagLevel == DiagnosticsEngine::Level::Warning)
    ++NumWarnings;
  else if (DiagLevel >= DiagnosticsEngine::Level::Error)
    ++NumErrors;
}

} // namespace soll
