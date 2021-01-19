// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Parse/Parser.h"
#include "soll/Basic/DiagnosticIDs.h"
#include "soll/Basic/DiagnosticParse.h"
#include "soll/Basic/OperatorPrecedence.h"
#include "soll/Basic/TokenKinds.h"
#include "soll/Lex/Lexer.h"
#include "soll/Lex/Token.h"
#include <llvm/Support/Compiler.h>

namespace soll {

static BinaryOperatorKind token2bop(const Token &Tok) {
  switch (Tok.getKind()) {
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

static UnaryOperatorKind token2uop(const Token &Tok, bool IsPreOp = true) {
  switch (Tok.getKind()) {
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

static IntegerType::IntKind token2inttype(const Token &Tok) {
  switch (Tok.getKind()) {
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

static FixedBytesType::ByteKind token2bytetype(const Token &Tok) {
  switch (Tok.getKind()) {
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

uint64_t token2UnitMultiplier(const Token &Tok) {
  switch (Tok.getKind()) {
  case tok::kw_wei:
    return 1;
  case tok::kw_szabo:
    return 1000000000000ul;
  case tok::kw_finney:
    return 1000000000000000ul;
  case tok::kw_ether:
    return 1000000000000000000ul;
  case tok::kw_seconds:
    return 1;
  case tok::kw_minutes:
    return 60;
  case tok::kw_hours:
    return 3600;
  case tok::kw_days:
    return 86400;
  case tok::kw_weeks:
    return 604800;
  default:
    assert(false && "Invalid unit token.");
  }
  LLVM_BUILTIN_UNREACHABLE;
}

Parser::Parser(Lexer &TheLexer, Sema &Actions, DiagnosticsEngine &Diags)
    : TheLexer(TheLexer), Actions(Actions), Diags(Diags) {
  Tok = *TheLexer.CachedLex();
}

std::unique_ptr<SourceUnit> Parser::parse() {
  std::unique_ptr<SourceUnit> SU;
  {
    ParseScope SourceUnitScope{this, 0};
    std::vector<std::unique_ptr<Decl>> Nodes;
    const SourceLocation Begin = Tok.getLocation();

    while (Tok.isNot(tok::eof)) {
      switch (Tok.getKind()) {
      case tok::kw_pragma:
        Nodes.push_back(parsePragmaDirective());
        break;
      case tok::kw_import:
        ConsumeToken();
        break;
      case tok::kw_interface:
      case tok::kw_library:
      case tok::kw_contract: {
        Nodes.push_back(parseContractDefinition());
        break;
      }
      default:
        ConsumeAnyToken();
        break;
      }
    }
    SU = std::make_unique<SourceUnit>(SourceRange(Begin, Tok.getLocation()),
                                      std::move(Nodes));
  }
  Actions.resolveType(*SU);
  return SU;
}

std::unique_ptr<PragmaDirective> Parser::parsePragmaDirective() {
  // pragma anything* ;
  // Currently supported:
  // pragma solidity ^0.4.0 || ^0.3.0;
  const SourceLocation Begin = Tok.getLocation();
  std::vector<std::string> Literals;
  std::vector<Token> Tokens;
  ConsumeToken(); // 'pragma'
  do {
    const tok::TokenKind Kind = Tok.getKind();
    switch (Kind) {
#define PUNCTUATOR(X, Y) case tok::X:
#include "soll/Basic/TokenKinds.def"
      Tokens.push_back(Tok);
      Literals.push_back(tok::getPunctuatorSpelling(Kind));
      ConsumeToken();
      break;
    case tok::raw_identifier:
    case tok::identifier:
      Tokens.push_back(Tok);
      Literals.emplace_back(Tok.getIdentifierInfo()->getName().str());
      ConsumeToken();
      break;
    case tok::numeric_constant:
      Tokens.push_back(Tok);
      Literals.emplace_back(Tok.getLiteralData(), Tok.getLength());
      ConsumeToken();
      break;
    default:
      Diag(diag::err_unknown_pragma);
      ConsumeAnyToken();
      break;
    }
  } while (!Tok.isOneOf(tok::semi, tok::eof));
  const SourceLocation End = Tok.getEndLoc();
  if (ExpectAndConsumeSemi()) {
    return nullptr;
  }

  // TODO: Implement version recognize and compare. ref: parsePragmaVersion
  return std::make_unique<PragmaDirective>(SourceRange(Begin, End));
}

ContractDecl::ContractKind Parser::parseContractKind() {
  switch (Tok.getKind()) {
  case tok::kw_interface:
    ConsumeToken();
    return ContractDecl::ContractKind::Interface;
  case tok::kw_contract:
    ConsumeToken();
    return ContractDecl::ContractKind::Contract;
  case tok::kw_library:
    ConsumeToken();
    return ContractDecl::ContractKind::Library;
  default:
    Diag(diag::err_expected_contract_kind);
    ConsumeAnyToken();
    return ContractDecl::ContractKind::Contract;
  }
}

Decl::Visibility Parser::parseVisibilitySpecifier() {
  switch (Tok.getKind()) {
  case tok::kw_public:
    ConsumeToken();
    return Decl::Visibility::Public;
  case tok::kw_internal:
    ConsumeToken();
    return Decl::Visibility::Internal;
  case tok::kw_private:
    ConsumeToken();
    return Decl::Visibility::Private;
  case tok::kw_external:
    ConsumeToken();
    return Decl::Visibility::External;
  default:
    Diag(diag::err_expected_visibility);
    ConsumeAnyToken();
    return Decl::Visibility::Default;
  }
}

StateMutability Parser::parseStateMutability() {
  StateMutability stateMutability(StateMutability::NonPayable);
  switch (Tok.getKind()) {
  case tok::kw_payable:
    ConsumeToken();
    return StateMutability::Payable;
  case tok::kw_view:
    ConsumeToken();
    return StateMutability::View;
  case tok::kw_pure:
    ConsumeToken();
    return StateMutability::Pure;
  case tok::kw_constant:
    Diag(diag::warn_constant_removed);
    ConsumeToken();
    return StateMutability::View;
  default:
    Diag(diag::err_expected_state_mutability);
    ConsumeAnyToken();
    return StateMutability::NonPayable;
    assert(false && "Invalid state mutability specifier.");
  }
  return stateMutability;
}

DataLocation Parser::parseDataLocation() {
  switch (Tok.getKind()) {
  case tok::kw_storage:
    ConsumeToken();
    return DataLocation::Storage;
  case tok::kw_memory:
    ConsumeToken();
    return DataLocation::Memory;
  case tok::kw_calldata:
    ConsumeToken();
    return DataLocation::CallData;
  default:
    return DataLocation::Storage;
  }
}

std::unique_ptr<ContractDecl> Parser::parseContractDefinition() {
  const SourceLocation Begin = Tok.getLocation();
  ParseScope ContractScope{this, 0};
  ContractDecl::ContractKind CtKind = parseContractKind();
  if (!Tok.isAnyIdentifier()) {
    Diag(diag::err_expected) << tok::identifier;
    return nullptr;
  }
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  ConsumeToken();

  std::vector<std::unique_ptr<InheritanceSpecifier>> BaseContracts;
  std::vector<std::unique_ptr<Decl>> SubNodes;
  std::unique_ptr<FunctionDecl> Constructor;
  std::unique_ptr<FunctionDecl> Fallback;

  if (TryConsumeToken(tok::kw_is)) {
    do {
      // TODO: Update vector<InheritanceSpecifier> baseContracts
      if (!Tok.isAnyIdentifier()) {
        Diag(diag::err_expected) << tok::identifier;
        return nullptr;
      }
      std::string BaseName = Tok.getIdentifierInfo()->getName();
      ConsumeToken(); // identifier

      std::vector<std::unique_ptr<Expr>> Arguments;
      if (isTokenParen()) {
        ConsumeParen();
        while (!isTokenParen()) {
          if (ExpectAndConsume(tok::comma)) {
            return nullptr;
          }
          Arguments.emplace_back(parseExpression());
        }
      }
      BaseContracts.emplace_back(std::make_unique<InheritanceSpecifier>(
          std::move(BaseName), std::move(Arguments)));
    } while (TryConsumeToken(tok::comma));
  }

  if (ExpectAndConsume(tok::l_brace)) {
    return nullptr;
  }

  SourceLocation End = Tok.getEndLoc();
  while (Tok.isNot(tok::eof)) {
    if (Tok.is(tok::r_brace)) {
      End = Tok.getEndLoc();
      ConsumeBrace();
      break;
    }

    // TODO: < Parse all Types in contract's context >
    if (Tok.isOneOf(tok::kw_function, tok::kw_constructor, tok::kw_fallback)) {
      auto FD = parseFunctionDefinitionOrFunctionTypeStateVariable();
      if (FD) {
        Actions.addDecl(FD.get());
        if (FD->isConstructor()) {
          if (Constructor) {
            Diag(FD->getLocation().getBegin(), diag::err_multiple_constuctors);
            Diag(Constructor->getLocation().getBegin(),
                 diag::note_previous_definition);
            assert(false && "multiple constructor defined!");
          }
          Constructor = std::move(FD);
        } else if (FD->isFallback()) {
          if (Fallback) {
            Diag(FD->getLocation().getBegin(), diag::err_multiple_fallbacks);
            Diag(Fallback->getLocation().getBegin(),
                 diag::note_previous_definition);
            assert(false && "multiple fallback defined!");
          }
          Fallback = std::move(FD);
        } else {
          SubNodes.push_back(std::move(FD));
        }
      }
      Actions.EraseFunRtnTys();
    } else if (Tok.is(tok::kw_struct)) {
      SubNodes.push_back(parseStructDeclaration());
    } else if (Tok.is(tok::kw_enum)) {
      // TODO: contract tok::kw_enum
      Diag(diag::err_unimplemented_token) << tok::kw_enum;
      return nullptr;
    } else if (Tok.isElementaryTypeName() || Tok.isAnyIdentifier() ||
               Tok.is(tok::kw_mapping)) {
      VarDeclParserOptions options;
      options.IsStateVariable = true;
      options.AllowInitialValue = true;
      SubNodes.push_back(parseVariableDeclaration(options));
      if (ExpectAndConsumeSemi()) {
        return nullptr;
      }
    } else if (Tok.is(tok::kw_modifier)) {
      // TODO: contract tok::kw_modifier
      Diag(diag::err_unimplemented_token) << tok::kw_modifier;
      return nullptr;
    } else if (TryConsumeToken(tok::kw_event)) {
      SubNodes.push_back(parseEventDefinition());
    } else if (Tok.is(tok::kw_using)) {
      // TODO: contract tok::kw_using
      Diag(diag::err_unimplemented_token) << tok::kw_using;
      return nullptr;
    } else {
      Diag(diag::err_expected_contract_part);
      return nullptr;
    }
  }
  auto CD = std::make_unique<ContractDecl>(
      SourceRange(Begin, End), Name, std::move(BaseContracts),
      std::move(SubNodes), std::move(Constructor), std::move(Fallback), CtKind);
  auto CT = std::make_shared<ContractType>(CD.get());
  CD->setContractType(CT);
  Actions.addDecl(CD.get());
  Actions.addContractDecl(CD.get());
  return CD;
}

Parser::FunctionHeaderParserResult
Parser::parseFunctionHeader(bool ForceEmptyName, bool AllowModifiers) {

  FunctionHeaderParserResult Result;

  Result.IsConstructor = false;
  Result.IsFallback = false;

  if (Tok.is(tok::kw_constructor)) {
    Result.IsConstructor = true;
  } else if (ForceEmptyName || Tok.is(tok::kw_fallback)) {
    Result.IsFallback = true;
  } else {
    assert(Tok.is(tok::kw_function));
  }
  ConsumeToken();

  if (Result.IsConstructor) {
    Result.Name = llvm::StringRef("solidity.constructor");
  } else if (Tok.is(tok::l_paren) || Result.IsFallback) {
    Result.Name = llvm::StringRef("solidity.fallback");
    Result.IsFallback = true;
  } else if (Tok.isAnyIdentifier()) {
    Result.Name = Tok.getIdentifierInfo()->getName();
    ConsumeToken(); // identifier
  } else {
    Diag(diag::err_expected_after) << "'function'"
                                   << "identifier";
    assert(false);
  }

  VarDeclParserOptions Options;
  Options.AllowLocationSpecifier = true;

  Result.Parameters = parseParameterList(Options);

  while (true) {
    if (AllowModifiers && Tok.is(tok::identifier)) {
      Diag(diag::err_unimplemented_token) << tok::identifier;

      std::string ModifierName = Tok.getIdentifierInfo()->getName();
      ConsumeToken(); // identifier

      std::vector<ExprPtr> Arguments;
      if (Tok.is(tok::l_paren)) {
        ConsumeParen(); // (
        std::vector<llvm::StringRef> Names;
        std::tie(Arguments, Names) = parseFunctionCallArguments();
        ConsumeParen(); // )
      }
      Result.Modifiers.push_back(std::make_unique<ModifierInvocation>(
          ModifierName, std::move(Arguments)));
    } else if (Tok.isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal,
                           tok::kw_external)) {
      // TODO: Special case of a public state variable of function Type.
      Result.Vsblty = parseVisibilitySpecifier();
    } else if (Tok.isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                           tok::kw_payable)) {
      Result.SM = parseStateMutability();
    } else {
      break;
    }
  }

  if (TryConsumeToken(tok::kw_returns)) {
    bool const PermitEmptyParameterList = false;
    Result.ReturnParameters =
        parseParameterList(Options, PermitEmptyParameterList);
    std::vector<TypePtr> Tys;
    for (auto &&Return : Result.ReturnParameters->getParams())
      Tys.push_back(Return->GetType());
    Actions.SetFunRtnTys(std::move(Tys));
  } else {
    Result.ReturnParameters = std::make_unique<ParamList>(
        std::vector<std::unique_ptr<VarDeclBase>>());
  }

  return Result;
}

std::unique_ptr<FunctionDecl>
Parser::parseFunctionDefinitionOrFunctionTypeStateVariable() {
  const SourceLocation Begin = Tok.getLocation();
  ParseScope ArgumentScope{this, 0};
  FunctionHeaderParserResult Header = parseFunctionHeader(false, true);
  if (Header.IsConstructor || !Header.Modifiers.empty() ||
      !Header.Name.empty() || Tok.isOneOf(tok::semi, tok::l_brace)) {
    // this has to be a function, consume the tokens and store them for later
    // parsing
    SourceLocation End = Tok.getEndLoc();
    std::unique_ptr<Block> B;
    if (Tok.is(tok::l_brace)) {
      ParseScope FunctionScope{this, Scope::FunctionScope};
      B = parseBlock();
      End = B->getLocation().getEnd();
    } else if (ExpectAndConsumeSemi()) {
      Diag(diag::err_expected_event);
      return nullptr;
    }
    auto FD = Actions.CreateFunctionDecl(
        SourceRange(Begin, End), Header.Name, Header.Vsblty, Header.SM,
        Header.IsConstructor, Header.IsFallback, std::move(Header.Parameters),
        std::move(Header.Modifiers), std::move(Header.ReturnParameters),
        std::move(B));
    return FD;
  } else {
    // TODO: State Variable case.
    return nullptr;
  }
}

std::unique_ptr<StructDecl> Parser::parseStructDeclaration() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'struct'
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  if (Actions.lookupName(Name)) {
    Diag(diag::err_duplicate_definition) << Name.str();
    return nullptr;
  }
  ConsumeToken(); // Name
  if (ExpectAndConsume(tok::l_brace)) {
    return nullptr;
  }
  SourceLocation End = Tok.getEndLoc();
  std::vector<TypePtr> ElementTypes;
  std::vector<std::string> ElementNames;
  while (Tok.isNot(tok::eof)) {
    if (Tok.is(tok::r_brace)) {
      End = Tok.getEndLoc();
      ConsumeBrace();
      break;
    }
    TypePtr T = parseTypeName(false);
    std::string ElementName = Tok.getIdentifierInfo()->getName().str();
    ConsumeToken();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    ElementTypes.emplace_back(T);
    ElementNames.emplace_back(ElementName);
  }
  auto SD = std::make_unique<StructDecl>(SourceRange(Begin, End), Name,
                                         std::move(ElementTypes),
                                         std::move(ElementNames));
  Actions.addDecl(SD.get());
  return SD;
}

bool Parser::ConsumeAndStoreUntil(tok::TokenKind T1, tok::TokenKind T2,
                                  llvm::SmallVector<Token, 4> &Toks) {
  while (true) {
    // If we found one of the tokens, stop and return true.
    if (Tok.is(T1) || Tok.is(T2)) {
      Toks.push_back(Tok);
      ConsumeAnyToken(); // T1 | T2
      return true;
    }

    switch (Tok.getKind()) {
    case tok::eof:
      // Ran out of tokens.
      return false;

    case tok::l_paren:
      // Recursively consume properly-nested parens.
      Toks.push_back(Tok);
      ConsumeParen();
      ConsumeAndStoreUntil(tok::r_paren, Toks);
      break;
    case tok::l_square:
      // Recursively consume properly-nested square brackets.
      Toks.push_back(Tok);
      ConsumeBracket();
      ConsumeAndStoreUntil(tok::r_square, Toks);
      break;
    case tok::l_brace:
      // Recursively consume properly-nested braces.
      Toks.push_back(Tok);
      ConsumeBrace();
      ConsumeAndStoreUntil(tok::r_brace, Toks);
      break;

    case tok::r_paren:
      Toks.push_back(Tok);
      ConsumeParen();
      break;

    case tok::r_square:
      Toks.push_back(Tok);
      ConsumeBracket();
      break;

    case tok::r_brace:
      Toks.push_back(Tok);
      ConsumeBrace();
      break;

    default:
      // consume this token.
      Toks.push_back(Tok);
      ConsumeAnyToken();
      break;
    }
  }
}

std::unique_ptr<VarDecl>
Parser::parseVariableDeclaration(VarDeclParserOptions const &Options,
                                 TypePtr &&LookAheadArrayType) { // TODO
  const SourceLocation Begin = Tok.getLocation();
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
  while (!Tok.is(tok::eof)) {
    if (Options.IsStateVariable &&
        Tok.isOneOf(tok::kw_public, tok::kw_private, tok::kw_internal)) {
      Vsblty = parseVisibilitySpecifier();
    } else if (Options.AllowIndexed && Tok.is(tok::kw_indexed)) {
      IsIndexed = true;
      ConsumeToken(); // 'indexed'
    } else if (Tok.is(tok::kw_constant)) {
      IsDeclaredConst = true;
      ConsumeToken(); // 'constant'
    } else if (Options.AllowLocationSpecifier &&
               Tok.isOneOf(tok::kw_memory, tok::kw_storage, tok::kw_calldata)) {
      if (Loc != VarDecl::Location::Unspecified) {
        Diag(diag::err_multiple_variable_location);
        return nullptr;
      }
      if (!T) {
        Diag(diag::err_location_without_typename);
        return nullptr;
      }
      switch (Tok.getKind()) {
      case tok::kw_storage:
        Loc = VarDecl::Location::Storage;
        ConsumeToken(); // 'storage'
        break;
      case tok::kw_memory:
        Loc = VarDecl::Location::Memory;
        ConsumeToken(); // 'memory'
        break;
      case tok::kw_calldata:
        Loc = VarDecl::Location::CallData;
        ConsumeToken(); // 'calldata'
        break;
      default:
        __builtin_unreachable();
      }
    } else {
      break;
    }
  }

  if (Options.AllowEmptyName && !Tok.isAnyIdentifier()) {
    Name = llvm::StringRef("");
  } else {
    Name = Tok.getIdentifierInfo()->getName();
    ConsumeToken();
  }

  std::unique_ptr<Expr> Value;
  if (Options.AllowInitialValue) {
    if (TryConsumeToken(tok::equal)) {
      Value = Actions.CreateDummy(parseExpression());
    }
  }

  auto VD = std::make_unique<VarDecl>(SourceRange(Begin, Tok.getEndLoc()), Name,
                                      Vsblty, std::move(T), std::move(Value),
                                      Options.IsStateVariable, IsIndexed,
                                      IsDeclaredConst, Loc);

  Actions.addDecl(VD.get());
  return VD;
}

std::unique_ptr<EventDecl> Parser::parseEventDefinition() {
  const SourceLocation Begin = Tok.getLocation();
  const std::string Name = Tok.getIdentifierInfo()->getName();
  ConsumeToken(); // identifier
  VarDeclParserOptions Options;
  Options.AllowIndexed = true;
  std::unique_ptr<ParamList> Parameters = parseParameterList(Options);
  bool Anonymous = false;
  if (TryConsumeToken(tok::kw_anonymous)) {
    Anonymous = true;
  }
  if (ExpectAndConsumeSemi()) {
    return nullptr;
  }
  auto ED = Actions.CreateEventDecl(SourceRange(Begin, Tok.getEndLoc()), Name,
                                    std::move(Parameters), Anonymous);
  Actions.addDecl(ED.get());
  return ED;
}

TypePtr Parser::parseTypeNameSuffix(TypePtr T) {
  while (isTokenBracket()) {
    ConsumeBracket();
    if (Tok.is(tok::numeric_constant)) {
      llvm::StringRef NumValue(Tok.getLiteralData(), Tok.getLength());
      const auto [Signed, Value] = numericParse(NumValue);
      if (Signed) {
        Diag(diag::err_negative_array_size);
        return nullptr;
      }
      ConsumeToken(); // numeric_constant
      if (ExpectAndConsume(tok::r_square)) {
        return nullptr;
      }
      T = std::make_shared<ArrayType>(std::move(T), Value, parseDataLocation());
    } else {
      if (ExpectAndConsume(tok::r_square)) {
        return nullptr;
      }
      T = std::make_shared<ArrayType>(std::move(T), parseDataLocation());
    }
  }
  return T;
}

// TODO: < Need complete all Types >
TypePtr Parser::parseTypeName(bool AllowVar) {
  TypePtr T;
  bool HaveType = false;
  const tok::TokenKind Kind = Tok.getKind();
  if (Tok.isElementaryTypeName()) {
    if (Kind == tok::kw_bool) {
      T = std::make_shared<BooleanType>();
      ConsumeToken(); // 'bool'
    } else if (tok::kw_int <= Kind && Kind <= tok::kw_uint256) {
      T = std::make_shared<IntegerType>(token2inttype(Tok));
      ConsumeToken(); // int or uint
    } else if (tok::kw_bytes1 <= Kind && Kind <= tok::kw_bytes32) {
      T = std::make_shared<FixedBytesType>(token2bytetype(Tok));
      ConsumeToken(); // fixedbytes
    } else if (Kind == tok::kw_bytes) {
      T = std::make_shared<BytesType>();
      ConsumeToken(); // 'bytes'
    } else if (Kind == tok::kw_string) {
      T = std::make_shared<StringType>();
      ConsumeToken(); // 'string'
    } else if (Kind == tok::kw_address) {
      ConsumeToken(); // 'address'
      StateMutability SM = StateMutability::NonPayable;
      if (Tok.isOneOf(tok::kw_constant, tok::kw_pure, tok::kw_view,
                      tok::kw_payable)) {
        SM = parseStateMutability();
      }
      T = std::make_shared<AddressType>(SM);
    }
    HaveType = true;
  } else if (Kind == tok::kw_var) {
    // TODO: parseTypeName tok::kw_var (var is deprecated)
    Diag(diag::err_unimplemented_token) << tok::kw_var;
    return nullptr;
  } else if (Kind == tok::kw_function) {
    // TODO: parseTypeName tok::kw_function
    Diag(diag::err_unimplemented_token) << tok::kw_function;
    return nullptr;
  } else if (Kind == tok::kw_mapping) {
    T = parseMapping();
  } else if (Kind == tok::identifier || Kind == tok::raw_identifier) {
    // TODO: parseTypeName tok::identifier
    llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
    Decl *D = Actions.lookupName(Name);
    if (auto SD = dynamic_cast<StructDecl *>(D)) {
      T = SD->getType();
      HaveType = true;
      ConsumeToken(); // identifier
    } else {
      T = std::make_shared<UnresolveType>(Name);
      HaveType = true;
      ConsumeToken(); // identifier
    }
  } else {
    assert(false && "Expected Type Name");
    return nullptr;
  }

  if (T || HaveType) {
    T = parseTypeNameSuffix(move(T));
  }
  return T;
}

std::shared_ptr<MappingType> Parser::parseMapping() {
  if (ExpectAndConsume(tok::kw_mapping)) {
    return nullptr;
  }
  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }
  bool const AllowVar = false;
  TypePtr KeyType;
  if (Tok.isElementaryTypeName()) {
    KeyType = parseTypeName(AllowVar);
  }
  if (ExpectAndConsume(tok::equalgreater)) {
    return nullptr;
  }
  TypePtr ValueType = parseTypeName(AllowVar);
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }
  return std::make_shared<MappingType>(std::move(KeyType),
                                       std::move(ValueType));
}

std::unique_ptr<ParamList>
Parser::parseParameterList(VarDeclParserOptions const &_Options,
                           bool AllowEmpty) {
  std::vector<std::unique_ptr<VarDeclBase>> Parameters;
  VarDeclParserOptions Options(_Options);
  Options.AllowEmptyName = true;
  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }
  if (!AllowEmpty || Tok.isNot(tok::r_paren)) {
    Parameters.push_back(parseVariableDeclaration(Options));
    while (Tok.isNot(tok::r_paren)) {
      if (ExpectAndConsume(tok::comma)) {
        return nullptr;
      }
      Parameters.push_back(parseVariableDeclaration(Options));
    }
  }
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }

  return std::make_unique<ParamList>(std::move(Parameters));
}

std::unique_ptr<Block> Parser::parseBlock() {
  const SourceLocation Begin = Tok.getLocation();
  ParseScope BlockScope{this, 0};
  std::vector<std::unique_ptr<Stmt>> Statements;
  if (ExpectAndConsume(tok::l_brace)) {
    return nullptr;
  }
  while (Tok.isNot(tok::r_brace)) {
    auto Statement = parseStatement();
    if (!Statement) {
      break;
    }
    Statements.emplace_back(std::move(Statement));
  }
  const SourceLocation End = Tok.getEndLoc();
  ConsumeBrace(); // '}'
  return std::make_unique<Block>(SourceRange(Begin, End),
                                 std::move(Statements));
}

// TODO: < Parse all statements >
std::unique_ptr<Stmt> Parser::parseStatement() {
  std::unique_ptr<Stmt> S;
  switch (Tok.getKind()) {
  case tok::kw_if:
    return parseIfStatement();
  case tok::kw_while:
    return parseWhileStatement();
  case tok::kw_do:
    return parseDoWhileStatement();
  case tok::kw_for:
    return parseForStatement();
  case tok::l_brace:
    return parseBlock();
  case tok::kw_continue: {
    const SourceLocation Begin = Tok.getLocation();
    ConsumeToken(); // 'continue'
    const SourceLocation End = Tok.getEndLoc();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    S = std::make_unique<ContinueStmt>(SourceRange(Begin, End));
    break;
  }
  case tok::kw_break: {
    const SourceLocation Begin = Tok.getLocation();
    ConsumeToken(); // 'break'
    const SourceLocation End = Tok.getEndLoc();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    S = std::make_unique<BreakStmt>(SourceRange(Begin, End));
    break;
  }
  case tok::kw_return: {
    const SourceLocation Begin = Tok.getLocation();
    ConsumeToken(); // 'return'
    std::unique_ptr<Expr> E;
    if (Tok.isNot(tok::semi)) {
      E = Actions.CreateDummy(parseExpression());
    }
    const SourceLocation End = Tok.getEndLoc();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    S = std::make_unique<ReturnStmt>(SourceRange(Begin, End), std::move(E));
    break;
  }
  case tok::kw_assembly:
    // TODO: parseStatement kw_assembly
    Diag(diag::err_unimplemented_token) << tok::kw_assembly;
    ConsumeToken(); // 'assembly'
    break;
  case tok::kw_emit:
    S = parseEmitStatement();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    break;
  case tok::identifier:
  case tok::raw_identifier:
  default:
    S = parseSimpleStatement();
    if (ExpectAndConsumeSemi()) {
      return nullptr;
    }
    break;
  }
  return S;
}

std::unique_ptr<IfStmt> Parser::parseIfStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'if'
  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }
  std::unique_ptr<Expr> Condition = std::make_unique<ImplicitCastExpr>(
      parseExpression(), CastKind::TypeCast, std::make_shared<BooleanType>());
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }
  std::unique_ptr<Stmt> TrueBody = parseStatement();
  SourceLocation End = TrueBody->getLocation().getEnd();

  std::unique_ptr<Stmt> FalseBody;
  if (TryConsumeToken(tok::kw_else)) {
    FalseBody = parseStatement();
    End = FalseBody->getLocation().getEnd();
  }
  return std::make_unique<IfStmt>(SourceRange(Begin, End), std::move(Condition),
                                  std::move(TrueBody), std::move(FalseBody));
}

