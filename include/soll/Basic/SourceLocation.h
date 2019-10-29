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
  unsigned getOffset() const { return ID; }

private:
  friend class SourceManager;
  static SourceLocation get(unsigned ID) {
    SourceLocation L;
    L.ID = ID;
    return L;
  }

public:
  unsigned getRawEncoding() const { return ID; }

  static SourceLocation getFromRawEncoding(unsigned Encoding) {
    SourceLocation X;
    X.ID = Encoding;
    return X;
  }

  SourceLocation getLocWithOffset(int Offset) const {
    SourceLocation L;
    L.ID = this->ID + Offset;
    return L;
  }

  void print(llvm::raw_ostream &OS, const SourceManager &SM);
};

inline bool operator==(const SourceLocation &LHS, const SourceLocation &RHS) {
  return LHS.getOffset() == RHS.getOffset();
}

inline bool operator!=(const SourceLocation &LHS, const SourceLocation &RHS) {
  return LHS.getOffset() != RHS.getOffset();
}

inline bool operator<(const SourceLocation &LHS, const SourceLocation &RHS) {
  return LHS.getOffset() < RHS.getOffset();
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
  PresumedLoc getPresumedLoc() const;
  std::pair<FullSourceLoc, llvm::StringRef> getModuleImportLoc() const;
  unsigned getFileOffset() const;

  llvm::Optional<unsigned> getLineNumber() const;
  llvm::Optional<unsigned> getColumnNumber() const;

  const FileEntry *getFileEntry() const;

  llvm::StringRef getBufferData() const;

  std::pair<FileID, unsigned> getDecomposedLoc() const;

  void dump() const;

  friend bool operator==(const FullSourceLoc &LHS, const FullSourceLoc &RHS) {
    return static_cast<const SourceLocation &>(LHS) ==
               static_cast<const SourceLocation &>(RHS) &&
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

} // namespace llvm
