// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/AST.h"
#include "soll/Sema/Sema.h"

namespace soll {

std::unique_ptr<AsmIdentifier> Sema::CreateAsmIdentifier(const Token &Tok,
                                                         bool IsCall) {
  static const llvm::StringMap<AsmIdentifier::SpecialIdentifier> SpecialLookup{
      /// Mark comment below logic identifiers because
      /// currently solidity only generate them for bitwise op.
      /// (meet solidity inline assembly usage)
      /// logical
      // {"not", AsmIdentifier::SpecialIdentifier::not_},
      // {"and", AsmIdentifier::SpecialIdentifier::and_},
      // {"or", AsmIdentifier::SpecialIdentifier::or_},
      // {"xor", AsmIdentifier::SpecialIdentifier::xor_},
      /// arithmetic
      {"addu256", AsmIdentifier::SpecialIdentifier::addu256},
      {"add", AsmIdentifier::SpecialIdentifier::addu256},
      {"subu256", AsmIdentifier::SpecialIdentifier::subu256},
      {"sub", AsmIdentifier::SpecialIdentifier::subu256},
      {"mulu256", AsmIdentifier::SpecialIdentifier::mulu256},
      {"mul", AsmIdentifier::SpecialIdentifier::mulu256},
      {"divu256", AsmIdentifier::SpecialIdentifier::divu256},
      {"div", AsmIdentifier::SpecialIdentifier::divu256},
      {"divs256", AsmIdentifier::SpecialIdentifier::divs256},
      {"sdiv", AsmIdentifier::SpecialIdentifier::divs256},
      {"modu256", AsmIdentifier::SpecialIdentifier::modu256},
      {"mod", AsmIdentifier::SpecialIdentifier::modu256},
      {"mods256", AsmIdentifier::SpecialIdentifier::mods256},
      {"smod", AsmIdentifier::SpecialIdentifier::mods256},
      {"signextendu256", AsmIdentifier::SpecialIdentifier::signextendu256},
      {"signextend", AsmIdentifier::SpecialIdentifier::signextendu256},
      {"expu256", AsmIdentifier::SpecialIdentifier::expu256},
      {"exp", AsmIdentifier::SpecialIdentifier::expu256},
      {"addmodu256", AsmIdentifier::SpecialIdentifier::addmodu256},
      {"addmod", AsmIdentifier::SpecialIdentifier::addmodu256},
      {"mulmodu256", AsmIdentifier::SpecialIdentifier::mulmodu256},
      {"mulmod", AsmIdentifier::SpecialIdentifier::mulmodu256},
      {"ltu256", AsmIdentifier::SpecialIdentifier::ltu256},
      {"lt", AsmIdentifier::SpecialIdentifier::ltu256},
      {"gtu256", AsmIdentifier::SpecialIdentifier::gtu256},
      {"gt", AsmIdentifier::SpecialIdentifier::gtu256},
      {"lts256", AsmIdentifier::SpecialIdentifier::lts256},
      {"slt", AsmIdentifier::SpecialIdentifier::lts256},
      {"gts256", AsmIdentifier::SpecialIdentifier::gts256},
      {"sgt", AsmIdentifier::SpecialIdentifier::gts256},
      {"equ256", AsmIdentifier::SpecialIdentifier::equ256},
      {"eq", AsmIdentifier::SpecialIdentifier::equ256},
      {"iszerou256", AsmIdentifier::SpecialIdentifier::iszerou256},
      {"iszero", AsmIdentifier::SpecialIdentifier::iszerou256},
      {"notu256", AsmIdentifier::SpecialIdentifier::notu256},
      {"not", AsmIdentifier::SpecialIdentifier::notu256},
      {"andu256", AsmIdentifier::SpecialIdentifier::andu256},
      {"and", AsmIdentifier::SpecialIdentifier::andu256},
      {"oru256", AsmIdentifier::SpecialIdentifier::oru256},
      {"or", AsmIdentifier::SpecialIdentifier::oru256},
      {"xoru256", AsmIdentifier::SpecialIdentifier::xoru256},
      {"xor", AsmIdentifier::SpecialIdentifier::xoru256},
      {"shlu256", AsmIdentifier::SpecialIdentifier::shlu256},
      {"shl", AsmIdentifier::SpecialIdentifier::shl},
      {"shru256", AsmIdentifier::SpecialIdentifier::shru256},
      {"shr", AsmIdentifier::SpecialIdentifier::shr},
      {"sars256", AsmIdentifier::SpecialIdentifier::sars256},
      {"sar", AsmIdentifier::SpecialIdentifier::sar},
      {"byte", AsmIdentifier::SpecialIdentifier::byte},
      // TODO: implement the rest identifiers
      /// memory and storage
      {"mload", AsmIdentifier::SpecialIdentifier::mload},
      {"mstore", AsmIdentifier::SpecialIdentifier::mstore},
      {"mstore8", AsmIdentifier::SpecialIdentifier::mstore8},
      {"msize", AsmIdentifier::SpecialIdentifier::msize},
      {"sload", AsmIdentifier::SpecialIdentifier::sload},
      {"sstore", AsmIdentifier::SpecialIdentifier::sstore},
      /// execution control
      {"stop", AsmIdentifier::SpecialIdentifier::stop},
      {"return", AsmIdentifier::SpecialIdentifier::return_},
      {"revert", AsmIdentifier::SpecialIdentifier::revert},
      {"selfdestruct", AsmIdentifier::SpecialIdentifier::selfdestruct},
      {"log0", AsmIdentifier::SpecialIdentifier::log0},
      {"log1", AsmIdentifier::SpecialIdentifier::log1},
      {"log2", AsmIdentifier::SpecialIdentifier::log2},
      {"log3", AsmIdentifier::SpecialIdentifier::log3},
      {"log4", AsmIdentifier::SpecialIdentifier::log4},
      /// state
      {"blockcoinbase", AsmIdentifier::SpecialIdentifier::blockcoinbase},
      {"coinbase", AsmIdentifier::SpecialIdentifier::blockcoinbase},
      {"blockdifficulty", AsmIdentifier::SpecialIdentifier::blockdifficulty},
      {"difficulty", AsmIdentifier::SpecialIdentifier::blockdifficulty},
      {"blockgaslimit", AsmIdentifier::SpecialIdentifier::blockgaslimit},
      {"blockhash", AsmIdentifier::SpecialIdentifier::blockhash},
      {"gaslimit", AsmIdentifier::SpecialIdentifier::blockgaslimit},
      {"blocknumber", AsmIdentifier::SpecialIdentifier::blocknumber},
      {"number", AsmIdentifier::SpecialIdentifier::blocknumber},
      {"blocktimestamp", AsmIdentifier::SpecialIdentifier::blocktimestamp},
      {"timestamp", AsmIdentifier::SpecialIdentifier::blocktimestamp},
      {"txorigin", AsmIdentifier::SpecialIdentifier::txorigin},
      {"origin", AsmIdentifier::SpecialIdentifier::txorigin},
      {"txgasprice", AsmIdentifier::SpecialIdentifier::txgasprice},
      {"gasprice", AsmIdentifier::SpecialIdentifier::txgasprice},
      {"gasleft", AsmIdentifier::SpecialIdentifier::gasleft},
      {"gas", AsmIdentifier::SpecialIdentifier::gasleft},
      {"balance", AsmIdentifier::SpecialIdentifier::balance},
      {"selfbalance", AsmIdentifier::SpecialIdentifier::selfbalance},
      {"create", AsmIdentifier::SpecialIdentifier::create},
      {"create2", AsmIdentifier::SpecialIdentifier::create2},
      {"call", AsmIdentifier::SpecialIdentifier::call},
      {"callcode", AsmIdentifier::SpecialIdentifier::callcode},
      {"delegatecall", AsmIdentifier::SpecialIdentifier::delegatecall},
      {"staticcall", AsmIdentifier::SpecialIdentifier::staticcall},
      {"caller", AsmIdentifier::SpecialIdentifier::caller},
      {"callvalue", AsmIdentifier::SpecialIdentifier::callvalue},
      {"calldataload", AsmIdentifier::SpecialIdentifier::calldataload},
      {"calldatasize", AsmIdentifier::SpecialIdentifier::calldatasize},
      {"calldatacopy", AsmIdentifier::SpecialIdentifier::calldatacopy},
      {"codesize", AsmIdentifier::SpecialIdentifier::codesize},
      {"codecopy", AsmIdentifier::SpecialIdentifier::codecopy},
      {"extcodecopy", AsmIdentifier::SpecialIdentifier::extcodecopy},
      {"extcodesize", AsmIdentifier::SpecialIdentifier::extcodesize},
      {"extcodehash", AsmIdentifier::SpecialIdentifier::extcodehash},
      {"address", AsmIdentifier::SpecialIdentifier::address},
      {"returndatasize", AsmIdentifier::SpecialIdentifier::returndatasize},
      {"returndatacopy", AsmIdentifier::SpecialIdentifier::returndatacopy},
      /// object
      {"datasize", AsmIdentifier::SpecialIdentifier::datasize},
      {"dataoffset", AsmIdentifier::SpecialIdentifier::dataoffset},
      {"datacopy", AsmIdentifier::SpecialIdentifier::datacopy},
      /// misc
      {"keccak256", AsmIdentifier::SpecialIdentifier::keccak256},
      {"invalid", AsmIdentifier::SpecialIdentifier::invalid},
      {"pop", AsmIdentifier::SpecialIdentifier::pop}};
  llvm::StringRef Name = Tok.getIdentifierInfo()->getName();
  if (auto Iter = SpecialLookup.find(Name); Iter != SpecialLookup.end()) {
    TypePtr Ty;
    switch (Iter->second) {
    case AsmIdentifier::SpecialIdentifier::not_: ///< (bool) -> bool
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::and_: ///< (bool, bool) -> bool
    case AsmIdentifier::SpecialIdentifier::or_:
    case AsmIdentifier::SpecialIdentifier::xor_:
      Ty = std::make_shared<BooleanType>();
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::addu256: ///< (u256, u256) -> u256
    case AsmIdentifier::SpecialIdentifier::subu256:
    case AsmIdentifier::SpecialIdentifier::mulu256:
    case AsmIdentifier::SpecialIdentifier::divu256:
    case AsmIdentifier::SpecialIdentifier::modu256:
    case AsmIdentifier::SpecialIdentifier::expu256:
    case AsmIdentifier::SpecialIdentifier::signextendu256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::divs256: ///< (s256, s256) -> s256
    case AsmIdentifier::SpecialIdentifier::mods256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::addmodu256: ///< (u256, u256, u256)
    case AsmIdentifier::SpecialIdentifier::mulmodu256: ///< -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::ltu256: ///< (u256, u256) -> bool
    case AsmIdentifier::SpecialIdentifier::gtu256:
    case AsmIdentifier::SpecialIdentifier::equ256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::lts256: ///< (s256, s256) -> bool
    case AsmIdentifier::SpecialIdentifier::gts256:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::I256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::iszerou256: ///< (u256) -> bool
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)},
          std::vector<TypePtr>{std::make_shared<BooleanType>()});
      break;
    case AsmIdentifier::SpecialIdentifier::notu256: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::andu256: ///< (u256, u256) -> u256
    case AsmIdentifier::SpecialIdentifier::oru256:
    case AsmIdentifier::SpecialIdentifier::xoru256:
    case AsmIdentifier::SpecialIdentifier::shlu256:
    case AsmIdentifier::SpecialIdentifier::shl:
    case AsmIdentifier::SpecialIdentifier::shru256:
    case AsmIdentifier::SpecialIdentifier::shr:
    case AsmIdentifier::SpecialIdentifier::sars256:
    case AsmIdentifier::SpecialIdentifier::sar:
    case AsmIdentifier::SpecialIdentifier::byte:
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::mload: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::mstore: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::mstore8: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::msize: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::extcodehash:
    case AsmIdentifier::SpecialIdentifier::selfdestruct:
    case AsmIdentifier::SpecialIdentifier::sload: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::sstore: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::pop: ///< (u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::invalid:
    case AsmIdentifier::SpecialIdentifier::stop: ///< () -> void
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::return_: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::revert: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::log0: ///< (u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::log1: ///< (u256, u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::log2: ///< (u256, u256, u256, u256)
                                                 ///< -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::log3: ///< (u256, u256, u256, u256,
                                                 ///< u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty, Ty, Ty, Ty}, std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::log4: ///< (u256, u256, u256, u256,
                                                 ///< u256, u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty, Ty, Ty, Ty, Ty}, std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::blockcoinbase: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::blockdifficulty: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::blockgaslimit: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::blocknumber: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::blocktimestamp: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::txorigin: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::txgasprice: ///< () -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::selfbalance:
    case AsmIdentifier::SpecialIdentifier::address:
    case AsmIdentifier::SpecialIdentifier::codesize:
    case AsmIdentifier::SpecialIdentifier::returndatasize:
    case AsmIdentifier::SpecialIdentifier::gasleft: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::create: ///< (u256, u256, u256) ->
                                                   ///< u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::create2: ///< (u256, u256, u256,
                                                    ///< u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::call:
    case AsmIdentifier::SpecialIdentifier::callcode:
    case AsmIdentifier::SpecialIdentifier::delegatecall:
    case AsmIdentifier::SpecialIdentifier::
        staticcall: ///< (u256, u256, u256, u256, u256, u256, u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{Ty, Ty, Ty, Ty, Ty, Ty, Ty},
          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::caller: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::callvalue: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::balance:
    case AsmIdentifier::SpecialIdentifier::extcodesize:
    case AsmIdentifier::SpecialIdentifier::blockhash:
    case AsmIdentifier::SpecialIdentifier::calldataload: ///< (u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    case AsmIdentifier::SpecialIdentifier::calldatasize: ///< () -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::calldatacopy: ///< (u256, u256, u256)
                                                         ///< -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::datasize: ///< (string) -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<StringType>()},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::dataoffset: ///< (string) -> u256
      Ty = std::make_shared<FunctionType>(
          std::vector<TypePtr>{std::make_shared<StringType>()},
          std::vector<TypePtr>{
              std::make_shared<IntegerType>(IntegerType::IntKind::U256)});
      break;
    case AsmIdentifier::SpecialIdentifier::datacopy:
    case AsmIdentifier::SpecialIdentifier::returndatacopy:
    case AsmIdentifier::SpecialIdentifier::codecopy: ///< (u256, u256, u256) ->
                                                     ///< void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::extcodecopy: ///< (u256, u256, u256,
                                                        ///< u256) -> void
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty, Ty, Ty},
                                          std::vector<TypePtr>{});
      break;
    case AsmIdentifier::SpecialIdentifier::keccak256: ///< (u256, u256) -> u256
      Ty = std::make_shared<IntegerType>(IntegerType::IntKind::U256);
      Ty = std::make_shared<FunctionType>(std::vector<TypePtr>{Ty, Ty},
                                          std::vector<TypePtr>{Ty});
      break;
    // TODO: implement the rest identifiers
    default:
      assert(false && "unknown special identifier");
      __builtin_unreachable();
    }
    return std::make_unique<AsmIdentifier>(Tok, Iter->second, std::move(Ty));
  }
  Decl *D = lookupName(Name);
  if (D == nullptr) {
    auto Unresolved = std::make_unique<AsmIdentifier>(Tok);
    if (IsCall) {
      CurrentScope()->addUnresolved(Unresolved.get());
    } else {
      CurrentScope()->addUnresolvedExternal(Unresolved.get());
    }
    return Unresolved;
  }
  return std::make_unique<AsmIdentifier>(Tok, D);
}