std::unique_ptr<WhileStmt> Parser::parseWhileStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'while'
  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }
  std::unique_ptr<Expr> Condition = std::make_unique<ImplicitCastExpr>(
      parseExpression(), CastKind::TypeCast, std::make_shared<BooleanType>());
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }
  std::unique_ptr<Stmt> Body;
  {
    ParseScope WhileScope{this, Scope::BreakScope | Scope::ContinueScope};
    Body = parseStatement();
  }
  const SourceLocation End = Body->getLocation().getEnd();
  return std::make_unique<WhileStmt>(
      SourceRange(Begin, End), std::move(Condition), std::move(Body), false);
}

std::unique_ptr<WhileStmt> Parser::parseDoWhileStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'do'
  std::unique_ptr<Stmt> Body;
  {
    ParseScope DoWhileScope{this, Scope::BreakScope | Scope::ContinueScope};
    Body = parseStatement();
  }
  if (ExpectAndConsume(tok::kw_while)) {
    return nullptr;
  }
  if (ExpectAndConsume(tok::l_brace)) {
    return nullptr;
  }
  std::unique_ptr<Expr> Condition = std::make_unique<ImplicitCastExpr>(
      parseExpression(), CastKind::TypeCast, std::make_shared<BooleanType>());
  if (ExpectAndConsume(tok::r_brace)) {
    return nullptr;
  }
  const SourceLocation End = Tok.getEndLoc();
  if (ExpectAndConsumeSemi()) {
    return nullptr;
  }
  return std::make_unique<WhileStmt>(
      SourceRange(Begin, End), std::move(Condition), std::move(Body), true);
}

