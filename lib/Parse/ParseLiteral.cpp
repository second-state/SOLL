// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/DiagnosticParse.h"
#include "soll/Parse/Parser.h"

namespace soll {

std::string Parser::stringUnquote(llvm::StringRef Quoted) {
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
      if (UcnLen != 0) {
        Diag(diag::err_ucn_escape_incomplete);
        continue;
      }

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
      if (HexLen != 0) {
        Diag(diag::err_hex_escape_incomplete);
        continue;
      }
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
      Diag(diag::err_unknown_escape) << ResultChar;
      break;
    }
    Result.push_back(ResultChar);
  }
  return Result;
}

std::string Parser::hexUnquote(llvm::StringRef Quoted) {
  const char *TokBegin = Quoted.data();
  const char *TokEnd = TokBegin + Quoted.size() - 1;
  assert(TokBegin[0] == 'h');
  assert(TokBegin[1] == 'e');
  assert(TokBegin[2] == 'x');
  assert(TokBegin[3] == '"' || TokBegin[3] == '\'');
  assert(TokEnd[0] == TokBegin[3]);
  if (Quoted.size() % 2 != 1) {
    Diag(diag::err_hex_escape_incomplete);
    return std::string();
  }
  std::string Result;
  for (std::size_t I = 4; I < Quoted.size() - 1; I += 2) {
    Result.push_back((llvm::hexDigitValue(Quoted[I]) << 4) |
                     llvm::hexDigitValue(Quoted[I + 1]));
  }
  return Result;
}

std::string Parser::simplifyIntegerLiteral(llvm::StringRef Literal) {
  std::string LiteralStr = Literal.str();
  LiteralStr.erase(remove(LiteralStr.begin(), LiteralStr.end(), '_'),
                   LiteralStr.end());

  // Hex format integer literal
  if (LiteralStr.size() > 2 && LiteralStr[0] == '0' && LiteralStr[1] == 'x')
    return LiteralStr;
  size_t EIdx = 0, DotIdx = 0;
  int ENum = 0, DotNum = 0;
  for (size_t I = 0; I < LiteralStr.size(); ++I) {
    if (LiteralStr[I] == 'e') {
      ++ENum;
      EIdx = I;
    }
    if (LiteralStr[I] == '.') {
      ++DotNum;
      DotIdx = I;
    }
  }

  /* Invalid cases: 1e2e, 3e, 1.1.1, 1e2.1 */
  if (ENum != 1 || LiteralStr.back() == 'e' || DotNum > 1 ||
      (DotNum == 1 && EIdx < DotIdx))
    return LiteralStr;

  std::string L = LiteralStr.substr(0, EIdx);
  std::string R = LiteralStr.substr(EIdx + 1);
  size_t Exponent = std::stoul(R);
  if (DotNum) {
    while (L.back() != '.' && Exponent) {
      std::swap(L[DotIdx], L[DotIdx + 1]);
      ++DotIdx;
      --Exponent;
    }
    if (L.back() == '.')
      L.pop_back();
  }
  while (Exponent--)
    L += '0';
  return L;
}

std::pair<bool, llvm::APInt> Parser::numericParse(llvm::StringRef Literal,
                                                  uint64_t Unit) {
  llvm::APInt Result = llvm::APInt::getNullValue(1);
  bool Signed = false;
  if (!Literal.empty()) {
    if (Literal.front() == '-') {
      Signed = true;
      Literal = Literal.substr(1);
    }
    auto LiteralStr = simplifyIntegerLiteral(Literal);
    if (llvm::StringRef(LiteralStr).getAsInteger(0, Result)) {
      Diag(diag::err_invalid_numeric_literal) << Literal;
    }
    if (Unit != 1) {
      Result = Result.zext(Result.getBitWidth() + 64);
      Result *= Unit;
    }
    unsigned BitWidth = Result.getActiveBits();
    unsigned TruncWidth = (BitWidth + 7) & ~7;
    if (TruncWidth < Result.getBitWidth()) {
      Result = Result.trunc(TruncWidth);
    }
  }
  return {Signed, Result};
}

} // namespace soll