std::unique_ptr<Expr> Sema::CreateAsmCallExpr(SourceRange L, ExprPtr &&Callee,
                                              std::vector<ExprPtr> &&Args) {
  for (ExprPtr &Arg : Args) {
    Arg = CreateDummy(std::move(Arg));
  }

  std::unique_ptr<Expr> CE = nullptr;
  if (auto I = dynamic_cast<AsmIdentifier *>(Callee.get())) {
    if (I->isSpecialIdentifier()) {
      // TODO: handle invalid FunctionType.
      FunctionType *FTy = dynamic_cast<FunctionType *>(Callee->getType().get());
      // TODO: handle the case that number of return types > 1.
      TypePtr ReturnTy;
      if (!FTy->getReturnTypes().empty()) {
        ReturnTy = FTy->getReturnTypes()[0];
      }
      CE = Sema::CreateAsmBuiltinCallExpr(L, *I, std::move(Args), ReturnTy);
      if (CE) {
        return CE;
      }
    }
  } else {
    assert(false && "callee is not AsmIdentifier");
    __builtin_unreachable();
  }

  return std::make_unique<CallExpr>(L, std::move(Callee), std::move(Args));
}

std::unique_ptr<Expr>
Sema::CreateAsmBuiltinCallExpr(SourceRange L, const AsmIdentifier &Callee,
                               std::vector<ExprPtr> &&Args, TypePtr ReturnTy) {
  switch (Callee.getSpecialIdentifier()) {
  case AsmIdentifier::SpecialIdentifier::not_:
    return std::make_unique<AsmUnaryOperator>(L, std::move(Args[0]),
                                              std::move(ReturnTy), UO_LNot);
  case AsmIdentifier::SpecialIdentifier::and_:
    return std::make_unique<AsmBinaryOperator>(L, std::move(Args[0]),
                                               std::move(Args[1]),
                                               std::move(ReturnTy), BO_LAnd);
  case AsmIdentifier::SpecialIdentifier::or_:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_LOr);
  case AsmIdentifier::SpecialIdentifier::xor_:
    return std::make_unique<AsmBinaryOperator>(L, std::move(Args[0]),
                                               std::move(Args[1]),
                                               std::move(ReturnTy), BO_LXor);
  case AsmIdentifier::SpecialIdentifier::addu256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Add);
  case AsmIdentifier::SpecialIdentifier::subu256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Sub);
  case AsmIdentifier::SpecialIdentifier::mulu256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Mul);
  case AsmIdentifier::SpecialIdentifier::divu256:
  case AsmIdentifier::SpecialIdentifier::divs256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Div);
  case AsmIdentifier::SpecialIdentifier::modu256:
  case AsmIdentifier::SpecialIdentifier::mods256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Rem);
  // TODO: case AsmIdentifier::SpecialIdentifier::signextendu256:
  case AsmIdentifier::SpecialIdentifier::expu256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Exp);
  case AsmIdentifier::SpecialIdentifier::ltu256:
  case AsmIdentifier::SpecialIdentifier::lts256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_LT);
  case AsmIdentifier::SpecialIdentifier::gtu256:
  case AsmIdentifier::SpecialIdentifier::gts256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_GT);
  case AsmIdentifier::SpecialIdentifier::equ256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_EQ);
  case AsmIdentifier::SpecialIdentifier::iszerou256:
    return std::make_unique<AsmUnaryOperator>(L, std::move(Args[0]),
                                              std::move(ReturnTy), UO_IsZero);
  case AsmIdentifier::SpecialIdentifier::notu256:
    return std::make_unique<AsmUnaryOperator>(L, std::move(Args[0]),
                                              std::move(ReturnTy), UO_Not);
  case AsmIdentifier::SpecialIdentifier::andu256:
    return std::make_unique<AsmBinaryOperator>(L, std::move(Args[0]),
                                               std::move(Args[1]),
                                               std::move(ReturnTy), BO_AsmAnd);
  case AsmIdentifier::SpecialIdentifier::oru256:
    return std::make_unique<AsmBinaryOperator>(L, std::move(Args[0]),
                                               std::move(Args[1]),
                                               std::move(ReturnTy), BO_AsmOr);
  case AsmIdentifier::SpecialIdentifier::xoru256:
    return std::make_unique<AsmBinaryOperator>(L, std::move(Args[0]),
                                               std::move(Args[1]),
                                               std::move(ReturnTy), BO_AsmXor);
  case AsmIdentifier::SpecialIdentifier::shlu256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Shl);
  case AsmIdentifier::SpecialIdentifier::shl:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[1]), std::move(Args[0]), std::move(ReturnTy), BO_Shl);
  case AsmIdentifier::SpecialIdentifier::shru256:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[0]), std::move(Args[1]), std::move(ReturnTy), BO_Shr);
  case AsmIdentifier::SpecialIdentifier::shr:
    return std::make_unique<AsmBinaryOperator>(
        L, std::move(Args[1]), std::move(Args[0]), std::move(ReturnTy), BO_Shr);
  // TODO: case AsmIdentifier::SpecialIdentifier::sars256:
  // TODO: case AsmIdentifier::SpecialIdentifier::sar:
  default: ///< treated as normal CallExpr
    break;
  }
  return nullptr;
}

} // namespace soll