std::unique_ptr<ForStmt> Parser::parseForStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'for'
  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }

  // TODO: Maybe here have some predicate like peekExpression() instead of
  // checking for semicolon and RParen?
  std::unique_ptr<Stmt> Init;
  if (Tok.isNot(tok::semi)) {
    Init = parseSimpleStatement();
  }
  if (ExpectAndConsumeSemi()) {
    return nullptr;
  }

  std::unique_ptr<Expr> Condition;
  if (Tok.isNot(tok::semi)) {
    Condition = std::make_unique<ImplicitCastExpr>(
        parseExpression(), CastKind::TypeCast, std::make_shared<BooleanType>());
  }
  if (ExpectAndConsumeSemi()) {
    return nullptr;
  }

  std::unique_ptr<Expr> Loop;
  if (Tok.isNot(tok::r_paren)) {
    Loop = parseExpression();
  }
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }

  std::unique_ptr<Stmt> Body;
  {
    ParseScope ForScope{this, Scope::BreakScope | Scope::ContinueScope};
    Body = parseStatement();
  }
  const SourceLocation End = Body->getLocation().getEnd();
  return std::make_unique<ForStmt>(SourceRange(Begin, End), std::move(Init),
                                   std::move(Condition), std::move(Loop),
                                   std::move(Body));
}

