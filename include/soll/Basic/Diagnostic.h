#pragma once
#include "soll/Basic/DiagnosticIDs.h"
#include "soll/Basic/DiagnosticOptions.h"
#include "soll/Basic/IdentifierTable.h"
#include "soll/Basic/SourceLocation.h"
#include "soll/Basic/TokenKinds.h"
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>

namespace soll {

class DiagnosticBuilder;
class DiagnosticConsumer;
class StoredDiagnostic;

class FixItHint {
public:
  CharSourceRange RemoveRange;
  CharSourceRange InsertFromRange;
  std::string CodeToInsert;
  bool BeforePreviousInsertions = false;

  FixItHint() = default;

  bool isNull() const { return !RemoveRange.isValid(); }

  static FixItHint CreateInsertion(SourceLocation InsertionLoc,
                                   llvm::StringRef Code,
                                   bool BeforePreviousInsertions = false) {
    FixItHint Hint;
    Hint.RemoveRange =
        CharSourceRange::getCharRange(InsertionLoc, InsertionLoc);
    Hint.CodeToInsert = Code;
    Hint.BeforePreviousInsertions = BeforePreviousInsertions;
    return Hint;
  }

  static FixItHint
  CreateInsertionFromRange(SourceLocation InsertionLoc,
                           CharSourceRange FromRange,
                           bool BeforePreviousInsertions = false) {
    FixItHint Hint;
    Hint.RemoveRange =
        CharSourceRange::getCharRange(InsertionLoc, InsertionLoc);
    Hint.InsertFromRange = FromRange;
    Hint.BeforePreviousInsertions = BeforePreviousInsertions;
    return Hint;
  }

  static FixItHint CreateRemoval(CharSourceRange RemoveRange) {
    FixItHint Hint;
    Hint.RemoveRange = RemoveRange;
    return Hint;
  }
  static FixItHint CreateRemoval(SourceRange RemoveRange) {
    return CreateRemoval(CharSourceRange::getTokenRange(RemoveRange));
  }

  static FixItHint CreateReplacement(CharSourceRange RemoveRange,
                                     llvm::StringRef Code) {
    FixItHint Hint;
    Hint.RemoveRange = RemoveRange;
    Hint.CodeToInsert = Code;
    return Hint;
  }

  static FixItHint CreateReplacement(SourceRange RemoveRange,
                                     llvm::StringRef Code) {
    return CreateReplacement(CharSourceRange::getTokenRange(RemoveRange), Code);
  }
};

class DiagnosticsEngine : public llvm::RefCountedBase<DiagnosticsEngine> {
  DiagnosticsEngine(const DiagnosticsEngine &) = delete;
  DiagnosticsEngine &operator=(const DiagnosticsEngine &) = delete;

public:
  enum class Level {
    Ignored,
    Note,
    Remark,
    Warning,
    Error,
    Fatal,
  };

  enum class ArgumentKind : unsigned char {
    /// std::string
    ak_std_string,

    /// const char *
    ak_c_string,

    /// int
    ak_sint,

    /// unsigned
    ak_uint,

    /// enum TokenKind : unsigned
    ak_tokenkind,

    /// IdentifierInfo
    ak_identifierinfo,

    /// Qualifiers
    ak_qual,

    /// QualType
    ak_qualtype,

    /// DeclarationName
    ak_declarationname,

    /// NamedDecl *
    ak_nameddecl,

    /// NestedNameSpecifier *
    ak_nestednamespec,

    /*
        /// DeclContext *
        ak_declcontext,
    */

    /// pair<QualType, QualType>
    ak_qualtype_pair,

    /// Attr *
    ak_attr
  };

public:
  explicit DiagnosticsEngine(
      llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags,
      llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts,
      DiagnosticConsumer *client = nullptr, bool ShouldOwnClient = true);

  const llvm::IntrusiveRefCntPtr<DiagnosticIDs> &getDiagnosticIDs() const {
    return Diags;
  }
  DiagnosticOptions &getDiagnosticOptions() const { return *DiagOpts; }

  DiagnosticConsumer *getClient() { return Client; }
  const DiagnosticConsumer *getClient() const { return Client; }

  bool hasSourceManager() const { return SourceMgr != nullptr; }
  SourceManager &getSourceManager() const {
    assert(SourceMgr && "SourceManager not set!");
    return *SourceMgr;
  }
  void setSourceManager(SourceManager *SrcMgr) { SourceMgr = SrcMgr; }

  void setClient(DiagnosticConsumer *client, bool ShouldOwnClient = true);

