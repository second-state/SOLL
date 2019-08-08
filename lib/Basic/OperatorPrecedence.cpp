// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/Basic/OperatorPrecedence.h"

namespace soll {

prec::Level getBinOpPrecedence(tok::TokenKind Kind) {
  switch (Kind) {
  case tok::greater:              return prec::Relational;
  case tok::greatergreater:       return prec::Shift;
  case tok::comma:                return prec::Comma;
  case tok::equal:
  case tok::starequal:
  case tok::slashequal:
  case tok::percentequal:
  case tok::plusequal:
  case tok::minusequal:
  case tok::lesslessequal:
  case tok::greatergreaterequal:
  case tok::ampequal:
  case tok::caretequal:
  case tok::pipeequal:            return prec::Assignment;
  case tok::question:             return prec::Conditional;
  case tok::pipepipe:             return prec::LogicalOr;
  //case tok::caretcaret:
  case tok::ampamp:               return prec::LogicalAnd;
  case tok::pipe:                 return prec::InclusiveOr;
  case tok::caret:                return prec::ExclusiveOr;
  case tok::amp:                  return prec::And;
  case tok::exclaimequal:
  case tok::equalequal:           return prec::Equality;
  case tok::lessequal:
  case tok::less:
  case tok::greaterequal:         return prec::Relational;
  //case tok::spaceship:            return prec::Spaceship;
  case tok::lessless:             return prec::Shift;
  case tok::plus:
  case tok::minus:                return prec::Additive;
  case tok::percent:
  case tok::slash:
  case tok::star:                 return prec::Multiplicative;
  //case tok::periodstar:
  //case tok::arrowstar:            return prec::PointerToMember;
  default:                        return prec::Unknown;
  }
}

}  // namespace soll