std::unique_ptr<EmitStmt> Parser::parseEmitStatement() {
  const SourceLocation Begin = Tok.getLocation();
  ConsumeToken(); // 'emit'
  const SourceLocation CallBegin = Tok.getLocation();

  IndexAccessedPath Iap;
  while (true) {
    if (Tok.isNot(tok::identifier)) {
      Diag(diag::err_expected_event);
      return nullptr;
    }
    Iap.Path.emplace_back(Tok);
    ConsumeToken(); // identifier
    if (Tok.isNot(tok::period))
      break;
    ConsumeToken(); // '.'
  };

  auto EventName = expressionFromIndexAccessStructure(Iap);

  if (ExpectAndConsume(tok::l_paren)) {
    return nullptr;
  }
  std::vector<std::unique_ptr<Expr>> Arguments;
  std::vector<llvm::StringRef> Names;
  std::tie(Arguments, Names) = parseFunctionCallArguments();
  const SourceLocation End = Tok.getEndLoc();
  if (ExpectAndConsume(tok::r_paren)) {
    return nullptr;
  }
  std::unique_ptr<CallExpr> Call = Actions.CreateCallExpr(
      SourceRange(CallBegin, End), std::move(EventName), std::move(Arguments));
  return std::make_unique<EmitStmt>(SourceRange(Begin, End), std::move(Call));
}

