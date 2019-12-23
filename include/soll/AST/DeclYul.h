// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include "soll/AST/ASTForward.h"
#include "soll/AST/Decl.h"
#include "soll/AST/ExprAsm.h"

namespace soll {

class YulCode : public Decl {
  std::unique_ptr<Block> Body;

public:
  YulCode(std::unique_ptr<Block> &&Body)
      : Decl(llvm::StringRef::withNullAsEmpty(nullptr)), ///< XXX: refactor
        Body(std::move(Body)) {}

  Block *getBody() { return Body.get(); }
  const Block *getBody() const { return Body.get(); }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class YulData : public Decl {
  std::unique_ptr<StringLiteral> Body;

public:
  YulData(llvm::StringRef Name, std::unique_ptr<StringLiteral> &&Body)
      : Decl(Name), Body(std::move(Body)) {}

  StringLiteral *getBody() { return Body.get(); }
  const StringLiteral *getBody() const { return Body.get(); }

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

class YulObject : public Decl {
  std::unique_ptr<YulCode> Code;
  std::vector<std::unique_ptr<YulObject>> ObjectList;
  std::vector<std::unique_ptr<YulData>> DataList;

public:
  YulObject(llvm::StringRef Name, std::unique_ptr<YulCode> &&Code,
            std::vector<std::unique_ptr<YulObject>> &&ObjectList,
            std::vector<std::unique_ptr<YulData>> &&DataList)
      : Decl(Name), Code(std::move(Code)), ObjectList(std::move(ObjectList)),
        DataList(std::move(DataList)) {}

  YulCode *getCode() { return Code.get(); }
  const YulCode *getCode() const { return Code.get(); }
  std::vector<YulObject *> getObjectList();
  std::vector<const YulObject *> getObjectList() const;
  std::vector<YulData *> getDataList();
  std::vector<const YulData *> getDataList() const;

  void accept(DeclVisitor &visitor) override;
  void accept(ConstDeclVisitor &visitor) const override;
};

} // namespace soll
