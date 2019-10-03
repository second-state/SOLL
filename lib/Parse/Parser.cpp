// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/Parser.h"
#include "soll/AST/AST.h"
#include "soll/Basic/OperatorPrecedence.h"
#include "soll/Lex/Lexer.h"

using namespace std;

namespace {

std::string stringUnquote(const std::string &Quoted) {
  std::string Result;
  assert(Quoted.size() >= 2 && "string token with size < 2!");
  const char *TokBegin = Quoted.data();
  const char *TokEnd = TokBegin + Quoted.size() - 1;
  assert(TokBegin[0] == '"' || TokBegin[0] == '\'');
  assert(TokEnd[0] == TokBegin[0]);
  ++TokBegin;

  const char *TokBuf = TokBegin;
  while (TokBuf != TokEnd) {
    if (TokBuf[0] != '\\') {
      const char *InStart = TokBuf;
      do {
        ++TokBuf;
      } while (TokBuf != TokEnd && TokBuf[0] != '\\');
      Result.append(InStart, TokBuf - InStart);
      continue;
    }
    if (TokBuf[1] == 'u') {
      std::uint_fast8_t UcnLen = 4;
      std::uint32_t UcnVal = 0;
      TokBuf += 2;
      for (; TokBuf != TokEnd && UcnLen; --UcnLen) {
        unsigned CharVal = llvm::hexDigitValue(TokBuf[0]);
        UcnVal <<= 4;
        UcnVal |= CharVal;
      }
      assert(UcnLen == 0 && "Unicode escape incompleted");

      // Convert to UTF8
      std::uint_fast8_t BytesToWrite = 0;
      if (UcnVal < 0x80U) {
        BytesToWrite = 1;
      } else if (UcnVal < 0x800U) {
        BytesToWrite = 2;
      } else if (UcnVal < 0x10000U) {
        BytesToWrite = 3;
      } else {
        BytesToWrite = 4;
      }
      constexpr const unsigned ByteMask = 0xBF;
      constexpr const unsigned ByteMark = 0x80;
      static constexpr const unsigned FirstByteMark[5] = {0x00, 0x00, 0xC0,
                                                          0xE0, 0xF0};
      std::array<char, 4> Buffer;
      char *ResultBuf = &Buffer[5];
      switch (BytesToWrite) {
      case 4:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 3:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 2:
        *--ResultBuf = static_cast<char>((UcnVal | ByteMark) & ByteMask);
        UcnVal >>= 6;
        [[fallthrough]];
      case 1:
        *--ResultBuf = static_cast<char>(UcnVal | FirstByteMark[BytesToWrite]);
      }
      Result.append(ResultBuf, BytesToWrite);
      continue;
    }
    TokBuf += 2;
    char ResultChar = TokBuf[1];
    switch (ResultChar) {
    case 'x': {
      std::uint_fast8_t HexLen = 2;
      std::uint8_t HexVal = 0;
      for (; TokBuf != TokEnd && HexLen; --HexLen) {
        unsigned CharVal = llvm::hexDigitValue(TokBuf[0]);
        HexVal <<= 4;
        HexVal |= CharVal;
      }
      assert(HexLen == 0 && "Hex escape incompleted");
      ResultChar = static_cast<char>(HexVal);
      break;
    }
    case '\\':
    case '\'':
    case '\"':
      break;
    case 'b':
      ResultChar = '\b';
      break;
    case 'f':
      ResultChar = '\f';
      break;
    case 'n':
      ResultChar = '\n';
      break;
    case 'r':
      ResultChar = '\r';
      break;
    case 't':
      ResultChar = '\t';
      break;
    case 'v':
      ResultChar = '\v';
      break;
    default:
      assert(false && "unknown escape sequence!");
      break;
    }
    Result.push_back(ResultChar);
  }
  return Result;
}

std::string hexUnquote(const std::string &Quoted) {
  assert(Quoted.size() % 2 == 0 && "Hex escape incompleted");
  std::string Result;
  for (std::size_t I = 0; I < Quoted.size(); I += 2) {
    Result.push_back((llvm::hexDigitValue(Quoted[I]) << 4) |
                     llvm::hexDigitValue(Quoted[I + 1]));
  }
  return Result;
}

} // namespace

namespace soll {

static BinaryOperatorKind token2bop(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::starstar:
    return BO_Exp;
  case tok::star:
    return BO_Mul;
  case tok::slash:
    return BO_Div;
  case tok::percent:
    return BO_Rem;
  case tok::plus:
    return BO_Add;
  case tok::minus:
    return BO_Sub;
  case tok::lessless:
    return BO_Shl;
  case tok::greatergreater:
    return BO_Shr;
  case tok::amp:
    return BO_And;
  case tok::caret:
    return BO_Xor;
  case tok::pipe:
    return BO_Or;
  case tok::less:
    return BO_LT;
  case tok::greater:
    return BO_GT;
  case tok::lessequal:
    return BO_LE;
  case tok::greaterequal:
    return BO_GE;
  case tok::equalequal:
    return BO_EQ;
  case tok::exclaimequal:
    return BO_NE;
  case tok::ampamp:
    return BO_LAnd;
  case tok::pipepipe:
    return BO_LOr;
  case tok::equal:
    return BO_Assign;
  case tok::starequal:
    return BO_MulAssign;
  case tok::slashequal:
    return BO_DivAssign;
  case tok::percentequal:
    return BO_RemAssign;
  case tok::plusequal:
    return BO_AddAssign;
  case tok::minusequal:
    return BO_SubAssign;
  case tok::lesslessequal:
    return BO_ShlAssign;
  case tok::greatergreaterequal:
    return BO_ShrAssign;
  case tok::ampequal:
    return BO_AndAssign;
  case tok::caretequal:
    return BO_XorAssign;
  case tok::pipeequal:
    return BO_OrAssign;
  case tok::comma:
    return BO_Comma;
  default:
    return BO_Undefined;
  }
}

static UnaryOperatorKind token2uop(llvm::Optional<Token> Tok,
                                   bool IsPreOp = true) {
  switch (Tok->getKind()) {
  case tok::plusplus:
    if (IsPreOp)
      return UO_PreInc;
    else
      return UO_PostInc;
  case tok::minusminus:
    if (IsPreOp)
      return UO_PreDec;
    else
      return UO_PostDec;
  case tok::amp:
    return UO_AddrOf;
  case tok::star:
    return UO_Deref;
  case tok::plus:
    return UO_Plus;
  case tok::minus:
    return UO_Minus;
  case tok::tilde:
    return UO_Not;
  case tok::exclaim:
    return UO_LNot;
  default:
    return UO_Undefined;
  }
}