std::unique_ptr<Stmt> Parser::parseSimpleStatement() {
  const SourceLocation Begin = Tok.getLocation();
  LookAheadInfo StatementType;
  IndexAccessedPath Iap;
  std::unique_ptr<Expr> Expression;
  size_t EmptyComponents = 0;

  bool IsParenExpr = false;
  if (isTokenParen()) {
    ConsumeParen();
    IsParenExpr = true;

    while (Tok.getKind() == tok::comma) {
      ExpectAndConsume(tok::comma);
      EmptyComponents++;
    }
  }

  std::tie(StatementType, Iap) = tryParseIndexAccessedPath();
  switch (StatementType) {
  case LookAheadInfo::VariableDeclaration:
    return parseVariableDeclarationStatement(
        typeNameFromIndexAccessStructure(Iap));
  case LookAheadInfo::Expression:
    Expression = parseExpression(expressionFromIndexAccessStructure(Iap));
    break;
  default:
    assert(false && "Unhandle statement.");
  }
  if (IsParenExpr) {
    std::vector<ExprPtr> Comps(EmptyComponents);
    bool ReachComma = false;
    Comps.emplace_back(std::move(Expression));

    if (EmptyComponents != 0) { // already meet
      ReachComma = true;
    }

    while (Tok.getKind() != tok::r_paren) {
      ExpectAndConsume(tok::comma);
      ReachComma = true;
      if (Tok.getKind() == tok::comma || Tok.getKind() == tok::r_paren) {
        Comps.emplace_back(ExprPtr());
      } else {
        Comps.emplace_back(parseExpression());
      }
    }

    ExpectAndConsume(tok::r_paren);

    const SourceLocation End = Tok.getEndLoc();
    bool IsArray = false;

    if (!ReachComma) {
      return parseExpression(std::make_unique<ParenExpr>(
          SourceRange(Begin, End), std::move(Comps.back())));
    }

    for (auto &Comp : Comps) {
      if (Comp)
        Comp = Actions.CreateDummy(std::move(Comp));
    }

    return parseExpression(std::make_unique<TupleExpr>(
        SourceRange(Begin, End), std::move(Comps), IsArray));
  }
  return Expression;
}

