// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>
#include <vector>

namespace soll {

enum InputKind { Sol, Yul };
enum ActionKind {
  /// Parse ASTs and print them.
  ASTDump,

  /// Emit a .s file.
  EmitAssembly,

  /// Emit a .bc file.
  EmitBC,

  /// Emit a .ll file.
  EmitLLVM,

  /// Generate LLVM IR, but do not emit anything.
  EmitLLVMOnly,

  /// Generate machine code, but don't emit anything.
  EmitCodeGenOnly,

  /// Emit a .o file.
  EmitObj,

  /// Emit function signatures.
  EmitFuncSig,

  /// Emit ABI json.
  EmitABI,

  /// Only execute frontend initialization.
  InitOnly,

  /// Parse and perform semantic analysis.
  ParseSyntaxOnly,
};

class FrontendInputFile {
  /// The file name, or "-" to read from standard input.
  std::string File;
  llvm::MemoryBuffer *Buffer = nullptr;

public:
  FrontendInputFile() = default;
  FrontendInputFile(llvm::StringRef File) : File(File.str()) {}
  FrontendInputFile(llvm::MemoryBuffer *Buffer) : Buffer(Buffer) {}

  bool isEmpty() const { return File.empty() && Buffer == nullptr; }
  bool isFile() const { return !isBuffer(); }
  bool isBuffer() const { return Buffer != nullptr; }

  llvm::StringRef getFile() const {
    assert(isFile());
    return File;
  }

  llvm::MemoryBuffer *getBuffer() const {
    assert(isBuffer());
    return Buffer;
  }
};

class FrontendOptions {
public:
  bool ShowHelp;
  bool ShowVersion;
  std::vector<FrontendInputFile> Inputs;
  InputKind Language = Sol;

  /// The output file, if any.
  std::string OutputFile;

  /// The frontend action to perform.
  ActionKind ProgramAction = EmitLLVM;
};

} // namespace soll
