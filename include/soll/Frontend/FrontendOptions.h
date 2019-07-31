// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>

namespace soll {

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

} // namespace soll
