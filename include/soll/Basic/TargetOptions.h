// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

enum TargetKind { EWASM, EVM };

class TargetOptions {
public:
  TargetKind BackendTarget = EWASM;
};

} // namespace soll