std::unique_ptr<DeclStmt>
Parser::parseVariableDeclarationStatement(TypePtr &&LookAheadArrayType) {
  // This does not parse multi variable declaration statements starting directly
  // with
  // `(`, they are parsed in parseSimpleStatement, because they are hard to
  // distinguish from tuple expressions.
  const SourceLocation Begin = Tok.getLocation();
  std::vector<std::unique_ptr<VarDeclBase>> Variables;
  std::unique_ptr<Expr> Value;
  if (!LookAheadArrayType && Tok.is(tok::kw_var) &&
      NextToken().is(tok::l_paren)) {
    // [0.4.20] The var keyword has been deprecated for security reasons.
    // https://github.com/ethereum/solidity/releases/tag/v0.4.20
    Diag(diag::err_unimplemented_token) << tok::kw_var;
    return nullptr;
  } else {
    VarDeclParserOptions Options;
    Options.AllowVar = false;
    Options.AllowLocationSpecifier = true;
    Variables.push_back(
        parseVariableDeclaration(Options, std::move(LookAheadArrayType)));
  }
  SourceLocation End = Variables.back()->getLocation().getEnd();
  if (TryConsumeToken(tok::equal)) {
    Value = Actions.CreateDummy(parseExpression());
    End = Value->getLocation().getEnd();
  }

  return std::make_unique<DeclStmt>(SourceRange(Begin, End),
                                    std::move(Variables), std::move(Value));
}

bool Parser::IndexAccessedPath::empty() const {
  if (!Indices.empty()) {
    assert(!(Path.empty() && ElementaryType) && "");
  }
  return Path.empty() && (ElementaryType == nullptr) && Indices.empty();
}

std::pair<Parser::LookAheadInfo, Parser::IndexAccessedPath>
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
    return std::make_pair(StatementType, IndexAccessedPath());
  default:
    break;
  }
  // At this point, we have 'Identifier "["' or 'Identifier "." Identifier' or
  // 'ElementoryTypeName "["'. We parse '(Identifier ("." Identifier)*
  // |ElementaryTypeName) ( "[" Expression "]" )*' until we can decide whether
  // to hand this over to ExpressionStatement or create a
  // VariableDeclarationStatement out of it.
  IndexAccessedPath Iap = parseIndexAccessedPath();

  if (Tok.isOneOf(tok::identifier, tok::kw_memory, tok::kw_storage,
                  tok::kw_calldata))
    return std::make_pair(LookAheadInfo::VariableDeclaration, std::move(Iap));
  else
    return std::make_pair(LookAheadInfo::Expression, std::move(Iap));
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
  if (Tok.isOneOf(tok::kw_mapping, tok::kw_function, tok::kw_var)) {
    return LookAheadInfo::VariableDeclaration;
  }

  bool MightBeTypeName = Tok.isElementaryTypeName() || Tok.is(tok::identifier);

  if (MightBeTypeName) {
    const Token &NextTok = NextToken();
    // So far we only allow ``address payable`` in variable declaration
    // statements and in no other kind of statement. This means, for example,
    // that we do not allow type expressions of the form
    // ``address payable;``.
    // If we want to change this in the future, we need to consider another
    // scanner token here.
    if (Tok.isElementaryTypeName() &&
        NextTok.isOneOf(tok::kw_pure, tok::kw_view, tok::kw_payable)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok.isOneOf(tok::raw_identifier, tok::identifier, tok::kw_memory,
                        tok::kw_storage, tok::kw_calldata)) {
      return LookAheadInfo::VariableDeclaration;
    }
    if (NextTok.isOneOf(tok::l_square, tok::period)) {
      return LookAheadInfo::IndexAccessStructure;
    }
  }
  return LookAheadInfo::Expression;
}

Parser::IndexAccessedPath Parser::parseIndexAccessedPath() {
  IndexAccessedPath Iap;
  if (Tok.isAnyIdentifier()) {
    do {
      Iap.Path.emplace_back(Tok);
      ConsumeToken(); // identifier
    } while (TryConsumeToken(tok::period));
  } else {
    Iap.ElementaryType = parseTypeName(false);
  }

  while (Tok.is(tok::l_square)) {
    ConsumeBracket();
    auto E = parseExpression();
    Iap.Indices.emplace_back(std::move(E), Tok.getEndLoc());
    if (ExpectAndConsume(tok::r_square)) {
      break;
    }
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
    // TODO: UserDefinedTypeName
    // T = UserDefinedTypeName with Path
  }
  for (auto &Length : Iap.Indices) {
    if (const auto *NL =
            dynamic_cast<const NumberLiteral *>(Length.first.get())) {
      T = std::make_shared<ArrayType>(std::move(T), NL->getValue(),
                                      parseDataLocation());
    } else {
      Diag(diag::err_expected) << tok::numeric_constant;
      return nullptr;
    }
  }
  return T;
}

// TODO: IAP relative function
std::unique_ptr<Expr>
Parser::expressionFromIndexAccessStructure(Parser::IndexAccessedPath &Iap) {
  if (Iap.empty()) {
    return nullptr;
  }
  std::unique_ptr<Expr> Expression = Actions.CreateIdentifier(Iap.Path.front());
  if (!Expression) {
    return nullptr;
  }
  for (size_t i = 1; i < Iap.Path.size(); ++i) {
    Expression =
        Actions.CreateMemberExpr(std::move(Expression), std::move(Iap.Path[i]));
    if (!Expression) {
      return nullptr;
    }
  }

  for (auto &Index : Iap.Indices) {
    Expression = Actions.CreateIndexAccess(
        std::move(Index.second), std::move(Expression), std::move(Index.first));
    if (!Expression) {
      return nullptr;
    }
  }
  return Expression;
}

std::unique_ptr<Expr>
Parser::parseExpression(std::unique_ptr<Expr> &&PartiallyParsedExpression) {
  const SourceLocation Begin =
      PartiallyParsedExpression
          ? PartiallyParsedExpression->getLocation().getBegin()
          : Tok.getLocation();
  std::unique_ptr<Expr> Expression =
      parseBinaryExpression(4, std::move(PartiallyParsedExpression));

  if (tok::equal <= Tok.getKind() && Tok.getKind() <= tok::percentequal) {
    const BinaryOperatorKind Op = token2bop(Tok);
    ConsumeToken();
    std::unique_ptr<Expr> RightHandSide = parseExpression();
    const SourceLocation End = RightHandSide->getLocation().getEnd();
    return Actions.CreateBinOp(SourceRange(Begin, End), Op,
                               std::move(Expression), std::move(RightHandSide));
  }

  if (TryConsumeToken(tok::question)) {
    std::unique_ptr<Expr> trueExpression = parseExpression();
    if (ExpectAndConsume(tok::colon)) {
      return nullptr;
    }
    std::unique_ptr<Expr> falseExpression = parseExpression();
    // TODO: Create ConditionExpression
    return nullptr;
  }

  return Expression;
}