  inline DiagnosticBuilder Report(SourceLocation Loc, unsigned DiagID);
  inline DiagnosticBuilder Report(unsigned DiagID);
  void Report(const StoredDiagnostic &storedDiag);

  void Clear() { CurDiagID = std::numeric_limits<unsigned>::max(); }

private:
  friend class Diagnostic;
  friend class DiagnosticIDs;
  friend class DiagnosticBuilder;

  llvm::IntrusiveRefCntPtr<DiagnosticIDs> Diags;
  llvm::IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts;
  DiagnosticConsumer *Client = nullptr;
  std::unique_ptr<DiagnosticConsumer> Owner;
  SourceManager *SourceMgr = nullptr;

  unsigned NumWarnings = 0;
  unsigned NumErrors = 0;

  SourceLocation CurDiagLoc;
  unsigned CurDiagID = std::numeric_limits<unsigned>::max();

  enum {
    MaxArguments = 10,
  };
  signed char NumDiagArgs;
  ArgumentKind DiagArgumentsKind[MaxArguments];
  std::string DiagArgumentsStr[MaxArguments];
  intptr_t DiagArgumentsVal[MaxArguments];
  llvm::SmallVector<CharSourceRange, 8> DiagRanges;
  llvm::SmallVector<FixItHint, 8> DiagFixItHints;

  unsigned DelayedDiagID = 0;
  std::string DelayedDiagArg1;
  std::string DelayedDiagArg2;

  void ReportDelayed();

  std::string FlagValue;

protected:
  bool EmitCurrentDiagnostic(bool Force = false);
};

class DiagnosticBuilder {
  friend class DiagnosticsEngine;
  friend class PartialDiagnostic;

  mutable DiagnosticsEngine *DiagObj = nullptr;
  mutable unsigned NumArgs = 0;

  mutable bool IsActive = false;

  mutable bool IsForceEmit = false;

  DiagnosticBuilder() = default;

  explicit DiagnosticBuilder(DiagnosticsEngine *diagObj)
      : DiagObj(diagObj), IsActive(true) {
    assert(diagObj && "DiagnosticBuilder requires a valid DiagnosticsEngine!");
    diagObj->DiagRanges.clear();
    diagObj->DiagFixItHints.clear();
  }

protected:
  void FlushCounts() { DiagObj->NumDiagArgs = NumArgs; }

  void Clear() const {
    DiagObj = nullptr;
    IsActive = false;
    IsForceEmit = false;
  }

  bool isActive() const { return IsActive; }

  bool Emit() {
    if (!isActive())
      return false;

    FlushCounts();

    bool Result = DiagObj->EmitCurrentDiagnostic(IsForceEmit);

    Clear();

    return Result;
  }

public:
  DiagnosticBuilder(const DiagnosticBuilder &D) {
    DiagObj = D.DiagObj;
    IsActive = D.IsActive;
    IsForceEmit = D.IsForceEmit;
    D.Clear();
    NumArgs = D.NumArgs;
  }

  DiagnosticBuilder &operator=(const DiagnosticBuilder &) = delete;

  ~DiagnosticBuilder() { Emit(); }

  static DiagnosticBuilder getEmpty() { return {}; }

  const DiagnosticBuilder &setForceEmit() const {
    IsForceEmit = true;
    return *this;
  }

  operator bool() const { return true; }

  void AddString(llvm::StringRef S) const {
    assert(isActive() && "Clients must not add to cleared diagnostic!");
    assert(NumArgs < DiagnosticsEngine::MaxArguments &&
           "Too many arguments to diagnostic!");
    DiagObj->DiagArgumentsKind[NumArgs] =
        DiagnosticsEngine::ArgumentKind::ak_std_string;
    DiagObj->DiagArgumentsStr[NumArgs++] = S;
  }

  void AddTaggedVal(intptr_t V, DiagnosticsEngine::ArgumentKind Kind) const {
    assert(isActive() && "Clients must not add to cleared diagnostic!");
    assert(NumArgs < DiagnosticsEngine::MaxArguments &&
           "Too many arguments to diagnostic!");
    DiagObj->DiagArgumentsKind[NumArgs] = Kind;
    DiagObj->DiagArgumentsVal[NumArgs++] = V;
  }

  void AddSourceRange(const CharSourceRange &R) const {
    assert(isActive() && "Clients must not add to cleared diagnostic!");
    DiagObj->DiagRanges.push_back(R);
  }

  void AddFixItHint(const FixItHint &Hint) const {
    assert(isActive() && "Clients must not add to cleared diagnostic!");
    if (!Hint.isNull())
      DiagObj->DiagFixItHints.push_back(Hint);
  }