static IntegerType::IntKind token2inttype(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::kw_uint8:
    return IntegerType::IntKind::U8;
  case tok::kw_uint16:
    return IntegerType::IntKind::U16;
  case tok::kw_uint24:
    return IntegerType::IntKind::U24;
  case tok::kw_uint32:
    return IntegerType::IntKind::U32;
  case tok::kw_uint40:
    return IntegerType::IntKind::U40;
  case tok::kw_uint48:
    return IntegerType::IntKind::U48;
  case tok::kw_uint56:
    return IntegerType::IntKind::U56;
  case tok::kw_uint64:
    return IntegerType::IntKind::U64;
  case tok::kw_uint72:
    return IntegerType::IntKind::U72;
  case tok::kw_uint80:
    return IntegerType::IntKind::U80;
  case tok::kw_uint88:
    return IntegerType::IntKind::U88;
  case tok::kw_uint96:
    return IntegerType::IntKind::U96;
  case tok::kw_uint104:
    return IntegerType::IntKind::U104;
  case tok::kw_uint112:
    return IntegerType::IntKind::U112;
  case tok::kw_uint120:
    return IntegerType::IntKind::U120;
  case tok::kw_uint128:
    return IntegerType::IntKind::U128;
  case tok::kw_uint136:
    return IntegerType::IntKind::U136;
  case tok::kw_uint144:
    return IntegerType::IntKind::U144;
  case tok::kw_uint152:
    return IntegerType::IntKind::U152;
  case tok::kw_uint160:
    return IntegerType::IntKind::U160;
  case tok::kw_uint168:
    return IntegerType::IntKind::U168;
  case tok::kw_uint176:
    return IntegerType::IntKind::U176;
  case tok::kw_uint184:
    return IntegerType::IntKind::U184;
  case tok::kw_uint192:
    return IntegerType::IntKind::U192;
  case tok::kw_uint200:
    return IntegerType::IntKind::U200;
  case tok::kw_uint208:
    return IntegerType::IntKind::U208;
  case tok::kw_uint216:
    return IntegerType::IntKind::U216;
  case tok::kw_uint224:
    return IntegerType::IntKind::U224;
  case tok::kw_uint232:
    return IntegerType::IntKind::U232;
  case tok::kw_uint240:
    return IntegerType::IntKind::U240;
  case tok::kw_uint248:
    return IntegerType::IntKind::U248;
  case tok::kw_uint256:
    return IntegerType::IntKind::U256;
  case tok::kw_uint:
    return IntegerType::IntKind::U256;
  case tok::kw_int8:
    return IntegerType::IntKind::I8;
  case tok::kw_int16:
    return IntegerType::IntKind::I16;
  case tok::kw_int24:
    return IntegerType::IntKind::I24;
  case tok::kw_int32:
    return IntegerType::IntKind::I32;
  case tok::kw_int40:
    return IntegerType::IntKind::I40;
  case tok::kw_int48:
    return IntegerType::IntKind::I48;
  case tok::kw_int56:
    return IntegerType::IntKind::I56;
  case tok::kw_int64:
    return IntegerType::IntKind::I64;
  case tok::kw_int72:
    return IntegerType::IntKind::I72;
  case tok::kw_int80:
    return IntegerType::IntKind::I80;
  case tok::kw_int88:
    return IntegerType::IntKind::I88;
  case tok::kw_int96:
    return IntegerType::IntKind::I96;
  case tok::kw_int104:
    return IntegerType::IntKind::I104;
  case tok::kw_int112:
    return IntegerType::IntKind::I112;
  case tok::kw_int120:
    return IntegerType::IntKind::I120;
  case tok::kw_int128:
    return IntegerType::IntKind::I128;
  case tok::kw_int136:
    return IntegerType::IntKind::I136;
  case tok::kw_int144:
    return IntegerType::IntKind::I144;
  case tok::kw_int152:
    return IntegerType::IntKind::I152;
  case tok::kw_int160:
    return IntegerType::IntKind::I160;
  case tok::kw_int168:
    return IntegerType::IntKind::I168;
  case tok::kw_int176:
    return IntegerType::IntKind::I176;
  case tok::kw_int184:
    return IntegerType::IntKind::I184;
  case tok::kw_int192:
    return IntegerType::IntKind::I192;
  case tok::kw_int200:
    return IntegerType::IntKind::I200;
  case tok::kw_int208:
    return IntegerType::IntKind::I208;
  case tok::kw_int216:
    return IntegerType::IntKind::I216;
  case tok::kw_int224:
    return IntegerType::IntKind::I224;
  case tok::kw_int232:
    return IntegerType::IntKind::I232;
  case tok::kw_int240:
    return IntegerType::IntKind::I240;
  case tok::kw_int248:
    return IntegerType::IntKind::I248;
  case tok::kw_int256:
    return IntegerType::IntKind::I256;
  case tok::kw_int:
    return IntegerType::IntKind::I256;
  default:
    assert(false && "Invalid int token.");
  }
  LLVM_BUILTIN_UNREACHABLE;
}

static FixedBytesType::ByteKind token2bytetype(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::kw_bytes1:
    return FixedBytesType::ByteKind::B1;
  case tok::kw_bytes2:
    return FixedBytesType::ByteKind::B2;
  case tok::kw_bytes3:
    return FixedBytesType::ByteKind::B3;
  case tok::kw_bytes4:
    return FixedBytesType::ByteKind::B4;
  case tok::kw_bytes5:
    return FixedBytesType::ByteKind::B5;
  case tok::kw_bytes6:
    return FixedBytesType::ByteKind::B6;
  case tok::kw_bytes7:
    return FixedBytesType::ByteKind::B7;
  case tok::kw_bytes8:
    return FixedBytesType::ByteKind::B8;
  case tok::kw_bytes9:
    return FixedBytesType::ByteKind::B9;
  case tok::kw_bytes10:
    return FixedBytesType::ByteKind::B10;
  case tok::kw_bytes11:
    return FixedBytesType::ByteKind::B11;
  case tok::kw_bytes12:
    return FixedBytesType::ByteKind::B12;
  case tok::kw_bytes13:
    return FixedBytesType::ByteKind::B13;
  case tok::kw_bytes14:
    return FixedBytesType::ByteKind::B14;
  case tok::kw_bytes15:
    return FixedBytesType::ByteKind::B15;
  case tok::kw_bytes16:
    return FixedBytesType::ByteKind::B16;
  case tok::kw_bytes17:
    return FixedBytesType::ByteKind::B17;
  case tok::kw_bytes18:
    return FixedBytesType::ByteKind::B18;
  case tok::kw_bytes19:
    return FixedBytesType::ByteKind::B19;
  case tok::kw_bytes20:
    return FixedBytesType::ByteKind::B20;
  case tok::kw_bytes21:
    return FixedBytesType::ByteKind::B21;
  case tok::kw_bytes22:
    return FixedBytesType::ByteKind::B22;
  case tok::kw_bytes23:
    return FixedBytesType::ByteKind::B23;
  case tok::kw_bytes24:
    return FixedBytesType::ByteKind::B24;
  case tok::kw_bytes25:
    return FixedBytesType::ByteKind::B25;
  case tok::kw_bytes26:
    return FixedBytesType::ByteKind::B26;
  case tok::kw_bytes27:
    return FixedBytesType::ByteKind::B27;
  case tok::kw_bytes28:
    return FixedBytesType::ByteKind::B28;
  case tok::kw_bytes29:
    return FixedBytesType::ByteKind::B29;
  case tok::kw_bytes30:
    return FixedBytesType::ByteKind::B30;
  case tok::kw_bytes31:
    return FixedBytesType::ByteKind::B31;
  case tok::kw_bytes32:
    return FixedBytesType::ByteKind::B32;
  default:
    assert(false && "Invalid int token.");
  }
  LLVM_BUILTIN_UNREACHABLE;
}

