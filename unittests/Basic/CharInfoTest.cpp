// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/CharInfo.h"
#include "catch.hpp"

using namespace soll;

// Check various predicates.
TEST_CASE("isASCII", "[CharInfoTest]") {
  REQUIRE(isASCII('\0'));
  REQUIRE(isASCII('\n'));
  REQUIRE(isASCII(' '));
  REQUIRE(isASCII('a'));
  REQUIRE(isASCII('\x7f'));
  REQUIRE_FALSE(isASCII('\x80'));
  REQUIRE_FALSE(isASCII('\xc2'));
  REQUIRE_FALSE(isASCII('\xff'));
}

TEST_CASE("isIdentifierHead", "[CharInfoTest]") {
  REQUIRE(isIdentifierHead('a'));
  REQUIRE(isIdentifierHead('A'));
  REQUIRE(isIdentifierHead('z'));
  REQUIRE(isIdentifierHead('Z'));
  REQUIRE(isIdentifierHead('_'));
  REQUIRE(isIdentifierHead('$'));

  REQUIRE_FALSE(isIdentifierHead('0'));
  REQUIRE_FALSE(isIdentifierHead('.'));
  REQUIRE_FALSE(isIdentifierHead('`'));
  REQUIRE_FALSE(isIdentifierHead('\0'));

  REQUIRE_FALSE(isIdentifierHead('\x80'));
  REQUIRE_FALSE(isIdentifierHead('\xc2'));
  REQUIRE_FALSE(isIdentifierHead('\xff'));
}

TEST_CASE("isIdentifierBody", "[CharInfoTest]") {
  REQUIRE(isIdentifierBody('a'));
  REQUIRE(isIdentifierBody('A'));
  REQUIRE(isIdentifierBody('z'));
  REQUIRE(isIdentifierBody('Z'));
  REQUIRE(isIdentifierBody('_'));
  REQUIRE(isIdentifierBody('$'));

  REQUIRE(isIdentifierBody('0'));
  REQUIRE_FALSE(isIdentifierBody('.'));
  REQUIRE_FALSE(isIdentifierBody('`'));
  REQUIRE_FALSE(isIdentifierBody('\0'));

  REQUIRE_FALSE(isIdentifierBody('\x80'));
  REQUIRE_FALSE(isIdentifierBody('\xc2'));
  REQUIRE_FALSE(isIdentifierBody('\xff'));
}

TEST_CASE("isHorizontalWhitespace", "[CharInfoTest]") {
  REQUIRE_FALSE(isHorizontalWhitespace('a'));
  REQUIRE_FALSE(isHorizontalWhitespace('_'));
  REQUIRE_FALSE(isHorizontalWhitespace('0'));
  REQUIRE_FALSE(isHorizontalWhitespace('.'));
  REQUIRE_FALSE(isHorizontalWhitespace('`'));
  REQUIRE_FALSE(isHorizontalWhitespace('\0'));
  REQUIRE_FALSE(isHorizontalWhitespace('\x7f'));

  REQUIRE(isHorizontalWhitespace(' '));
  REQUIRE(isHorizontalWhitespace('\t'));
  REQUIRE(isHorizontalWhitespace('\f')); // ??
  REQUIRE(isHorizontalWhitespace('\v')); // ??

  REQUIRE_FALSE(isHorizontalWhitespace('\n'));
  REQUIRE_FALSE(isHorizontalWhitespace('\r'));

  REQUIRE_FALSE(isHorizontalWhitespace('\x80'));
  REQUIRE_FALSE(isHorizontalWhitespace('\xc2'));
  REQUIRE_FALSE(isHorizontalWhitespace('\xff'));
}

TEST_CASE("isVerticalWhitespace", "[CharInfoTest]") {
  REQUIRE_FALSE(isVerticalWhitespace('a'));
  REQUIRE_FALSE(isVerticalWhitespace('_'));
  REQUIRE_FALSE(isVerticalWhitespace('0'));
  REQUIRE_FALSE(isVerticalWhitespace('.'));
  REQUIRE_FALSE(isVerticalWhitespace('`'));
  REQUIRE_FALSE(isVerticalWhitespace('\0'));
  REQUIRE_FALSE(isVerticalWhitespace('\x7f'));

  REQUIRE_FALSE(isVerticalWhitespace(' '));
  REQUIRE_FALSE(isVerticalWhitespace('\t'));
  REQUIRE_FALSE(isVerticalWhitespace('\f')); // ??
  REQUIRE_FALSE(isVerticalWhitespace('\v')); // ??

  REQUIRE(isVerticalWhitespace('\n'));
  REQUIRE(isVerticalWhitespace('\r'));

  REQUIRE_FALSE(isVerticalWhitespace('\x80'));
  REQUIRE_FALSE(isVerticalWhitespace('\xc2'));
  REQUIRE_FALSE(isVerticalWhitespace('\xff'));
}