  void addFlagValue(llvm::StringRef V) const { DiagObj->FlagValue = V; }
};

struct AddFlagValue {
  llvm::StringRef Val;

  explicit AddFlagValue(llvm::StringRef V) : Val(V) {}
};

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           const AddFlagValue V) {
  DB.addFlagValue(V.Val);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           llvm::StringRef S) {
  DB.AddString(S);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           const char *Str) {
  DB.AddTaggedVal(reinterpret_cast<intptr_t>(Str),
                  DiagnosticsEngine::ArgumentKind::ak_c_string);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB, int I) {
  DB.AddTaggedVal(I, DiagnosticsEngine::ArgumentKind::ak_sint);
  return DB;
}

template <typename T>
inline std::enable_if_t<std::is_same<T, bool>::value, const DiagnosticBuilder &>
operator<<(const DiagnosticBuilder &DB, T I) {
  DB.AddTaggedVal(I, DiagnosticsEngine::ArgumentKind::ak_sint);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           unsigned I) {
  DB.AddTaggedVal(I, DiagnosticsEngine::ArgumentKind::ak_uint);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           tok::TokenKind I) {
  DB.AddTaggedVal(static_cast<unsigned>(I),
                  DiagnosticsEngine::ArgumentKind::ak_tokenkind);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           const IdentifierInfo *II) {
  DB.AddTaggedVal(reinterpret_cast<intptr_t>(II),
                  DiagnosticsEngine::ArgumentKind::ak_identifierinfo);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           SourceRange R) {
  DB.AddSourceRange(CharSourceRange::getTokenRange(R));
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           llvm::ArrayRef<SourceRange> Ranges) {
  for (SourceRange R : Ranges)
    DB.AddSourceRange(CharSourceRange::getTokenRange(R));
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           const CharSourceRange &R) {
  DB.AddSourceRange(R);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           const FixItHint &Hint) {
  DB.AddFixItHint(Hint);
  return DB;
}

inline const DiagnosticBuilder &operator<<(const DiagnosticBuilder &DB,
                                           llvm::ArrayRef<FixItHint> Hints) {
  for (const FixItHint &Hint : Hints)
    DB.AddFixItHint(Hint);
  return DB;
}

inline DiagnosticBuilder DiagnosticsEngine::Report(SourceLocation Loc,
                                                   unsigned DiagID) {
  assert(CurDiagID == std::numeric_limits<unsigned>::max() &&
         "Multiple diagnostics in flight at once!");
  CurDiagLoc = Loc;
  CurDiagID = DiagID;
  FlagValue.clear();
  return DiagnosticBuilder(this);
}

inline DiagnosticBuilder DiagnosticsEngine::Report(unsigned DiagID) {
  return Report(SourceLocation(), DiagID);
}

class Diagnostic {
  const DiagnosticsEngine *DiagObj;
  llvm::StringRef StoredDiagMessage;

public:
  explicit Diagnostic(const DiagnosticsEngine *DO) : DiagObj(DO) {}
  Diagnostic(const DiagnosticsEngine *DO, llvm::StringRef storedDiagMessage)
      : DiagObj(DO), StoredDiagMessage(storedDiagMessage) {}

  const DiagnosticsEngine *getDiags() const { return DiagObj; }
  unsigned getID() const { return DiagObj->CurDiagID; }
  const SourceLocation &getLocation() const { return DiagObj->CurDiagLoc; }
  bool hasSourceManager() const { return DiagObj->hasSourceManager(); }
  SourceManager &getSourceManager() const {
    return DiagObj->getSourceManager();
  }

  unsigned getNumArgs() const { return DiagObj->NumDiagArgs; }

  DiagnosticsEngine::ArgumentKind getArgKind(unsigned Idx) const {
    assert(Idx < getNumArgs() && "Argument index out of range!");
    return static_cast<DiagnosticsEngine::ArgumentKind>(
        DiagObj->DiagArgumentsKind[Idx]);
  }

  const std::string &getArgStdStr(unsigned Idx) const {
    assert(getArgKind(Idx) == DiagnosticsEngine::ArgumentKind::ak_std_string &&
           "invalid argument accessor!");
    return DiagObj->DiagArgumentsStr[Idx];
  }

  const char *getArgCStr(unsigned Idx) const {
    assert(getArgKind(Idx) == DiagnosticsEngine::ArgumentKind::ak_c_string &&
           "invalid argument accessor!");
    return reinterpret_cast<const char *>(DiagObj->DiagArgumentsVal[Idx]);
  }

  int getArgSInt(unsigned Idx) const {
    assert(getArgKind(Idx) == DiagnosticsEngine::ArgumentKind::ak_sint &&
           "invalid argument accessor!");
    return (int)DiagObj->DiagArgumentsVal[Idx];
  }

