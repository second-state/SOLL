#include "soll/Basic/DiagnosticIDs.h"
#include "soll/Basic/AllDiagnostics.h"
#include "soll/Basic/Diagnostic.h"

namespace {

template <size_t SizeOfStr, typename FieldType> struct StringSizerHelper {
  static_assert(SizeOfStr <= FieldType(~0U), "Field too small!");
  enum { Size = SizeOfStr };
};
#define STR_SIZE(str, fieldTy) StringSizerHelper<sizeof(str) - 1, fieldTy>::Size

enum {
  CLASS_NOTE = 0x01,
  CLASS_REMARK = 0x02,
  CLASS_WARNING = 0x03,
  CLASS_EXTENSION = 0x04,
  CLASS_ERROR = 0x05
};

} // namespace

namespace soll {

struct StaticDiagInfoRec {
  uint16_t DiagID;
  unsigned DefaultSeverity : 3;
  unsigned Class : 3;
  unsigned WarnNoWerror : 1;
  unsigned WarnShowInSystemHeader : 1;
  unsigned Category : 6;

  uint16_t OptionGroupIndex;

  uint16_t DescriptionLen;
  const char *DescriptionStr;

  unsigned getOptionGroupIndex() const { return OptionGroupIndex; }

  llvm::StringRef getDescription() const {
    return llvm::StringRef(DescriptionStr, DescriptionLen);
  }

  diag::Flavor getFlavor() const {
    return Class == CLASS_REMARK ? diag::Flavor::Remark
                                 : diag::Flavor::WarningOrError;
  }

  bool operator<(const StaticDiagInfoRec &RHS) const {
    return DiagID < RHS.DiagID;
  }
};

static const StaticDiagInfoRec StaticDiagInfo[] = {
#define DIAG(ENUM, CLASS, DEFAULT_SEVERITY, DESC, GROUP, NOWERROR,             \
             SHOWINSYSHEADER, CATEGORY)                                        \
  {diag::ENUM, DEFAULT_SEVERITY,         CLASS,                                \
   NOWERROR,   SHOWINSYSHEADER,          CATEGORY,                             \
   GROUP,      STR_SIZE(DESC, uint16_t), DESC},
#include "soll/Basic/DiagnosticCommonKinds.inc"
//#include "soll/Basic/DiagnosticDriverKinds.inc"
//#include "soll/Basic/DiagnosticFrontendKinds.inc"
//#include "soll/Basic/DiagnosticSerializationKinds.inc"
#include "soll/Basic/DiagnosticLexKinds.inc"
//#include "soll/Basic/DiagnosticParseKinds.inc"
//#include "soll/Basic/DiagnosticASTKinds.inc"
//#include "soll/Basic/DiagnosticCommentKinds.inc"
//#include "soll/Basic/DiagnosticCrossTUKinds.inc"
//#include "soll/Basic/DiagnosticSemaKinds.inc"
//#include "soll/Basic/DiagnosticAnalysisKinds.inc"
//#include "soll/Basic/DiagnosticRefactoringKinds.inc"
#undef DIAG
};

static const unsigned StaticDiagInfoSize = llvm::array_lengthof(StaticDiagInfo);

static const StaticDiagInfoRec *GetDiagInfo(unsigned DiagID) {
  using namespace diag;
  if (DiagID >= DIAG_UPPER_LIMIT || DiagID <= DIAG_START_COMMON)
    return nullptr;

  unsigned Offset = 0;
  unsigned ID = DiagID - DIAG_START_COMMON - 1;
#define CATEGORY(NAME, PREV)                                                   \
  if (DiagID > DIAG_START_##NAME) {                                            \
    Offset += NUM_BUILTIN_##PREV##_DIAGNOSTICS - DIAG_START_##PREV - 1;        \
    ID -= DIAG_START_##NAME - DIAG_START_##PREV;                               \
  }
  CATEGORY(LEX, COMMON)
// CATEGORY(DRIVER, COMMON)
// CATEGORY(FRONTEND, DRIVER)
// CATEGORY(SERIALIZATION, FRONTEND)
// CATEGORY(LEX, SERIALIZATION)
// CATEGORY(PARSE, LEX)
// CATEGORY(AST, PARSE)
// CATEGORY(COMMENT, AST)
// CATEGORY(CROSSTU, COMMENT)
// CATEGORY(SEMA, CROSSTU)
// CATEGORY(ANALYSIS, SEMA)
// CATEGORY(REFACTORING, ANALYSIS)
#undef CATEGORY

  if (ID + Offset >= StaticDiagInfoSize)
    return nullptr;

  assert(ID < StaticDiagInfoSize && Offset < StaticDiagInfoSize);

  const StaticDiagInfoRec *Found = &StaticDiagInfo[ID + Offset];
  if (Found->DiagID != DiagID)
    return nullptr;
  return Found;
}

