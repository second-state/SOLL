// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/DeclYul.h"

namespace soll {

///
/// YulObject
///
std::vector<YulObject *> YulObject::getObjectList() {
  std::vector<YulObject *> ObjectList;
  for (auto &O : this->ObjectList)
    ObjectList.emplace_back(O.get());
  return ObjectList;
}

std::vector<const YulObject *> YulObject::getObjectList() const {
  std::vector<const YulObject *> ObjectList;
  for (auto &O : this->ObjectList)
    ObjectList.emplace_back(O.get());
  return ObjectList;
}

std::vector<YulData *> YulObject::getDataList() {
  std::vector<YulData *> DataList;
  for (auto &D : this->DataList)
    DataList.emplace_back(D.get());
  return DataList;
}

std::vector<const YulData *> YulObject::getDataList() const {
  std::vector<const YulData *> DataList;
  for (auto &D : this->DataList)
    DataList.emplace_back(D.get());
  return DataList;
}
std::variant<std::monostate, const YulData *, const YulObject *>
YulObject::lookupYulDataOrYulObject(llvm::StringRef Name) const {
  auto Iterator = LookupYulDataOrYulObject.find(Name);
  if (Iterator == LookupYulDataOrYulObject.end()) {
    return std::monostate();
  }
  return std::visit(
      [](const auto *Ptr) {
        return std::variant<std::monostate, const YulData *, const YulObject *>(
            Ptr);
      },
      Iterator->second);
}

} // namespace soll