TEST_CASE("isWhitespace", "[CharInfoTest]") {
  REQUIRE_FALSE(isWhitespace('a'));
  REQUIRE_FALSE(isWhitespace('_'));
  REQUIRE_FALSE(isWhitespace('0'));
  REQUIRE_FALSE(isWhitespace('.'));
  REQUIRE_FALSE(isWhitespace('`'));
  REQUIRE_FALSE(isWhitespace('\0'));
  REQUIRE_FALSE(isWhitespace('\x7f'));

  REQUIRE(isWhitespace(' '));
  REQUIRE(isWhitespace('\t'));
  REQUIRE(isWhitespace('\f'));
  REQUIRE(isWhitespace('\v'));

  REQUIRE(isWhitespace('\n'));
  REQUIRE(isWhitespace('\r'));

  REQUIRE_FALSE(isWhitespace('\x80'));
  REQUIRE_FALSE(isWhitespace('\xc2'));
  REQUIRE_FALSE(isWhitespace('\xff'));
}

TEST_CASE("isDigit", "[CharInfoTest]") {
  REQUIRE(isDigit('0'));
  REQUIRE(isDigit('9'));

  REQUIRE_FALSE(isDigit('a'));
  REQUIRE_FALSE(isDigit('A'));

  REQUIRE_FALSE(isDigit('z'));
  REQUIRE_FALSE(isDigit('Z'));

  REQUIRE_FALSE(isDigit('.'));
  REQUIRE_FALSE(isDigit('_'));

  REQUIRE_FALSE(isDigit('/'));
  REQUIRE_FALSE(isDigit('\0'));

  REQUIRE_FALSE(isDigit('\x80'));
  REQUIRE_FALSE(isDigit('\xc2'));
  REQUIRE_FALSE(isDigit('\xff'));
}

TEST_CASE("isHexDigit", "[CharInfoTest]") {
  REQUIRE(isHexDigit('0'));
  REQUIRE(isHexDigit('9'));

  REQUIRE(isHexDigit('a'));
  REQUIRE(isHexDigit('A'));

  REQUIRE_FALSE(isHexDigit('z'));
  REQUIRE_FALSE(isHexDigit('Z'));

  REQUIRE_FALSE(isHexDigit('.'));
  REQUIRE_FALSE(isHexDigit('_'));

  REQUIRE_FALSE(isHexDigit('/'));
  REQUIRE_FALSE(isHexDigit('\0'));

  REQUIRE_FALSE(isHexDigit('\x80'));
  REQUIRE_FALSE(isHexDigit('\xc2'));
  REQUIRE_FALSE(isHexDigit('\xff'));
}

TEST_CASE("isLetter", "[CharInfoTest]") {
  REQUIRE_FALSE(isLetter('0'));
  REQUIRE_FALSE(isLetter('9'));

  REQUIRE(isLetter('a'));
  REQUIRE(isLetter('A'));

  REQUIRE(isLetter('z'));
  REQUIRE(isLetter('Z'));

  REQUIRE_FALSE(isLetter('.'));
  REQUIRE_FALSE(isLetter('_'));

  REQUIRE_FALSE(isLetter('/'));
  REQUIRE_FALSE(isLetter('('));
  REQUIRE_FALSE(isLetter('\0'));

  REQUIRE_FALSE(isLetter('\x80'));
  REQUIRE_FALSE(isLetter('\xc2'));
  REQUIRE_FALSE(isLetter('\xff'));
}

TEST_CASE("isLowercase", "[CharInfoTest]") {
  REQUIRE_FALSE(isLowercase('0'));
  REQUIRE_FALSE(isLowercase('9'));

  REQUIRE(isLowercase('a'));
  REQUIRE_FALSE(isLowercase('A'));

  REQUIRE(isLowercase('z'));
  REQUIRE_FALSE(isLowercase('Z'));

  REQUIRE_FALSE(isLowercase('.'));
  REQUIRE_FALSE(isLowercase('_'));

  REQUIRE_FALSE(isLowercase('/'));
  REQUIRE_FALSE(isLowercase('('));
  REQUIRE_FALSE(isLowercase('\0'));

  REQUIRE_FALSE(isLowercase('\x80'));
  REQUIRE_FALSE(isLowercase('\xc2'));
  REQUIRE_FALSE(isLowercase('\xff'));
}