static unsigned getBuiltinDiagClass(unsigned DiagID) {
  if (const StaticDiagInfoRec *Info = GetDiagInfo(DiagID))
    return Info->Class;
  return ~0U;
}

static DiagnosticIDs::Level toLevel(diag::Severity SV) {
  switch (SV) {
  case diag::Severity::Ignored:
    return DiagnosticIDs::Ignored;
  case diag::Severity::Remark:
    return DiagnosticIDs::Remark;
  case diag::Severity::Warning:
    return DiagnosticIDs::Warning;
  case diag::Severity::Error:
    return DiagnosticIDs::Error;
  case diag::Severity::Fatal:
    return DiagnosticIDs::Fatal;
  }
  llvm_unreachable("unexpected severity");
}

DiagnosticIDs::Level
DiagnosticIDs::getDiagnosticLevel(unsigned DiagID, SourceLocation Loc,
                                  const DiagnosticsEngine &Diag) const {
  assert(DiagID < diag::DIAG_UPPER_LIMIT && "Invalid DiagID");

  unsigned DiagClass = getBuiltinDiagClass(DiagID);
  if (DiagClass == CLASS_NOTE)
    return DiagnosticIDs::Note;
  return toLevel(getDiagnosticSeverity(DiagID, Loc, Diag));
}

diag::Severity
DiagnosticIDs::getDiagnosticSeverity(unsigned DiagID, SourceLocation Loc,
                                     const DiagnosticsEngine &Diag) const {
  assert(getBuiltinDiagClass(DiagID) != CLASS_NOTE);

  switch (getBuiltinDiagClass(DiagID)) {
  case CLASS_REMARK:
    return diag::Severity::Remark;
  case CLASS_EXTENSION:
  case CLASS_WARNING:
    return diag::Severity::Warning;
  case CLASS_ERROR:
  default:
    return diag::Severity::Error;
  }
}

bool DiagnosticIDs::ProcessDiag(DiagnosticsEngine &Diag) const {
  Diagnostic Info(&Diag);

  assert(Diag.getClient() && "DiagnosticClient not set!");

  unsigned DiagID = Info.getID();
  DiagnosticIDs::Level DiagLevel =
      getDiagnosticLevel(DiagID, Info.getLocation(), Diag);

  EmitDiag(Diag, DiagLevel);
  return true;
}

void DiagnosticIDs::EmitDiag(DiagnosticsEngine &Diag, Level DiagLevel) const {
  Diagnostic Info(&Diag);
  assert(DiagLevel != DiagnosticIDs::Ignored &&
         "Cannot emit ignored diagnostics!");

  Diag.Client->HandleDiagnostic(
      static_cast<DiagnosticsEngine::Level>(DiagLevel), Info);
  if (Diag.Client->IncludeInDiagnosticCounts()) {
    if (DiagLevel == DiagnosticIDs::Warning)
      ++Diag.NumWarnings;
  }

  Diag.CurDiagID = ~0U;
}

bool DiagnosticIDs::isUnrecoverable(unsigned DiagID) const {
  assert(DiagID < diag::DIAG_UPPER_LIMIT && "Invalid DiagID");

  if (getBuiltinDiagClass(DiagID) < CLASS_ERROR)
    return false;

  return true;
}

} // namespace soll