static DataLocation getLoc(llvm::Optional<Token> Tok) {
  switch (Tok->getKind()) {
  case tok::kw_storage:
    return DataLocation::Storage;
  case tok::kw_memory:
    return DataLocation::Memory;
  case tok::kw_calldata:
    return DataLocation::CallData;
  default:
    return DataLocation::Storage;
  }
}

Parser::Parser(Lexer &lexer, Sema &sema) : TheLexer(lexer), Actions(sema) {}

unique_ptr<SourceUnit> Parser::parse() {
  ParseScope SourceUnitScope{this, 0};
  llvm::Optional<Token> CurTok;
  vector<unique_ptr<Decl>> Nodes;

  while ((CurTok = TheLexer.LookAhead(0))->isNot(tok::eof)) {
    switch (CurTok->getKind()) {
    case tok::kw_pragma:
      Nodes.push_back(parsePragmaDirective());
      break;
    case tok::kw_import:
      TheLexer.CachedLex();
      break;
    case tok::kw_interface:
    case tok::kw_library:
    case tok::kw_contract: {
      Nodes.push_back(parseContractDefinition());
      break;
    }
    default:
      TheLexer.CachedLex();
      break;
    }
  }
  return make_unique<SourceUnit>(std::move(Nodes));
}

unique_ptr<PragmaDirective> Parser::parsePragmaDirective() {
  // pragma anything* ;
  // Currently supported:
  // pragma solidity ^0.4.0 || ^0.3.0;
  vector<string> Literals;
  vector<llvm::Optional<Token>> Tokens;
  TheLexer.CachedLex();
  do {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::unknown) {
      assert(false && "Solidity Error: Token incompatible with Solidity parser "
                      "as part of pragma directive.");
    } else if (tok::getPunctuatorSpelling(Kind) != nullptr) {
      TheLexer.CachedLex();
    } else {
      assert((Kind == tok::identifier || tok::isLiteral(Kind)) &&
             "except literal");
      string literal = getLiteralAndAdvance(TheLexer.LookAhead(0)).str();
      Literals.push_back(literal);
      Tokens.push_back(TheLexer.LookAhead(0));
    }
  } while (!TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::eof));
  TheLexer.CachedLex();

  // TODO: Implement version recognize and compare. ref: parsePragmaVersion
  return std::make_unique<PragmaDirective>();
}

ContractDecl::ContractKind Parser::parseContractKind() {
  ContractDecl::ContractKind Kind;
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_interface:
    Kind = ContractDecl::ContractKind::Interface;
    break;
  case tok::kw_contract:
    Kind = ContractDecl::ContractKind::Contract;
    break;
  case tok::kw_library:
    Kind = ContractDecl::ContractKind::Library;
    break;
  default:
    assert(false && "Invalid contract kind.");
  }
  TheLexer.CachedLex();
  return Kind;
}

Decl::Visibility Parser::parseVisibilitySpecifier() {
  Decl::Visibility Vsblty(Decl::Visibility::Default);
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_public:
    Vsblty = Decl::Visibility::Public;
    break;
  case tok::kw_internal:
    Vsblty = Decl::Visibility::Internal;
    break;
  case tok::kw_private:
    Vsblty = Decl::Visibility::Private;
    break;
  case tok::kw_external:
    Vsblty = Decl::Visibility::External;
    break;
  default:
    assert(false && "Invalid visibility specifier.");
  }
  TheLexer.CachedLex();
  return Vsblty;
}

StateMutability Parser::parseStateMutability() {
  StateMutability stateMutability(StateMutability::NonPayable);
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_payable:
    stateMutability = StateMutability::Payable;
    break;
  case tok::kw_view:
    stateMutability = StateMutability::View;
    break;
  case tok::kw_pure:
    stateMutability = StateMutability::Pure;
    break;
  case tok::kw_constant:
    stateMutability = StateMutability::View;
    assert(false && "The state mutability modifier \"constant\" was removed in "
                    "version 0.5.0. "
                    "Use \"view\" or \"pure\" instead.");
    break;
  default:
    assert(false && "Invalid state mutability specifier.");
  }
  TheLexer.CachedLex();
  return stateMutability;
}