std::unique_ptr<Expr> Parser::parseBinaryExpression(
    int MinPrecedence, std::unique_ptr<Expr> &&PartiallyParsedExpression) {
  const SourceLocation Begin =
      PartiallyParsedExpression
          ? PartiallyParsedExpression->getLocation().getBegin()
          : Tok.getLocation();
  std::unique_ptr<Expr> Expression =
      parseUnaryExpression(std::move(PartiallyParsedExpression));
  int Precedence = static_cast<int>(getBinOpPrecedence(Tok.getKind()));
  for (; Precedence >= MinPrecedence; --Precedence) {
    while (getBinOpPrecedence(Tok.getKind()) == Precedence) {
      const BinaryOperatorKind Op = token2bop(Tok);
      ConsumeToken(); // binary op
      std::unique_ptr<Expr> RightHandSide =
          parseBinaryExpression(Precedence + 1);
      const SourceLocation End = RightHandSide->getLocation().getEnd();
      Expression =
          Actions.CreateBinOp(SourceRange(Begin, End), Op,
                              std::move(Expression), std::move(RightHandSide));
    }
  }
  return Expression;
}

std::unique_ptr<Expr> Parser::parseUnaryExpression(
    std::unique_ptr<Expr> &&PartiallyParsedExpression) {
  const SourceLocation Begin =
      PartiallyParsedExpression
          ? PartiallyParsedExpression->getLocation().getBegin()
          : Tok.getLocation();
  UnaryOperatorKind Op = token2uop(Tok);

  if (!PartiallyParsedExpression && Tok.isUnaryOp()) {
    ConsumeToken(); // pre '++' or '--'
    std::unique_ptr<Expr> SubExps = parseUnaryExpression();
    const SourceLocation End = SubExps->getLocation().getEnd();
    return std::make_unique<UnaryOperator>(SourceRange(Begin, End),
                                           std::move(SubExps), Op);
  } else {
    // potential postfix expression
    std::unique_ptr<Expr> SubExps =
        parseLeftHandSideExpression(std::move(PartiallyParsedExpression));
    Op = token2uop(Tok, false);
    if (!(Op == UnaryOperatorKind::UO_PostInc ||
          Op == UnaryOperatorKind::UO_PostDec))
      return SubExps;
    const SourceLocation End = Tok.getEndLoc();
    ConsumeToken(); // post '++' or '--'
    return std::make_unique<UnaryOperator>(SourceRange(Begin, End),
                                           std::move(SubExps), Op);
  }
}

std::unique_ptr<Expr> Parser::parseLeftHandSideExpression(
    std::unique_ptr<Expr> &&PartiallyParsedExpression) {
  const SourceLocation Begin =
      PartiallyParsedExpression
          ? PartiallyParsedExpression->getLocation().getBegin()
          : Tok.getLocation();
  std::unique_ptr<Expr> Expression;
  if (PartiallyParsedExpression) {
    Expression = std::move(PartiallyParsedExpression);
  } else if (TryConsumeToken(tok::kw_new)) {
    TypePtr typeName = parseTypeName(false);
    // [AST] create NewExpression
  } else {
    Expression = parsePrimaryExpression();
  }

  while (true) {
    switch (Tok.getKind()) {
    case tok::l_square: {
      ConsumeBracket(); // '['
      std::unique_ptr<Expr> Index;
      if (Tok.isNot(tok::r_square)) {
        Index = parseExpression();
      }
      if (ExpectAndConsume(tok::r_square)) {
        return nullptr;
      }
      Expression = Actions.CreateIndexAccess(
          Tok.getEndLoc(), std::move(Expression), std::move(Index));
      break;
    }
    case tok::period: {
      ConsumeToken(); // '.'
      if (!Tok.isAnyIdentifier()) {
        Diag(diag::err_expected) << tok::identifier;
      }
      Expression = Actions.CreateMemberExpr(std::move(Expression), Tok);
      if (!Expression) {
        return nullptr;
      }
      ConsumeToken(); // identifier
      break;
    }
    case tok::l_paren: {
      ConsumeParen(); // '('
      std::vector<std::unique_ptr<Expr>> Arguments;
      std::vector<llvm::StringRef> Names;
      std::tie(Arguments, Names) = parseFunctionCallArguments();
      const SourceLocation End = Tok.getEndLoc();
      if (ExpectAndConsume(tok::r_paren)) {
        return nullptr;
      }
      // TODO: Fix passs arguments' name fail.
      if (Names.empty())
        Expression =
            Actions.CreateCallExpr(SourceRange(Begin, End),
                                   std::move(Expression), std::move(Arguments));
      else
        Expression = Actions.CreateNamedCallExpr(
            SourceRange(Begin, End), std::move(Expression),
            std::move(Arguments), std::move(Names));
      break;
    }
    default:
      return Expression;
    }
  }
}

