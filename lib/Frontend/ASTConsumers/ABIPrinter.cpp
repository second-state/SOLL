// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "../utils/json/json.hpp"
#include "soll/AST/AST.h"
#include "soll/AST/ASTConsumer.h"
#include "soll/Frontend/ASTConsumers.h"
#include <llvm/Support/raw_ostream.h>

using json = nlohmann::json;

namespace {

std::string ToString(soll::StateMutability SM) {
  switch (SM) {
  case soll::StateMutability::NonPayable:
    return "nonpayable";
  case soll::StateMutability::Payable:
    return "payable";
  case soll::StateMutability::Pure:
    return "pure";
  case soll::StateMutability::View:
    return "view";
  default:
    __builtin_unreachable();
  }
}

bool GetPayable(soll::StateMutability SM) {
  return SM == soll::StateMutability::Payable;
}

bool GetConstant(soll::StateMutability SM) {
  return SM == soll::StateMutability::Pure || SM == soll::StateMutability::View;
}

} // namespace

namespace soll {

class ABIPrinter : public ASTConsumer, public ConstDeclVisitor {

  llvm::raw_ostream &Out;

public:
  ABIPrinter(llvm::raw_ostream &Out = llvm::outs()) : Out(Out) {}

  void HandleSourceUnit(ASTContext &Ctx, SourceUnit &S) override {
    S.accept(*this);
  }

  void visit(ContractDeclType &) override;

  llvm::raw_ostream &os() { return Out; }
};

std::unique_ptr<ASTConsumer> CreateABIPrinter(llvm::raw_ostream &Out) {
  return std::make_unique<ABIPrinter>(Out);
}

void ABIPrinter::visit(ContractDeclType &CD) {
  json abi = json::array();
  auto constructor = CD.getConstructor();
  auto fallback = CD.getFallback();

  if (constructor != nullptr) {
    auto payable = GetPayable(constructor->getStateMutability());
    auto stateMutability = ToString(constructor->getStateMutability());
    auto inputs = json::array();
    for (auto param : constructor->getParams()->getParams()) {
      inputs.push_back(
          {{"name", param->getName()}, {"type", param->GetType()->getName()}});
    }
    abi.push_back({{"type", "constructor"},
                   {"stateMutability", stateMutability},
                   {"payable", payable},
                   {"inputs", inputs}});
  }
  if (fallback != nullptr) {
    auto payable = GetPayable(fallback->getStateMutability());
    auto stateMutability = ToString(fallback->getStateMutability());
    abi.push_back({{"type", "fallback"},
                   {"stateMutability", stateMutability},
                   {"payable", payable}});
  }
  for (auto func : CD.getFuncs()) {
    if (func->getVisibility() == Decl::Visibility::Public ||
        func->getVisibility() == Decl::Visibility::External) {
      auto name = func->getName().str();
      auto stateMutability = ToString(func->getStateMutability());
      auto inputs = json::array();
      auto outputs = json::array();
      auto payable = GetPayable(func->getStateMutability());
      auto constant = GetConstant(func->getStateMutability());
      for (auto param : func->getParams()->getParams()) {
        inputs.push_back({{"name", param->getName()},
                          {"type", param->GetType()->getName()}});
      }
      for (auto param : func->getReturnParams()->getParams()) {
        outputs.push_back({{"name", param->getName()},
                           {"type", param->GetType()->getName()}});
      }
      abi.push_back({{"type", "function"},
                     {"name", name},
                     {"inputs", inputs},
                     {"outputs", outputs},
                     {"stateMutability", stateMutability},
                     {"payable", payable},
                     {"constant", constant}});
    }
  }
  for (auto event : CD.getEvents()) {
    auto name = event->getName().str();
    auto inputs = json::array();
    auto anonymous = event->isAnonymous();
    for (auto param : event->getParams()->getParams()) {
      if (auto VD = dynamic_cast<const VarDecl *>(param)) {
        inputs.push_back({{"name", VD->getName()},
                          {"type", VD->GetType()->getName()},
                          {"indexed", VD->isIndexed()}});
      }
    }
    abi.push_back({{"type", "event"},
                   {"name", name},
                   {"inputs", inputs},
                   {"anonymous", anonymous}});
  }

  os() << abi.dump() << "\n";
}

} // namespace soll
