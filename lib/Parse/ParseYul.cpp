// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/Parser.h"

using namespace std;

namespace soll {

static bool isObject(const Token &Tok) {
  return Tok.isAnyIdentifier() &&
         Tok.getIdentifierInfo()->getName().equals("object");
}

static bool isData(const Token &Tok) {
  return Tok.isAnyIdentifier() &&
         Tok.getIdentifierInfo()->getName().equals("data");
}

unique_ptr<SourceUnit> Parser::parseYul() {
  std::unique_ptr<SourceUnit> SU;
  vector<unique_ptr<Decl>> Nodes;

  if (Tok.is(tok::l_brace)) {
    auto Body = parseAsmBlock();
    auto Code = make_unique<YulCode>(Body->getLocation(), std::move(Body));
    auto DataList = make_unique_vector<YulData>();
    auto Obj = make_unique<YulObject>(
        Code->getLocation(), "object", std::move(Code),
        make_unique_vector<YulObject>(), std::move(DataList));
    Nodes.push_back(std::move(Obj));
  } else if (isObject(Tok)) {
    Nodes.push_back(parseYulObject());
  } else if (Tok.is(tok::l_brace)) {
    // Special case: Code-only form.
    assert(false && "not support code-only form");
    __builtin_unreachable();
  }
  SU = make_unique<SourceUnit>(
      SourceRange(Nodes.front()->getLocation().getBegin(),
                  Nodes.back()->getLocation().getEnd()),
      std::move(Nodes));
  Actions.setLibrariesAddressMap(&LibrariesAddressMap);
  Actions.resolveIdentifierDecl(*SU);
  Actions.resolveType(*SU);
  Actions.resolveImmutable(*SU);
  Actions.resolveUniqueName(*SU);
  return SU;
}

unique_ptr<YulObject> Parser::parseYulObject() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken();

  auto Name = stringUnquote(std::string(Tok.getLiteralData(), Tok.getLength()));

  ConsumeStringToken();
  ExpectAndConsume(tok::l_brace);

  auto Code = parseYulCode();
  auto ObjectList = make_unique_vector<YulObject>();
  auto DataList = make_unique_vector<YulData>();

  while (Tok.isNot(tok::r_brace)) {
    if (isObject(Tok)) {
      ObjectList.push_back(parseYulObject());
    } else if (isData(Tok)) {
      DataList.push_back(parseYulData());
    } else {
      ConsumeAnyToken();
    }
  }

  ExpectAndConsume(tok::r_brace);
  auto Obj = make_unique<YulObject>(SourceRange(Begin, Tok.getEndLoc()), Name,
                                    std::move(Code), std::move(ObjectList),
                                    std::move(DataList));
  return Obj;
}

unique_ptr<YulCode> Parser::parseYulCode() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken();
  auto Body = parseAsmBlock();
  auto Code = make_unique<YulCode>(SourceRange(Begin, Tok.getEndLoc()),
                                   std::move(Body));
  return Code;
}

unique_ptr<YulData> Parser::parseYulData() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken();
  llvm::StringRef NameRef(Tok.getLiteralData(), Tok.getLength());
  auto Name = Tok.is(tok::string_literal) ? stringUnquote(NameRef)
                                          : hexUnquote(NameRef);
  ConsumeStringToken();
  llvm::StringRef BodyRef(Tok.getLiteralData(), Tok.getLength());
  std::string BodyStr = Tok.is(tok::string_literal) ? stringUnquote(BodyRef)
                                                    : hexUnquote(BodyRef);
  auto Body = make_unique<StringLiteral>(Tok, std::move(BodyStr));
  ConsumeStringToken();
  auto Data = make_unique<YulData>(SourceRange(Begin, Tok.getEndLoc()),
                                   std::move(Name), std::move(Body));
  return Data;
}

} // namespace soll