std::unique_ptr<Expr> Parser::parsePrimaryExpression() {
  const SourceLocation Begin = Tok.getLocation();
  std::unique_ptr<Expr> Expression;

  // Explicit Type Casting
  if (Tok.isElementaryTypeName() && NextToken().is(tok::l_paren)) {
    const auto TypeNameTok = Tok;
    ConsumeToken(); // elementary typename
    ConsumeParen(); // '('
    if (TypeNameTok.is(tok::kw_address)) {
      auto E = parseExpression();
      const SourceLocation End = E->getLocation().getEnd();
      Expression = std::make_unique<ExplicitCastExpr>(
          SourceRange(Begin, End), std::move(E), CastKind::TypeCast,
          std::make_shared<AddressType>(StateMutability::Payable));
    } else if (TypeNameTok.is(tok::kw_bytes)) {
      auto E = parseExpression();
      const SourceLocation End = E->getLocation().getEnd();
      Expression = std::make_unique<ExplicitCastExpr>(
          SourceRange(Begin, End), std::move(E), CastKind::TypeCast,
          std::make_shared<BytesType>());
    } else if (TypeNameTok.is(tok::kw_string)) {
      auto E = parseExpression();
      const SourceLocation End = E->getLocation().getEnd();
      Expression = std::make_unique<ExplicitCastExpr>(
          SourceRange(Begin, End), std::move(E), CastKind::TypeCast,
          std::make_shared<StringType>());
    } else {
      auto E = parseExpression();
      const SourceLocation End = E->getLocation().getEnd();
      Expression = std::make_unique<ExplicitCastExpr>(
          SourceRange(Begin, End), std::move(E), CastKind::IntegralCast,
          std::make_shared<IntegerType>(token2inttype(TypeNameTok)));
    }
    if (ExpectAndConsume(tok::r_paren)) {
      return nullptr;
    }
    return Expression;
  }

  const auto Kind = Tok.getKind();
  switch (Kind) {
  case tok::kw_true:
    Expression = std::make_unique<BooleanLiteral>(Tok, true);
    ConsumeToken(); // 'true'
    break;
  case tok::kw_false:
    Expression = std::make_unique<BooleanLiteral>(Tok, false);
    ConsumeToken(); // 'false'
    break;
  case tok::numeric_constant: {
    llvm::StringRef NumValue(Tok.getLiteralData(), Tok.getLength());
    const bool HasUnit =
        NextToken().isOneOf(tok::kw_wei, tok::kw_szabo, tok::kw_finney,
                            tok::kw_ether, tok::kw_seconds, tok::kw_minutes,
                            tok::kw_hours, tok::kw_days, tok::kw_weeks);
    if (!HasUnit) {
      const auto [Signed, Value] = numericParse(NumValue);
      Expression =
          std::make_unique<NumberLiteral>(Tok.getRange(), Signed, Value);
      ConsumeToken(); // numeric constant
    } else {
      const auto [Signed, Value] =
          numericParse(NumValue, token2UnitMultiplier(NextToken()));
      Expression = std::make_unique<NumberLiteral>(
          SourceRange(Tok.getLocation(), NextToken().getEndLoc()), Signed,
          Value);
      ConsumeToken(); // numeric constant
      ConsumeToken(); // unit keyword
    }
    break;
  }
  case tok::string_literal: {
    llvm::StringRef StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = std::make_unique<StringLiteral>(Tok, stringUnquote(StrValue));
    ConsumeStringToken(); // string literal
    break;
  }
  case tok::hex_string_literal: {
    llvm::StringRef StrValue(Tok.getLiteralData(), Tok.getLength());
    Expression = std::make_unique<StringLiteral>(Tok, hexUnquote(StrValue));
    ConsumeStringToken(); // hex string literal
    break;
  }
  case tok::identifier: {
    Expression = Actions.CreateIdentifier(Tok);
    ConsumeToken(); // identifier
    break;
  }
  case tok::kw_type:
    // TODO: Type expression is globally-avariable function
    Diag(diag::err_unimplemented_token) << tok::kw_type;
    return nullptr;
  case tok::l_paren:
  case tok::l_square: {
    // TODO: Tuple case
    //
    // Tuple/parenthesized expression or inline array/bracketed expression.
    // Special cases: ()/[] is empty tuple/array type, (x) is not a real tuple,
    // (x,) is one-dimensional tuple, elements in arrays cannot be left out,
    // only in tuples.
    std::vector<ExprPtr> Comps;
    const SourceLocation Begin = Tok.getLocation();
    const tok::TokenKind OppositeKind =
        (Kind == tok::l_paren ? tok::r_paren : tok::r_square);
    bool IsArray = (Kind == tok::l_square);
    ConsumeAnyToken(); // '[' or '('

    if (OppositeKind != Tok.getKind()) {
      while (true) {
        if (Tok.getKind() != tok::comma && Tok.getKind() != OppositeKind) {
          Comps.emplace_back(parseExpression());
        } else if (IsArray) {
          // Expected expression (inline array elements cannot be omitted).
          Diag(diag::err_unimplemented_inline_array);
        } else {
          Comps.emplace_back(ExprPtr());
        }

        if (Tok.getKind() == OppositeKind) {
          break;
        }

        ExpectAndConsume(tok::comma);
      }
    }
    const SourceLocation End = Tok.getEndLoc();
    ExpectAndConsume(OppositeKind);
    if (!IsArray && Comps.size() == 1) { // something like (e) is not a tuple
      Expression = std::make_unique<ParenExpr>(SourceRange(Begin, End),
                                               std::move(Comps.back()));
    } else {
      for (auto &comp : Comps) {
        if (comp)
          comp = Actions.CreateDummy(std::move(comp));
      }
      Expression = std::make_unique<TupleExpr>(SourceRange(Begin, End),
                                               std::move(Comps), IsArray);
    }
    break;
  }
  default:
    // TODO: Type MxN case
    Diag(diag::err_unimplemented_token) << Tok.getKind();
    ConsumeToken();
    return nullptr;
  }
  return Expression;
}

std::vector<std::unique_ptr<Expr>> Parser::parseFunctionCallListArguments() {
  std::vector<std::unique_ptr<Expr>> Arguments;
  if (Tok.isNot(tok::r_paren)) {
    Arguments.push_back(parseExpression());
    while (Tok.isNot(tok::r_paren)) {
      if (ExpectAndConsume(tok::comma)) {
        return Arguments;
      }
      Arguments.push_back(parseExpression());
    }
  }
  return Arguments;
}

std::pair<std::vector<std::unique_ptr<Expr>>, std::vector<llvm::StringRef>>
Parser::parseFunctionCallArguments() {
  std::pair<std::vector<std::unique_ptr<Expr>>, std::vector<llvm::StringRef>>
      Ret;
  if (Tok.is(tok::l_brace)) {
    ConsumeBrace();
    bool First = true;
    while (Tok.isNot(tok::r_brace)) {
      if (!First) {
        if (ExpectAndConsume(tok::comma)) {
          return Ret;
        }
      }

      if (Tok.isNot(tok::identifier)) {
        Diag(diag::err_expected) << tok::identifier;
        return Ret;
      }
      Ret.second.emplace_back(Tok.getIdentifierInfo()->getName());
      ConsumeToken(); // identifier
      if (Tok.isNot(tok::colon)) {
        Diag(diag::err_expected) << tok::colon;
        return Ret;
      }
      ConsumeToken(); // ':'
      Ret.first.emplace_back(parseExpression());

      if (Tok.is(tok::comma) && NextToken().is(tok::r_brace)) {
        Diag(diag::err_trailing_comma);
        ConsumeToken(); // ','
      }
      First = false;
    }
    ConsumeBrace(); // '}'
  } else {
    Ret.first = parseFunctionCallListArguments();
  }
  return Ret;
}

void Parser::EnterScope(unsigned ScopeFlags) { Actions.PushScope(ScopeFlags); }

void Parser::ExitScope() { Actions.PopScope(); }

bool Parser::ExpectAndConsumeSemi(unsigned DiagID) {
  if (TryConsumeToken(tok::semi))
    return false;

  if ((Tok.is(tok::r_paren) || Tok.is(tok::r_square)) &&
      NextToken().is(tok::semi)) {
    Diag(diag::err_extraneous_token_before_semi);
    ConsumeAnyToken(); // The ')' or ']'.
    ConsumeToken();    // The ';'.
    return false;
  }

  return ExpectAndConsume(tok::semi, DiagID);
}

bool Parser::ExpectAndConsume(tok::TokenKind ExpectedTok, unsigned DiagID,
                              llvm::StringRef Msg) {
  if (Tok.is(ExpectedTok)) {
    ConsumeAnyToken();
    return false;
  }

  // TODO: Detect common single-character typos and resume.
  DiagnosticBuilder DB = Diag(DiagID);
  if (DiagID == diag::err_expected)
    DB << ExpectedTok;
  else if (DiagID == diag::err_expected_after)
    DB << Msg << ExpectedTok;
  else
    DB << Msg;

  return true;
}

DiagnosticBuilder Parser::Diag(SourceLocation Loc, unsigned DiagID) {
  return Diags.Report(Loc, DiagID);
}

} // namespace soll
