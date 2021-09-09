// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/AST.h"
#include "soll/AST/Decl.h"
#include "soll/Basic/DiagnosticSema.h"
#include "soll/Sema/Sema.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace soll {
/// Magic variables get negative ids for easy differentiation
namespace {
int magicVariableToID(std::string const &_name) {
  if (_name == "abi")
    return -1;
  else if (_name == "addmod")
    return -2;
  else if (_name == "assert")
    return -3;
  else if (_name == "block")
    return -4;
  else if (_name == "blockhash")
    return -5;
  else if (_name == "ecrecover")
    return -6;
  else if (_name == "gasleft")
    return -7;
  else if (_name == "keccak256")
    return -8;
  else if (_name == "msg")
    return -15;
  else if (_name == "mulmod")
    return -16;
  else if (_name == "now")
    return -17;
  else if (_name == "require")
    return -18;
  else if (_name == "revert")
    return -19;
  else if (_name == "ripemd160")
    return -20;
  else if (_name == "selfdestruct")
    return -21;
  else if (_name == "sha256")
    return -22;
  else if (_name == "sha3")
    return -23;
  else if (_name == "suicide")
    return -24;
  else if (_name == "super")
    return -25;
  else if (_name == "tx")
    return -26;
  else if (_name == "type")
    return -27;
  else if (_name == "this")
    return -28;
  else
    __builtin_unreachable();
}
} // namespace

class GlobalContext {
public:
  void setCurrentContract(const ContractDecl *Cont) { CurrentContract = Cont; }
  void resetCurrentContract() { CurrentContract = nullptr; }
  const MagicVariableDecl *currentThis() {
    if (!ThisCache[CurrentContract]) {
      TypePtr Type = CurrentContract->getType();
      ThisCache[CurrentContract] = std::make_shared<MagicVariableDecl>(
          magicVariableToID("this"), "this", Type);
    }
    return ThisCache[CurrentContract].get();
  }

  const MagicVariableDecl *currentSuper() {
    assert(false && "unimp currentSuper");
    return nullptr;
  }

  GlobalContext() = default;
  GlobalContext(const GlobalContext &) = delete;
  ~GlobalContext() = default;

private:
  const ContractDecl *CurrentContract = nullptr;
  std::map<const ContractDecl *, std::shared_ptr<MagicVariableDecl>> ThisCache;
  std::map<const ContractDecl *, std::shared_ptr<MagicVariableDecl>> SuperCache;
};
} // namespace soll