unique_ptr<ContractDecl> Parser::parseContractDefinition() {
  ParseScope ContractScope{this, 0};
  ContractDecl::ContractKind CtKind = parseContractKind();
  llvm::StringRef Name;
  vector<unique_ptr<InheritanceSpecifier>> BaseContracts;
  vector<unique_ptr<Decl>> SubNodes;
  unique_ptr<FunctionDecl> Constructor;
  unique_ptr<FunctionDecl> Fallback;
  Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();

  if (TheLexer.LookAhead(0)->is(tok::kw_is)) {
    do {
      TheLexer.CachedLex();
      TheLexer.CachedLex();
      // TODO: Update vector<InheritanceSpecifier> baseContracts
      BaseContracts.push_back(std::move(std::make_unique<InheritanceSpecifier>(
          TheLexer.LookAhead(0)->getIdentifierInfo()->getName().str(),
          vector<std::unique_ptr<Expr>>())));
    } while ((TheLexer.LookAhead(0))->is(tok::comma));
  }

  TheLexer.CachedLex(); // (
  while (true) {
    tok::TokenKind Kind = TheLexer.LookAhead(0)->getKind();
    if (Kind == tok::r_brace) {
      break;
    }
    // TODO: < Parse all Types in contract's context >
    if (Kind == tok::kw_function || Kind == tok::kw_constructor) {
      auto FD = parseFunctionDefinitionOrFunctionTypeStateVariable();
      if (FD) {
        Actions.addDecl(FD.get());
        if (FD->isConstructor()) {
          assert(!Constructor && "multiple constructor defined!");
          Constructor = std::move(FD);
        } else if (FD->isFallback()) {
          assert(!Fallback && "multiple fallback defined!");
          Fallback = std::move(FD);
        } else {
          SubNodes.push_back(std::move(FD));
        }
      }
      Actions.EraseFunRtnTys();
    } else if (Kind == tok::kw_struct) {
      // TODO: contract tok::kw_struct
      assert(false && "struct not implemented");
    } else if (Kind == tok::kw_enum) {
      // TODO: contract tok::kw_enum
      assert(false && "enum not implemented");
    } else if (Kind == tok::identifier || Kind == tok::kw_mapping ||
               TheLexer.LookAhead(0)->isElementaryTypeName()) {
      VarDeclParserOptions options;
      options.IsStateVariable = true;
      options.AllowInitialValue = true;
      SubNodes.push_back(parseVariableDeclaration(options));
      TheLexer.CachedLex(); // ;
    } else if (Kind == tok::kw_modifier) {
      // TODO: contract tok::kw_modifier
      assert(false && "modifier not implemented");
    } else if (Kind == tok::kw_event) {
      SubNodes.push_back(parseEventDefinition());
    } else if (Kind == tok::kw_using) {
      // TODO: contract tok::kw_using
      assert(false && "using not implemented");
    } else {
      assert(false && "Solidity Error: Function, variable, struct or modifier "
                      "declaration expected.");
    }
  }
  auto CD = std::make_unique<ContractDecl>(
      Name, std::move(BaseContracts), std::move(SubNodes),
      std::move(Constructor), std::move(Fallback), CtKind);
  Actions.addDecl(CD.get());
  return CD;
}

Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool ForceEmptyName, bool AllowModifiers) {

  FunctionHeaderParserResult Result;

  Result.IsConstructor = false;
  Result.IsFallback = false;

  if (TheLexer.CachedLex()->is(tok::kw_constructor)) {
    Result.IsConstructor = true;
  }

  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  if (Result.IsConstructor) {
    Result.Name = llvm::StringRef("solidity.constructor");
  } else if (ForceEmptyName || CurTok->is(tok::l_paren)) {
    Result.Name = llvm::StringRef("solidity.fallback");
    Result.IsFallback = true;
  } else {
    Result.Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();
  }

  VarDeclParserOptions Options;
  Options.AllowLocationSpecifier = true;

  Result.Parameters = parseParameterList(Options);

  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (AllowModifiers && CurTok->is(tok::identifier)) {
      // TODO: Function Modifier
    } else if (CurTok->isOneOf(tok::kw_public, tok::kw_private,
                               tok::kw_internal, tok::kw_external)) {
      // TODO: Special case of a public state variable of function Type.
      Result.Vsblty = parseVisibilitySpecifier();
    } else if (CurTok->isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                               tok::kw_payable)) {
      Result.SM = parseStateMutability();
    } else {
      break;
    }
  }

  if (TheLexer.LookAhead(0)->is(tok::kw_returns)) {
    bool const PermitEmptyParameterList = false;
    TheLexer.CachedLex();
    Result.ReturnParameters =
        parseParameterList(Options, PermitEmptyParameterList);
    vector<TypePtr> Tys;
    for (auto &&Return : Result.ReturnParameters->getParams())
      Tys.push_back(Return->GetType());
    Actions.SetFunRtnTys(Tys);
  } else {
    Result.ReturnParameters =
        make_unique<ParamList>(std::vector<std::unique_ptr<VarDecl>>());
  }

  return Result;
}

unique_ptr<FunctionDecl>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  ParseScope ArgumentScope{this, 0};
  FunctionHeaderParserResult Header = parseFunctionHeader(false, true);
  if (Header.IsConstructor || !Header.Modifiers.empty() ||
      !Header.Name.empty() ||
      TheLexer.LookAhead(0)->isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function
    unique_ptr<Block> block;
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      ParseScope FunctionScope{this, Scope::FunctionScope};
      // TODO: cache token and delay parsing after contract defined
      block = parseBlock();
    }
    return Actions.CreateFunctionDecl(
        Header.Name, Header.Vsblty, Header.SM, Header.IsConstructor,
        Header.IsFallback, std::move(Header.Parameters),
        std::move(Header.Modifiers), std::move(Header.ReturnParameters),
        std::move(block));
  } else {
    // TODO: State Variable case.
    return nullptr;
  }
}

unique_ptr<VarDecl>
Parser::parseVariableDeclaration(VarDeclParserOptions const &Options,
                                 TypePtr &&LookAheadArrayType) {
  TypePtr T;
  if (LookAheadArrayType) {
    T = std::move(LookAheadArrayType);
  } else {
    T = parseTypeName(Options.AllowVar);
  }

  bool IsIndexed = false;
  bool IsDeclaredConst = false;
  Decl::Visibility Vsblty = Decl::Visibility::Default;
  VarDecl::Location Loc = VarDecl::Location::Unspecified;
  llvm::StringRef Name;
  llvm::Optional<Token> CurTok;
  while (true) {
    CurTok = TheLexer.LookAhead(0);
    if (Options.IsStateVariable &&
        CurTok->isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal)) {
      Vsblty = parseVisibilitySpecifier();
    } else {
      if (Options.AllowIndexed && CurTok->is(tok::kw_indexed)) {
        IsIndexed = true;
      } else if (CurTok->is(tok::kw_constant)) {
        IsDeclaredConst = true;
      } else if (Options.AllowLocationSpecifier &&
                 CurTok->isOneOf(tok::kw_memory, tok::kw_storage,
                                 tok::kw_calldata)) {
        if (Loc != VarDecl::Location::Unspecified)
          assert(false && "Location already specified.");
        else if (!T)
          assert(false && "Location specifier needs explicit type name.");
        else {
          switch (TheLexer.LookAhead(0)->getKind()) {
          case tok::kw_storage:
            Loc = VarDecl::Location::Storage;
            break;
          case tok::kw_memory:
            Loc = VarDecl::Location::Memory;
            break;
          case tok::kw_calldata:
            Loc = VarDecl::Location::CallData;
            break;
          default:
            assert(false && "Unknown data location.");
          }
        }
      } else
        break;
      TheLexer.CachedLex();
    }
  }

  if (Options.AllowEmptyName && TheLexer.LookAhead(0)->isNot(tok::identifier)) {
    Name = llvm::StringRef("");
  } else {
    Name = TheLexer.CachedLex()->getIdentifierInfo()->getName();
  }

  unique_ptr<Expr> Value;
  if (Options.AllowInitialValue) {
    if (TheLexer.LookAhead(0)->is(tok::equal)) {
      TheLexer.CachedLex();
      Value = parseExpression();
    }
  }

  auto VD = std::make_unique<VarDecl>(std::move(T), Name, std::move(Value),
                                      Vsblty, Options.IsStateVariable,
                                      IsIndexed, IsDeclaredConst, Loc);

  Actions.addDecl(VD.get());
  return VD;
}

unique_ptr<EventDecl> Parser::parseEventDefinition() {
  TheLexer.CachedLex(); // event
  std::string Name = getLiteralAndAdvance(TheLexer.LookAhead(0)).str();
  VarDeclParserOptions Options;
  Options.AllowIndexed = true;
  std::unique_ptr<ParamList> Parameters = parseParameterList(Options);
  bool Anonymous = false;
  if (TheLexer.CachedLex()->is(tok::kw_anonymous)) {
    Anonymous = true;
    TheLexer.CachedLex(); // ;
  }
  auto ED = Actions.CreateEventDecl(Name, std::move(Parameters), Anonymous);
  Actions.addDecl(ED.get());
  return ED;
}

