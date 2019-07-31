// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <cstdint>
#include <llvm/Support/PointerLikeTypeTraits.h>
#include <llvm/Support/raw_ostream.h>

namespace soll {

class SourceManager;

class FileID {
  int ID = 0;

public:
  bool isValid() const { return ID != 0; }
  bool isInvalid() const { return ID == 0; }

  bool operator==(const FileID &RHS) const { return ID == RHS.ID; }
  bool operator<(const FileID &RHS) const { return ID < RHS.ID; }
  bool operator<=(const FileID &RHS) const { return ID <= RHS.ID; }
  bool operator!=(const FileID &RHS) const { return !(*this == RHS); }
  bool operator>(const FileID &RHS) const { return RHS < *this; }
  bool operator>=(const FileID &RHS) const { return RHS <= *this; }

  static FileID getSentinel() { return get(-1); }
  unsigned getHashValue() const { return static_cast<unsigned>(ID); }

private:
  friend class ASTWriter;
  friend class ASTReader;
  friend class SourceManager;

  static FileID get(int V) {
    FileID F;
    F.ID = V;
    return F;
  }

  int getOpaqueValue() const { return ID; }
};

class SourceLocation {
  unsigned ID;

public:
  bool isValid() const { return ID != 0; }
  bool isInvalid() const { return ID == 0; }

private:
  friend class SourceManager;
  static SourceLocation get(unsigned ID) {
    SourceLocation L;
    L.ID = ID;
    return L;
  }

public:
  SourceLocation getLocWithOffset(int Offset) const {
    SourceLocation L;
    L.ID = ID + Offset;
    return L;
  }

  unsigned getRawEncoding() const { return ID; }
  static SourceLocation getFromRawEncoding(unsigned Encoding) {
    SourceLocation X;
    X.ID = Encoding;
    return X;
  }

  void *getPtrEncoding() const {
    return reinterpret_cast<void *>(static_cast<uintptr_t>(getRawEncoding()));
  }
  static SourceLocation getFromPtrEncoding(const void *Encoding) {
    return getFromRawEncoding(
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(Encoding)));
  }

  void print(llvm::raw_ostream &OS, const SourceManager &SM);
};

inline bool operator==(const SourceLocation &LHS, const SourceLocation &RHS) {
  return LHS.getRawEncoding() == RHS.getRawEncoding();
}

inline bool operator!=(const SourceLocation &LHS, const SourceLocation &RHS) {
  return !(LHS == RHS);
}

inline bool operator<(const SourceLocation &LHS, const SourceLocation &RHS) {
  return LHS.getRawEncoding() < RHS.getRawEncoding();
}

class SourceRange {
  SourceLocation B;
  SourceLocation E;

public:
  SourceRange() = default;
  SourceRange(SourceLocation loc) : B(loc), E(loc) {}
  SourceRange(SourceLocation begin, SourceLocation end) : B(begin), E(end) {}

  SourceLocation getBegin() const { return B; }
  SourceLocation getEnd() const { return E; }

  void setBegin(SourceLocation b) { B = b; }
  void setEnd(SourceLocation e) { E = e; }

  bool isValid() const { return B.isValid() && E.isValid(); }
  bool isInvalid() const { return !isValid(); }

  bool operator==(const SourceRange &X) const { return B == X.B && E == X.E; }

  bool operator!=(const SourceRange &X) const { return B != X.B || E != X.E; }

  void print(llvm::raw_ostream &OS, const SourceManager &SM) const;
  std::string printToString(const SourceManager &SM) const;
  void dump(const SourceManager &SM) const;
};

class CharSourceRange {
  SourceRange Range;
  bool IsTokenRange = false;

public:
  CharSourceRange() = default;
  CharSourceRange(SourceRange R, bool ITR) : Range(R), IsTokenRange(ITR) {}

  static CharSourceRange getTokenRange(SourceRange R) {
    return CharSourceRange(R, true);
  }

  static CharSourceRange getCharRange(SourceRange R) {
    return CharSourceRange(R, false);
  }

  static CharSourceRange getTokenRange(SourceLocation B, SourceLocation E) {
    return getTokenRange(SourceRange(B, E));
  }

  static CharSourceRange getCharRange(SourceLocation B, SourceLocation E) {
    return getCharRange(SourceRange(B, E));
  }

  bool isTokenRange() const { return IsTokenRange; }
  bool isCharRange() const { return !IsTokenRange; }

  SourceLocation getBegin() const { return Range.getBegin(); }
  SourceLocation getEnd() const { return Range.getEnd(); }
  SourceRange getAsRange() const { return Range; }

  void setBegin(SourceLocation b) { Range.setBegin(b); }
  void setEnd(SourceLocation e) { Range.setEnd(e); }
  void setTokenRange(bool TR) { IsTokenRange = TR; }

  bool isValid() const { return Range.isValid(); }
  bool isInvalid() const { return !isValid(); }
};

class PresumedLoc {
  const char *Filename = nullptr;
  unsigned Line, Col;
  SourceLocation IncludeLoc;

public:
  PresumedLoc() = default;
  PresumedLoc(const char *FN, unsigned Ln, unsigned Co, SourceLocation IL)
      : Filename(FN), Line(Ln), Col(Co), IncludeLoc(IL) {}

