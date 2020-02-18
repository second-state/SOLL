// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

namespace soll {

enum TargetKind { EWASM, EVM };
enum DeployPlatformKind { Normal, Chain };

class TargetOptions {
public:
  TargetKind BackendTarget = EWASM;
  DeployPlatformKind DeployPlatform = Normal;
};

} // namespace soll