TypePtr Parser::parseTypeNameSuffix(TypePtr T) {
  while (TheLexer.LookAhead(0)->is(tok::l_square)) {
    TheLexer.CachedLex(); // [
    if (TheLexer.LookAhead(0)->isNot(tok::r_square)) {
      int NumValue;
      getLiteralAndAdvance(TheLexer.LookAhead(0)).getAsInteger(0, NumValue);
      T = make_shared<ArrayType>(std::move(T), NumValue,
                                 getLoc(TheLexer.LookAhead(0)));
    } else {
      T = make_shared<ArrayType>(std::move(T), getLoc(TheLexer.LookAhead(0)));
    }
    TheLexer.CachedLex(); // ]
  }
  return T;
}

// TODO: < Need complete all Types >
TypePtr Parser::parseTypeName(bool AllowVar) {
  TypePtr T;
  bool HaveType = false;
  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  tok::TokenKind Kind = CurTok->getKind();
  if (CurTok->isElementaryTypeName()) {
    if (CurTok->getKind() == tok::kw_bool) {
      T = std::make_shared<BooleanType>();
      TheLexer.CachedLex();
    } else if (tok::kw_int <= CurTok->getKind() &&
               CurTok->getKind() <= tok::kw_uint256) {
      T = std::make_shared<IntegerType>(token2inttype(CurTok));
      TheLexer.CachedLex();
    } else if (tok::kw_bytes1 <= CurTok->getKind() &&
               CurTok->getKind() <= tok::kw_bytes32) {
      T = std::make_shared<FixedBytesType>(token2bytetype(CurTok));
      TheLexer.CachedLex();
    } else if (CurTok->getKind() == tok::kw_bytes) {
      T = std::make_shared<BytesType>();
      TheLexer.CachedLex();
    } else if (CurTok->getKind() == tok::kw_string) {
      T = std::make_shared<StringType>();
      TheLexer.CachedLex();
    } else if (CurTok->getKind() == tok::kw_address) {
      TheLexer.CachedLex();
      StateMutability SM = StateMutability::NonPayable;
      if (TheLexer.LookAhead(1)->isOneOf(tok::kw_constant, tok::kw_pure,
                                         tok::kw_view, tok::kw_payable)) {
        SM = parseStateMutability();
      }
      T = std::make_shared<AddressType>();
    }
    HaveType = true;
  } else if (Kind == tok::kw_var) {
    // TODO: parseTypeName tok::kw_var (var is deprecated)
    assert(false && "Expected Type Name");
  } else if (Kind == tok::kw_function) {
    // TODO: parseTypeName tok::kw_function
  } else if (Kind == tok::kw_mapping) {
    T = std::move(parseMapping());
  } else if (Kind == tok::identifier) {
    // TODO: parseTypeName tok::identifier
  } else {
    assert(false && "Expected Type Name");
  }

  if (T || HaveType) {
    T = parseTypeNameSuffix(move(T));
  }
  return T;
}

shared_ptr<MappingType> Parser::parseMapping() {
  TheLexer.CachedLex(); // mapping
  TheLexer.CachedLex(); // (
  bool const AllowVar = false;
  TypePtr KeyType;
  if (TheLexer.LookAhead(0)->isElementaryTypeName()) {
    KeyType = parseTypeName(AllowVar);
  }
  TheLexer.CachedLex(); // =>
  TypePtr ValueType = parseTypeName(AllowVar);
  TheLexer.CachedLex(); // )
  return std::make_shared<MappingType>(std::move(KeyType),
                                       std::move(ValueType));
}

unique_ptr<ParamList>
Parser::parseParameterList(VarDeclParserOptions const &_Options,
                           bool AllowEmpty) {
  vector<unique_ptr<VarDecl>> Parameters;
  VarDeclParserOptions Options(_Options);
  Options.AllowEmptyName = true;
  TheLexer.CachedLex(); // (
  if (!AllowEmpty || TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
    Parameters.push_back(parseVariableDeclaration(Options));
    while (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
      TheLexer.CachedLex(); // ,
      Parameters.push_back(parseVariableDeclaration(Options));
    }
  }
  TheLexer.CachedLex(); // )
  return std::make_unique<ParamList>(std::move(Parameters));
}

unique_ptr<Block> Parser::parseBlock() {
  ParseScope BlockScope{this, 0};
  vector<unique_ptr<Stmt>> Statements;
  TheLexer.CachedLex();
  while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
    Statements.push_back(parseStatement());
  }
  TheLexer.CachedLex();
  return std::make_unique<Block>(std::move(Statements));
}

// TODO: < Parse all statements >
unique_ptr<Stmt> Parser::parseStatement() {
  unique_ptr<Stmt> Statement;
  llvm::Optional<Token> CurTok;
  switch (TheLexer.LookAhead(0)->getKind()) {
  case tok::kw_if:
    return parseIfStatement();
  case tok::kw_while:
    return parseWhileStatement();
  case tok::kw_do:
    return parseDoWhileStatement();
    break;
  case tok::kw_for:
    return parseForStatement();
  case tok::l_brace:
    return parseBlock();
  case tok::kw_continue:
    Statement = std::make_unique<ContinueStmt>();
    TheLexer.CachedLex();
    break;
  case tok::kw_break:
    Statement = std::make_unique<BreakStmt>();
    TheLexer.CachedLex();
    break;
  case tok::kw_return:
    TheLexer.CachedLex();
    if (TheLexer.LookAhead(0)->isNot(tok::semi)) {
      Statement = Actions.CreateReturnStmt(std::move(parseExpression()));
    }
    break;
  case tok::kw_assembly:
    // TODO: parseStatement kw_assembly
    break;
  case tok::kw_emit:
    Statement = parseEmitStatement();
    break;
  case tok::identifier:
  default:
    Statement = parseSimpleStatement();
    break;
  }
  TheLexer.CachedLex(); // ;
  return Statement;
}

unique_ptr<IfStmt> Parser::parseIfStatement() {
  TheLexer.CachedLex(); // if
  TheLexer.CachedLex(); // (
  unique_ptr<Expr> Condition = parseExpression();
  TheLexer.CachedLex(); // )
  unique_ptr<Stmt> TrueBody = parseStatement();
  unique_ptr<Stmt> FalseBody;
  if (TheLexer.LookAhead(0)->is(tok::kw_else)) {
    TheLexer.CachedLex();
    FalseBody = parseStatement();
  }
  return std::make_unique<IfStmt>(std::move(Condition), std::move(TrueBody),
                                  std::move(FalseBody));
}

unique_ptr<WhileStmt> Parser::parseWhileStatement() {
  TheLexer.CachedLex(); // while
  TheLexer.CachedLex(); // (
  unique_ptr<Expr> Condition = parseExpression();
  TheLexer.CachedLex(); // )
  unique_ptr<Stmt> Body;
  {
    ParseScope WhileScope{this, Scope::BreakScope | Scope::ContinueScope };
    Body = parseStatement();
  }
  return std::make_unique<WhileStmt>(std::move(Condition), std::move(Body),
                                     false);
}