  bool isInvalid() const { return Filename == nullptr; }
  bool isValid() const { return Filename != nullptr; }

  const char *getFilename() const {
    assert(isValid());
    return Filename;
  }

  unsigned getLine() const {
    assert(isValid());
    return Line;
  }

  unsigned getColumn() const {
    assert(isValid());
    return Col;
  }

  SourceLocation getIncludeLoc() const {
    assert(isValid());
    return IncludeLoc;
  }
};

class FileEntry;

class FullSourceLoc : public SourceLocation {
  const SourceManager *SrcMgr = nullptr;

public:
  /// Creates a FullSourceLoc where isValid() returns \c false.
  FullSourceLoc() = default;

  explicit FullSourceLoc(SourceLocation Loc, const SourceManager &SM)
      : SourceLocation(Loc), SrcMgr(&SM) {}

  bool hasManager() const {
    bool hasSrcMgr = SrcMgr != nullptr;
    assert(hasSrcMgr == isValid() &&
           "FullSourceLoc has location but no manager");
    return hasSrcMgr;
  }

  /// \pre This FullSourceLoc has an associated SourceManager.
  const SourceManager &getManager() const {
    assert(SrcMgr && "SourceManager is NULL.");
    return *SrcMgr;
  }

  FileID getFileID() const;

  FullSourceLoc getExpansionLoc() const;
  FullSourceLoc getSpellingLoc() const;
  FullSourceLoc getFileLoc() const;
  PresumedLoc getPresumedLoc(bool UseLineDirectives = true) const;
  bool isMacroArgExpansion(FullSourceLoc *StartLoc = nullptr) const;
  FullSourceLoc getImmediateMacroCallerLoc() const;
  std::pair<FullSourceLoc, llvm::StringRef> getModuleImportLoc() const;
  unsigned getFileOffset() const;

  unsigned getExpansionLineNumber() const;
  unsigned getExpansionColumnNumber() const;

  unsigned getSpellingLineNumber() const;
  unsigned getSpellingColumnNumber() const;

  const char *getCharacterData() const;

  unsigned getLineNumber() const;
  unsigned getColumnNumber() const;

  const FileEntry *getFileEntry() const;

  /// Return a StringRef to the source buffer data for the
  /// specified FileID.
  llvm::StringRef getBufferData() const;

  /// Decompose the specified location into a raw FileID + Offset pair.
  ///
  /// The first element is the FileID, the second is the offset from the
  /// start of the buffer of the location.
  std::pair<FileID, unsigned> getDecomposedLoc() const;

  bool isInSystemHeader() const;

  /// Determines the order of 2 source locations in the translation unit.
  ///
  /// \returns true if this source location comes before 'Loc', false otherwise.
  bool isBeforeInTranslationUnitThan(SourceLocation Loc) const;

  /// Determines the order of 2 source locations in the translation unit.
  ///
  /// \returns true if this source location comes before 'Loc', false otherwise.
  bool isBeforeInTranslationUnitThan(FullSourceLoc Loc) const {
    assert(Loc.isValid());
    assert(SrcMgr == Loc.SrcMgr && "Loc comes from another SourceManager!");
    return isBeforeInTranslationUnitThan((SourceLocation)Loc);
  }

  /// Comparison function class, useful for sorting FullSourceLocs.
  struct BeforeThanCompare {
    bool operator()(const FullSourceLoc &lhs, const FullSourceLoc &rhs) const {
      return lhs.isBeforeInTranslationUnitThan(rhs);
    }
  };

  /// Prints information about this FullSourceLoc to stderr.
  ///
  /// This is useful for debugging.
  void dump() const;

  friend bool operator==(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
    return LHS.getRawEncoding() == RHS.getRawEncoding() &&
           LHS.SrcMgr == RHS.SrcMgr;
  }

  friend bool operator!=(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
    return !(LHS == RHS);
  }
};

} // namespace soll

namespace llvm {

template <typename T> struct DenseMapInfo;

template <> struct DenseMapInfo<soll::FileID> {
  static soll::FileID getEmptyKey() { return {}; }

  static soll::FileID getTombstoneKey() { return soll::FileID::getSentinel(); }

  static unsigned getHashValue(soll::FileID S) { return S.getHashValue(); }

  static bool isEqual(soll::FileID LHS, soll::FileID RHS) { return LHS == RHS; }
};

template <typename T> struct isPodLike;

template <> struct isPodLike<soll::SourceLocation> {
  static const bool value = true;
};
template <> struct isPodLike<soll::FileID> { static const bool value = true; };

template <> struct PointerLikeTypeTraits<soll::SourceLocation> {
  enum { NumLowBitsAvailable = 0 };

  static void *getAsVoidPointer(soll::SourceLocation L) {
    return L.getPtrEncoding();
  }

  static soll::SourceLocation getFromVoidPointer(void *P) {
    return soll::SourceLocation::getFromRawEncoding((unsigned)(uintptr_t)P);
  }
};

} // namespace llvm
