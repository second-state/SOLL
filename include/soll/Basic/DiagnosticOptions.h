// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once
#include <llvm/ADT/IntrusiveRefCntPtr.h>

namespace soll {

class DiagnosticOptions : public llvm::RefCountedBase<DiagnosticOptions> {
public:
  enum class Format : bool { Soll, Vi };
  DiagnosticOptions() {}

private:
  Format m_Format : 1;
  bool m_WError : 1;
};

} // namespace soll