unique_ptr<WhileStmt> Parser::parseDoWhileStatement() {
  TheLexer.CachedLex(); // do
  unique_ptr<Stmt> Body;
  {
    ParseScope DoWhileScope{this, Scope::BreakScope | Scope::ContinueScope };
    Body = parseStatement();
  }
  TheLexer.CachedLex(); // while
  TheLexer.CachedLex(); // (
  unique_ptr<Expr> Condition = parseExpression();
  TheLexer.CachedLex(); // )
  TheLexer.CachedLex(); // ;
  return std::make_unique<WhileStmt>(std::move(Condition), std::move(Body),
                                     true);
}

unique_ptr<ForStmt> Parser::parseForStatement() {
  unique_ptr<Stmt> Init;
  unique_ptr<Expr> Condition;
  unique_ptr<Expr> Loop;
  TheLexer.CachedLex(); // for
  TheLexer.CachedLex(); // (

  // LTODO: Maybe here have some predicate like peekExpression() instead of
  // checking for semicolon and RParen?
  if (TheLexer.LookAhead(0)->isNot(tok::semi))
    Init = parseSimpleStatement();
  TheLexer.CachedLex(); // ;

  if (TheLexer.LookAhead(0)->isNot(tok::semi))
    Condition = parseExpression();
  TheLexer.CachedLex(); // ;

  if (TheLexer.LookAhead(0)->isNot(tok::r_paren))
    Loop = parseExpression();
  TheLexer.CachedLex(); // )

  unique_ptr<Stmt> Body;
  {
    ParseScope ForScope{this, Scope::BreakScope | Scope::ContinueScope };
    Body = parseStatement();
  }
  return std::make_unique<ForStmt>(std::move(Init), std::move(Condition),
                                   std::move(Loop), std::move(Body));
}

unique_ptr<EmitStmt> Parser::parseEmitStatement() {
  TheLexer.CachedLex(); // emit

  if (TheLexer.LookAhead(0)->isNot(tok::identifier)) {
    assert(false && "Expected event name or path.");
  }

  IndexAccessedPath Iap;
  while (true) {
    Iap.Path.push_back(Actions.CreateIdentifier(
        getLiteralAndAdvance(TheLexer.LookAhead(0)).str()));
    if (TheLexer.LookAhead(0)->isNot(tok::period))
      break;
    TheLexer.CachedLex(); // .
  };

  auto EventName = expressionFromIndexAccessStructure(Iap);

  TheLexer.CachedLex(); // (
  vector<unique_ptr<Expr>> Arguments;
  vector<llvm::StringRef> Names;
  tie(Arguments, Names) = parseFunctionCallArguments();
  TheLexer.CachedLex(); // )
  unique_ptr<CallExpr> Call =
      Actions.CreateCallExpr(std::move(EventName), std::move(Arguments));
  return make_unique<EmitStmt>(std::move(Call));
}

unique_ptr<Stmt> Parser::parseSimpleStatement() {
  llvm::Optional<Token> CurTok;
  LookAheadInfo StatementType;
  IndexAccessedPath Iap;
  unique_ptr<Expr> Expression;

  bool IsParenExpr = false;
  if (TheLexer.LookAhead(0)->is(tok::l_paren)) {
    IsParenExpr = true;
    TheLexer.CachedLex(); // (
  }

  tie(StatementType, Iap) = tryParseIndexAccessedPath();
  switch (StatementType) {
  case LookAheadInfo::VariableDeclaration:
    return parseVariableDeclarationStatement(
        typeNameFromIndexAccessStructure(Iap));
  case LookAheadInfo::Expression:
    Expression =
        parseExpression(expressionFromIndexAccessStructure(Iap));
    break;
  default:
    assert(false && "Unhandle statement.");
  }
  if (IsParenExpr) {
    TheLexer.CachedLex(); // )
    return parseExpression(std::make_unique<ParenExpr>(std::move(Expression)));
  }
  return Expression;
}

unique_ptr<DeclStmt>
Parser::parseVariableDeclarationStatement(TypePtr &&LookAheadArrayType) {
  // This does not parse multi variable declaration statements starting directly
  // with
  // `(`, they are parsed in parseSimpleStatement, because they are hard to
  // distinguish from tuple expressions.
  vector<unique_ptr<VarDecl>> Variables;
  unique_ptr<Expr> Value;
  if (!LookAheadArrayType && TheLexer.LookAhead(0)->is(tok::kw_var) &&
      TheLexer.LookAhead(0)->is(tok::l_paren)) {
    // [0.4.20] The var keyword has been deprecated for security reasons.
    // https://github.com/ethereum/solidity/releases/tag/v0.4.20
    assert(false &&
           "The var keyword has been deprecated for security reasons.");
  } else {
    VarDeclParserOptions Options;
    Options.AllowVar = false;
    Options.AllowLocationSpecifier = true;
    Variables.push_back(
        parseVariableDeclaration(Options, std::move(LookAheadArrayType)));
  }
  if (TheLexer.LookAhead(0)->is(tok::equal)) {
    TheLexer.CachedLex();
    Value = parseExpression();
  }

  return std::make_unique<DeclStmt>(std::move(Variables), std::move(Value));
}

bool Parser::IndexAccessedPath::empty() const {
  if (!Indices.empty()) {
    assert(!(Path.empty() && ElementaryType) && "");
  }
  return Path.empty() && (ElementaryType == nullptr) && Indices.empty();
}

pair<Parser::LookAheadInfo, Parser::IndexAccessedPath>
Parser::tryParseIndexAccessedPath() {
  // These two cases are very hard to distinguish:
  // x[7 * 20 + 3] a;     and     x[7 * 20 + 3] = 9;
  // In the first case, x is a type name, in the second it is the name of a
  // variable. As an extension, we can even have: `x.y.z[1][2] a;` and
  // `x.y.z[1][2] = 10;` Where in the first, x.y.z leads to a type name where in
  // the second, it accesses structs.

  auto StatementType = peekStatementType();

  switch (StatementType) {
  case LookAheadInfo::VariableDeclaration:
  case LookAheadInfo::Expression:
    return make_pair(StatementType, IndexAccessedPath());
  default:
    break;
  }
  // At this point, we have 'Identifier "["' or 'Identifier "." Identifier' or
  // 'ElementoryTypeName "["'. We parse '(Identifier ("." Identifier)*
  // |ElementaryTypeName) ( "[" Expression "]" )*' until we can decide whether
  // to hand this over to ExpressionStatement or create a
  // VariableDeclarationStatement out of it.
  IndexAccessedPath Iap = parseIndexAccessedPath();

  // if (m_scanner->currentToken() == Token::Identifier ||
  // TokenTraits::isLocationSpecifier(m_scanner->currentToken()))
  if (TheLexer.LookAhead(0)->isOneOf(tok::identifier, tok::kw_memory,
                                     tok::kw_storage, tok::kw_calldata))
    return make_pair(LookAheadInfo::VariableDeclaration, move(Iap));
  else
    return make_pair(LookAheadInfo::Expression, move(Iap));
}