  unsigned getArgUInt(unsigned Idx) const {
    assert(getArgKind(Idx) == DiagnosticsEngine::ArgumentKind::ak_uint &&
           "invalid argument accessor!");
    return (unsigned)DiagObj->DiagArgumentsVal[Idx];
  }

  const IdentifierInfo *getArgIdentifier(unsigned Idx) const {
    assert(getArgKind(Idx) ==
               DiagnosticsEngine::ArgumentKind::ak_identifierinfo &&
           "invalid argument accessor!");
    return reinterpret_cast<IdentifierInfo *>(DiagObj->DiagArgumentsVal[Idx]);
  }

  intptr_t getRawArg(unsigned Idx) const {
    assert(getArgKind(Idx) != DiagnosticsEngine::ArgumentKind::ak_std_string &&
           "invalid argument accessor!");
    return DiagObj->DiagArgumentsVal[Idx];
  }

  unsigned getNumRanges() const { return DiagObj->DiagRanges.size(); }

  const CharSourceRange &getRange(unsigned Idx) const {
    assert(Idx < getNumRanges() && "Invalid diagnostic range index!");
    return DiagObj->DiagRanges[Idx];
  }

  llvm::ArrayRef<CharSourceRange> getRanges() const {
    return DiagObj->DiagRanges;
  }

  unsigned getNumFixItHints() const { return DiagObj->DiagFixItHints.size(); }

  const FixItHint &getFixItHint(unsigned Idx) const {
    assert(Idx < getNumFixItHints() && "Invalid index!");
    return DiagObj->DiagFixItHints[Idx];
  }

  llvm::ArrayRef<FixItHint> getFixItHints() const {
    return DiagObj->DiagFixItHints;
  }

  void FormatDiagnostic(llvm::SmallVectorImpl<char> &OutStr) const;

  void FormatDiagnostic(const char *DiagStr, const char *DiagEnd,
                        llvm::SmallVectorImpl<char> &OutStr) const;
};

class StoredDiagnostic {
  unsigned ID;
  DiagnosticsEngine::Level Level;
  FullSourceLoc Loc;
  std::string Message;
  std::vector<CharSourceRange> Ranges;
  std::vector<FixItHint> FixIts;

public:
  StoredDiagnostic() = default;
  StoredDiagnostic(DiagnosticsEngine::Level Level, const Diagnostic &Info);
  StoredDiagnostic(DiagnosticsEngine::Level Level, unsigned ID,
                   llvm::StringRef Message);
  StoredDiagnostic(DiagnosticsEngine::Level Level, unsigned ID,
                   llvm::StringRef Message, FullSourceLoc Loc,
                   llvm::ArrayRef<CharSourceRange> Ranges,
                   llvm::ArrayRef<FixItHint> Fixits);

  explicit operator bool() const { return !Message.empty(); }

  unsigned getID() const { return ID; }
  DiagnosticsEngine::Level getLevel() const { return Level; }
  const FullSourceLoc &getLocation() const { return Loc; }
  llvm::StringRef getMessage() const { return Message; }

  void setLocation(FullSourceLoc Loc) { this->Loc = Loc; }

  using range_iterator = std::vector<CharSourceRange>::const_iterator;

  range_iterator range_begin() const { return Ranges.begin(); }
  range_iterator range_end() const { return Ranges.end(); }
  unsigned range_size() const { return Ranges.size(); }

  llvm::ArrayRef<CharSourceRange> getRanges() const {
    return llvm::makeArrayRef(Ranges);
  }

  using fixit_iterator = std::vector<FixItHint>::const_iterator;

  fixit_iterator fixit_begin() const { return FixIts.begin(); }
  fixit_iterator fixit_end() const { return FixIts.end(); }
  unsigned fixit_size() const { return FixIts.size(); }

  llvm::ArrayRef<FixItHint> getFixIts() const {
    return llvm::makeArrayRef(FixIts);
  }
};

class DiagnosticConsumer {
protected:
  unsigned NumWarnings = 0;
  unsigned NumErrors = 0;

public:
  DiagnosticConsumer() = default;
  virtual ~DiagnosticConsumer() = default;
  virtual void clear() {}
  virtual void BeginSourceFile() {}
  virtual void EndSourceFile() {}
  virtual void finish() {}
  virtual bool IncludeInDiagnosticCounts() const { return true; }
  virtual void HandleDiagnostic(DiagnosticsEngine::Level DiagLevel,
                                const Diagnostic &Info) {}
};

} // namespace soll