TEST_CASE("isUppercase", "[CharInfoTest]") {
  REQUIRE_FALSE(isUppercase('0'));
  REQUIRE_FALSE(isUppercase('9'));

  REQUIRE_FALSE(isUppercase('a'));
  REQUIRE(isUppercase('A'));

  REQUIRE_FALSE(isUppercase('z'));
  REQUIRE(isUppercase('Z'));

  REQUIRE_FALSE(isUppercase('.'));
  REQUIRE_FALSE(isUppercase('_'));

  REQUIRE_FALSE(isUppercase('/'));
  REQUIRE_FALSE(isUppercase('('));
  REQUIRE_FALSE(isUppercase('\0'));

  REQUIRE_FALSE(isUppercase('\x80'));
  REQUIRE_FALSE(isUppercase('\xc2'));
  REQUIRE_FALSE(isUppercase('\xff'));
}

TEST_CASE("isAlphanumeric", "[CharInfoTest]") {
  REQUIRE(isAlphanumeric('0'));
  REQUIRE(isAlphanumeric('9'));

  REQUIRE(isAlphanumeric('a'));
  REQUIRE(isAlphanumeric('A'));

  REQUIRE(isAlphanumeric('z'));
  REQUIRE(isAlphanumeric('Z'));

  REQUIRE_FALSE(isAlphanumeric('.'));
  REQUIRE_FALSE(isAlphanumeric('_'));

  REQUIRE_FALSE(isAlphanumeric('/'));
  REQUIRE_FALSE(isAlphanumeric('('));
  REQUIRE_FALSE(isAlphanumeric('\0'));

  REQUIRE_FALSE(isAlphanumeric('\x80'));
  REQUIRE_FALSE(isAlphanumeric('\xc2'));
  REQUIRE_FALSE(isAlphanumeric('\xff'));
}

TEST_CASE("isPunctuation", "[CharInfoTest]") {
  REQUIRE_FALSE(isPunctuation('0'));
  REQUIRE_FALSE(isPunctuation('9'));

  REQUIRE_FALSE(isPunctuation('a'));
  REQUIRE_FALSE(isPunctuation('A'));

  REQUIRE_FALSE(isPunctuation('z'));
  REQUIRE_FALSE(isPunctuation('Z'));

  REQUIRE(isPunctuation('.'));
  REQUIRE(isPunctuation('_'));

  REQUIRE(isPunctuation('/'));
  REQUIRE(isPunctuation('('));

  REQUIRE_FALSE(isPunctuation(' '));
  REQUIRE_FALSE(isPunctuation('\n'));
  REQUIRE_FALSE(isPunctuation('\0'));

  REQUIRE_FALSE(isPunctuation('\x80'));
  REQUIRE_FALSE(isPunctuation('\xc2'));
  REQUIRE_FALSE(isPunctuation('\xff'));
}

TEST_CASE("isPrintable", "[CharInfoTest]") {
  REQUIRE(isPrintable('0'));
  REQUIRE(isPrintable('9'));

  REQUIRE(isPrintable('a'));
  REQUIRE(isPrintable('A'));

  REQUIRE(isPrintable('z'));
  REQUIRE(isPrintable('Z'));

  REQUIRE(isPrintable('.'));
  REQUIRE(isPrintable('_'));

  REQUIRE(isPrintable('/'));
  REQUIRE(isPrintable('('));

  REQUIRE(isPrintable(' '));
  REQUIRE_FALSE(isPrintable('\t'));
  REQUIRE_FALSE(isPrintable('\n'));
  REQUIRE_FALSE(isPrintable('\0'));

  REQUIRE_FALSE(isPrintable('\x80'));
  REQUIRE_FALSE(isPrintable('\xc2'));
  REQUIRE_FALSE(isPrintable('\xff'));
}

TEST_CASE("toLowercase", "[CharInfoTest]") {
  REQUIRE('0' == toLowercase('0'));
  REQUIRE('9' == toLowercase('9'));

  REQUIRE('a' == toLowercase('a'));
  REQUIRE('a' == toLowercase('A'));

  REQUIRE('z' == toLowercase('z'));
  REQUIRE('z' == toLowercase('Z'));

  REQUIRE('.' == toLowercase('.'));
  REQUIRE('_' == toLowercase('_'));

  REQUIRE('/' == toLowercase('/'));
  REQUIRE('\0' == toLowercase('\0'));
}

TEST_CASE("toUppercase", "[CharInfoTest]") {
  REQUIRE('0' == toUppercase('0'));
  REQUIRE('9' == toUppercase('9'));

  REQUIRE('A' == toUppercase('a'));
  REQUIRE('A' == toUppercase('A'));

  REQUIRE('Z' == toUppercase('z'));
  REQUIRE('Z' == toUppercase('Z'));

  REQUIRE('.' == toUppercase('.'));
  REQUIRE('_' == toUppercase('_'));

  REQUIRE('/' == toUppercase('/'));
  REQUIRE('\0' == toUppercase('\0'));
}