Parser::LookAheadInfo Parser::peekStatementType() const {
  // Distinguish between variable declaration (and potentially assignment) and
  // expression statement (which include assignments to other expressions and
  // pre-declared variables). We have a variable declaration if we get a keyword
  // that specifies a type name. If it is an identifier or an elementary type
  // name followed by an identifier or a mutability specifier, we also have a
  // variable declaration. If we get an identifier followed by a "[" or ".", it
  // can be both ("lib.type[9] a;" or "variable.el[9] = 7;"). In all other
  // cases, we have an expression statement.
  llvm::Optional<Token> CurTok, NextTok;
  CurTok = TheLexer.LookAhead(0);
  bool MightBeTypeName =
      CurTok->isElementaryTypeName() || CurTok->is(tok::identifier);
  if (CurTok->isOneOf(tok::kw_mapping, tok::kw_function, tok::kw_var))
    return LookAheadInfo::VariableDeclaration;

  if (MightBeTypeName) {
    NextTok = TheLexer.LookAhead(1);
    // So far we only allow ``address payable`` in variable declaration
    // statements and in no other kind of statement. This means, for example,
    // that we do not allow type expressions of the form
    // ``address payable;``.
    // If we want to change this in the future, we need to consider another
    // scanner token here.
    if (CurTok->isElementaryTypeName() &&
        NextTok->isOneOf(tok::kw_pure, tok::kw_view, tok::kw_payable)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok->isOneOf(tok::identifier, tok::kw_memory, tok::kw_storage,
                         tok::kw_calldata)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok->isOneOf(tok::l_square, tok::period)) {
      return LookAheadInfo::IndexAccessStructure;
    }
  }
  return LookAheadInfo::Expression;
}

Parser::IndexAccessedPath Parser::parseIndexAccessedPath() {
  IndexAccessedPath Iap;
  if (TheLexer.LookAhead(0)->is(tok::identifier)) {
    Iap.Path.push_back(Actions.CreateIdentifier(
        getLiteralAndAdvance(TheLexer.LookAhead(0)).str()));
    while (TheLexer.LookAhead(0)->is(tok::period)) {
      TheLexer.CachedLex(); // .
      Iap.Path.push_back(Actions.CreateIdentifier(
          getLiteralAndAdvance(TheLexer.LookAhead(0)).str()));
    }
  } else {
    Iap.ElementaryType = parseTypeName(false);
  }

  while (TheLexer.LookAhead(0)->is(tok::l_square)) {
    TheLexer.CachedLex(); // [
    Iap.Indices.emplace_back(parseExpression());
    TheLexer.CachedLex(); // ]
  }

  return Iap;
}

// TODO: IAP relative function
TypePtr
Parser::typeNameFromIndexAccessStructure(Parser::IndexAccessedPath &Iap) {
  if (Iap.empty())
    return {};

  TypePtr T;

  if (Iap.ElementaryType != nullptr) {
    T = std::move(Iap.ElementaryType);
  } else {
    vector<std::string> Path;
    for (auto const &el : Iap.Path)
      Path.push_back(el->getName());
    // TODO: UserDefinedTypeName
    // T = UserDefinedTypeName with Path
  }
  for (auto &Length : Iap.Indices) {
    T = make_shared<ArrayType>(
        std::move(T),
        dynamic_cast<const NumberLiteral *>(Length.get())->getValue(),
        getLoc(TheLexer.LookAhead(0)));
  }
  return T;
}

// TODO: IAP relative function
unique_ptr<Expr>
Parser::expressionFromIndexAccessStructure(Parser::IndexAccessedPath &Iap) {
  if (Iap.empty()) {
    return {};
  }
  unique_ptr<Expr> Expression = std::move(Iap.Path.front());
  for (size_t i = 1; i < Iap.Path.size(); ++i) {
    Expression =
        make_unique<MemberExpr>(std::move(Expression), std::move(Iap.Path[i]));
  }

  for (auto &Index : Iap.Indices) {
    Expression =
        Actions.CreateIndexAccess(std::move(Expression), std::move(Index));
  }
  return Expression;
}

unique_ptr<Expr>
Parser::parseExpression(unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Expression =
      parseBinaryExpression(4, std::move(PartiallyParsedExpression));
  if (tok::equal <= TheLexer.LookAhead(0)->getKind() &&
      TheLexer.LookAhead(0)->getKind() < tok::percentequal) {
    BinaryOperatorKind Op = token2bop(TheLexer.CachedLex());
    unique_ptr<Expr> RightHandSide = parseExpression();
    return std::move(Actions.CreateBinOp(Op, std::move(Expression),
                                         std::move(RightHandSide)));
  } else if (TheLexer.LookAhead(0)->is(tok::question)) {
    TheLexer.CachedLex();
    unique_ptr<Expr> trueExpression = parseExpression();
    TheLexer.CachedLex();
    unique_ptr<Expr> falseExpression = parseExpression();
    // TODO: Create ConditionExpression
    return nullptr;
  } else
    return Expression;
}

unique_ptr<Expr>
Parser::parseBinaryExpression(int MinPrecedence,
                              unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Expression =
      parseUnaryExpression(std::move(PartiallyParsedExpression));
  int Precedence =
      static_cast<int>(getBinOpPrecedence(TheLexer.LookAhead(0)->getKind()));
  for (; Precedence >= MinPrecedence; --Precedence) {
    while (getBinOpPrecedence(TheLexer.LookAhead(0)->getKind()) == Precedence) {
      BinaryOperatorKind Op = token2bop(TheLexer.CachedLex());
      unique_ptr<Expr> RightHandSide = parseBinaryExpression(Precedence + 1);
      Expression = std::move(Actions.CreateBinOp(Op, std::move(Expression),
                                                 std::move(RightHandSide)));
    }
  }
  return Expression;
}

unique_ptr<Expr>
Parser::parseUnaryExpression(unique_ptr<Expr> &&PartiallyParsedExpression) {
  UnaryOperatorKind Op = token2uop(TheLexer.LookAhead(0));

  if (!PartiallyParsedExpression && TheLexer.LookAhead(0)->isUnaryOp()) {
    // prefix expression
    TheLexer.CachedLex();
    unique_ptr<Expr> SubExps = parseUnaryExpression();
    return std::make_unique<UnaryOperator>(std::move(SubExps), Op);
  } else {
    // potential postfix expression
    unique_ptr<Expr> SubExps =
        parseLeftHandSideExpression(std::move(PartiallyParsedExpression));
    Op = token2uop(TheLexer.LookAhead(0), false);
    if (!(Op == UnaryOperatorKind::UO_PostInc ||
          Op == UnaryOperatorKind::UO_PostDec))
      return SubExps;
    TheLexer.CachedLex();
    return std::make_unique<UnaryOperator>(std::move(SubExps), Op);
  }
}

