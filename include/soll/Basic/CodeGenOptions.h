// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

enum OptLevel { O0, O1, O2, O3, Os, Oz };

class CodeGenOptions {
public:
  /// Optimization level.
  OptLevel OptimizationLevel;
  /// Generate for runtime only.
  bool Runtime;
};

} // namespace soll