TEST_CASE("isValidIdentifier", "[CharInfoTest]") {
  REQUIRE_FALSE(isValidIdentifier(""));

  // 1 character
  REQUIRE_FALSE(isValidIdentifier("."));
  REQUIRE_FALSE(isValidIdentifier("\n"));
  REQUIRE_FALSE(isValidIdentifier(" "));
  REQUIRE_FALSE(isValidIdentifier("\x80"));
  REQUIRE_FALSE(isValidIdentifier("\xc2"));
  REQUIRE_FALSE(isValidIdentifier("\xff"));
  REQUIRE_FALSE(isValidIdentifier("1"));

  REQUIRE(isValidIdentifier("_"));
  REQUIRE(isValidIdentifier("a"));
  REQUIRE(isValidIdentifier("z"));
  REQUIRE(isValidIdentifier("A"));
  REQUIRE(isValidIdentifier("Z"));
  REQUIRE(isValidIdentifier("$"));

  // 2 characters, '_' suffix
  REQUIRE_FALSE(isValidIdentifier("._"));
  REQUIRE_FALSE(isValidIdentifier("\n_"));
  REQUIRE_FALSE(isValidIdentifier(" _"));
  REQUIRE_FALSE(isValidIdentifier("\x80_"));
  REQUIRE_FALSE(isValidIdentifier("\xc2_"));
  REQUIRE_FALSE(isValidIdentifier("\xff_"));
  REQUIRE_FALSE(isValidIdentifier("1_"));

  REQUIRE(isValidIdentifier("__"));
  REQUIRE(isValidIdentifier("a_"));
  REQUIRE(isValidIdentifier("z_"));
  REQUIRE(isValidIdentifier("A_"));
  REQUIRE(isValidIdentifier("Z_"));
  REQUIRE(isValidIdentifier("$_"));

  // 2 characters, '_' prefix
  REQUIRE_FALSE(isValidIdentifier("_."));
  REQUIRE_FALSE(isValidIdentifier("_\n"));
  REQUIRE_FALSE(isValidIdentifier("_ "));
  REQUIRE_FALSE(isValidIdentifier("_\x80"));
  REQUIRE_FALSE(isValidIdentifier("_\xc2"));
  REQUIRE_FALSE(isValidIdentifier("_\xff"));
  REQUIRE(isValidIdentifier("_1"));

  REQUIRE(isValidIdentifier("__"));
  REQUIRE(isValidIdentifier("_a"));
  REQUIRE(isValidIdentifier("_z"));
  REQUIRE(isValidIdentifier("_A"));
  REQUIRE(isValidIdentifier("_Z"));
  REQUIRE(isValidIdentifier("_$"));

  // 3 characters, '__' prefix
  REQUIRE_FALSE(isValidIdentifier("__."));
  REQUIRE_FALSE(isValidIdentifier("__\n"));
  REQUIRE_FALSE(isValidIdentifier("__ "));
  REQUIRE_FALSE(isValidIdentifier("__\x80"));
  REQUIRE_FALSE(isValidIdentifier("__\xc2"));
  REQUIRE_FALSE(isValidIdentifier("__\xff"));
  REQUIRE(isValidIdentifier("__1"));

  REQUIRE(isValidIdentifier("___"));
  REQUIRE(isValidIdentifier("__a"));
  REQUIRE(isValidIdentifier("__z"));
  REQUIRE(isValidIdentifier("__A"));
  REQUIRE(isValidIdentifier("__Z"));
  REQUIRE(isValidIdentifier("__$"));

  // 3 characters, '_' prefix and suffix
  REQUIRE_FALSE(isValidIdentifier("_._"));
  REQUIRE_FALSE(isValidIdentifier("_\n_"));
  REQUIRE_FALSE(isValidIdentifier("_ _"));
  REQUIRE_FALSE(isValidIdentifier("_\x80_"));
  REQUIRE_FALSE(isValidIdentifier("_\xc2_"));
  REQUIRE_FALSE(isValidIdentifier("_\xff_"));
  REQUIRE(isValidIdentifier("_1_"));

  REQUIRE(isValidIdentifier("___"));
  REQUIRE(isValidIdentifier("_a_"));
  REQUIRE(isValidIdentifier("_z_"));
  REQUIRE(isValidIdentifier("_A_"));
  REQUIRE(isValidIdentifier("_Z_"));
  REQUIRE(isValidIdentifier("_$_"));
}