unique_ptr<Expr> Parser::parseLeftHandSideExpression(
    unique_ptr<Expr> &&PartiallyParsedExpression) {
  unique_ptr<Expr> Expression;
  if (PartiallyParsedExpression)
    Expression = std::move(PartiallyParsedExpression);
  else if (TheLexer.LookAhead(0)->is(tok::kw_new)) {
    TheLexer.CachedLex();
    TypePtr typeName = parseTypeName(false);
    // [AST] create NewExpression
  } else
    Expression = std::move(parsePrimaryExpression());

  while (true) {
    switch (TheLexer.LookAhead(0)->getKind()) {
    case tok::l_square: {
      TheLexer.CachedLex(); // [
      unique_ptr<Expr> Index;
      if (TheLexer.LookAhead(0)->isNot(tok::r_square))
        Index = std::move(parseExpression());
      TheLexer.CachedLex(); // ]
      Expression =
          Actions.CreateIndexAccess(std::move(Expression), std::move(Index));
      break;
    }
    case tok::period: {
      TheLexer.CachedLex();
      Expression = make_unique<MemberExpr>(
          std::move(Expression),
          Actions.CreateIdentifier(
              getLiteralAndAdvance(TheLexer.LookAhead(0)).str()));
      break;
    }
    case tok::l_paren: {
      TheLexer.CachedLex(); // (
      vector<unique_ptr<Expr>> Arguments;
      vector<llvm::StringRef> Names;
      tie(Arguments, Names) = parseFunctionCallArguments();
      TheLexer.CachedLex(); // )
      // TODO: Fix passs arguments' name fail.
      Expression =
          Actions.CreateCallExpr(std::move(Expression), std::move(Arguments));
      break;
    }
    default:
      return Expression;
    }
  }
}

unique_ptr<Expr> Parser::parsePrimaryExpression() {
  llvm::Optional<Token> CurTok = TheLexer.LookAhead(0);
  unique_ptr<Expr> Expression;

  // Explicit Type Casting
  if (CurTok->isElementaryTypeName() &&
      TheLexer.LookAhead(1)->is(tok::l_paren)) {
    TheLexer.CachedLex(); // simple type, ex. address, int
    TheLexer.CachedLex(); // (
    if (CurTok->getKind() == tok::kw_address) {
      Expression = make_unique<ExplicitCastExpr>(std::move(parseExpression()),
                                                 CastKind::TypeCast,
                                                 make_shared<AddressType>());
    } else if (tok::kw_address < CurTok->getKind()) {
      Expression = make_unique<ExplicitCastExpr>(
          std::move(parseExpression()), CastKind::IntegralCast,
          make_shared<IntegerType>(token2inttype(CurTok)));
    }
    TheLexer.CachedLex(); // )
    return Expression;
  }

  switch (CurTok->getKind()) {
  case tok::kw_true:
    Expression = std::make_unique<BooleanLiteral>(true);
    TheLexer.CachedLex();
    break;
  case tok::kw_false:
    Expression = std::make_unique<BooleanLiteral>(false);
    TheLexer.CachedLex();
    break;
  case tok::numeric_constant: {
    int NumValue;
    getLiteralAndAdvance(CurTok).getAsInteger(0, NumValue);
    Expression = std::make_unique<NumberLiteral>(NumValue);
    break;
  }
  case tok::string_literal: {
    string StrValue = getLiteralAndAdvance(CurTok).str();
    Expression = make_unique<StringLiteral>(stringUnquote(std::move(StrValue)));
    break;
  }
  case tok::hex_string_literal: {
    string StrValue = getLiteralAndAdvance(CurTok).str();
    Expression = make_unique<StringLiteral>(hexUnquote(std::move(StrValue)));
    break;
  }
  case tok::identifier: {
    Expression = Actions.CreateIdentifier(getLiteralAndAdvance(CurTok).str());
    break;
  }
  case tok::kw_type:
    // TODO: Type expression is globally-avariable function
    assert(false && "Type not support right now\n");
    break;
  case tok::l_paren:
  case tok::l_square: {
    // TODO: Tuple case
    //
    // Tuple/parenthesized expression or inline array/bracketed expression.
    // Special cases: ()/[] is empty tuple/array type, (x) is not a real tuple,
    // (x,) is one-dimensional tuple, elements in arrays cannot be left out,
    // only in tuples.
    TheLexer.CachedLex(); // [ | (
    tok::TokenKind OppositeToken =
        (CurTok->getKind() == tok::l_paren ? tok::r_paren : tok::r_square);
    Expression = std::make_unique<ParenExpr>(std::move(parseExpression()));
    TheLexer.CachedLex();
    break;
  }
  case tok::unknown:
    assert(false && "Unknown token");
    break;
  default:
    // TODO: Type MxN case
    assert(false && "Expected primary expression.");
    break;
  }
  return Expression;
}

vector<unique_ptr<Expr>> Parser::parseFunctionCallListArguments() {
  vector<unique_ptr<Expr>> Arguments;
  if (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
    Arguments.push_back(parseExpression());
    while (TheLexer.LookAhead(0)->isNot(tok::r_paren)) {
      TheLexer.CachedLex();
      Arguments.push_back(parseExpression());
    }
  }
  return Arguments;
}

pair<vector<unique_ptr<Expr>>, vector<llvm::StringRef>>
Parser::parseFunctionCallArguments() {
  pair<vector<unique_ptr<Expr>>, vector<llvm::StringRef>> Ret;
  if (TheLexer.LookAhead(0)->is(tok::l_brace)) {
    // TODO: Unverified function parameters case
    // call({arg1 : 1, arg2 : 2 })
    TheLexer.CachedLex();
    bool First = true;
    while (TheLexer.LookAhead(0)->isNot(tok::r_brace)) {
      if (!First)
        TheLexer.CachedLex();

      Ret.second.push_back(getLiteralAndAdvance(TheLexer.LookAhead(0)));
      TheLexer.CachedLex();
      Ret.first.push_back(parseExpression());

      if (TheLexer.LookAhead(0)->is(tok::comma) &&
          TheLexer.LookAhead(1)->is(tok::r_brace)) {
        assert(false && "Unexpected trailing comma.");
        TheLexer.CachedLex();
      }
      First = false;
    }
    TheLexer.CachedLex();
  } else
    Ret.first = std::move(parseFunctionCallListArguments());
  return Ret;
}

unique_ptr<AST> Parser::createEmptyParameterList() { return nullptr; }

llvm::StringRef Parser::getLiteralAndAdvance(llvm::Optional<Token> Tok) {
  TheLexer.CachedLex();
  if (Tok->is(tok::identifier))
    return Tok->getIdentifierInfo()->getName();
  assert(Tok->isLiteral() && "except tok::literal");
  return llvm::StringRef(Tok->getLiteralData(), Tok->getLength());
}

void Parser::EnterScope(unsigned ScopeFlags) {
  Actions.PushScope(ScopeFlags);
}

void Parser::ExitScope() {
  Actions.PopScope();
}

} // namespace soll
