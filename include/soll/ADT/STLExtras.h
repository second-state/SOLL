#pragma once

#include <initializer_list>
#include <iterator>
#include <utility>

namespace soll {

/// Conditional const
/// cond_const<true,  Type>: const Type
/// cond_const<false, Type>: Type
template <bool Const, class Type> struct cond_const {
  typedef const Type type;
};

template <class Type> struct cond_const<false, Type> { typedef Type type; };

/// Create container C of unique_ptr from initializer_list
template <class C> struct container {
  template <class Type> struct movable {
    mutable Type T;
    template <class U> movable(U &&u) : T(std::forward<U>(u)) {}
    operator Type() const && { return std::move(T); }
  };

  static C init(std::initializer_list<movable<typename C::value_type>> l) {
    return C(std::make_move_iterator(l.begin()),
             std::make_move_iterator(l.end()));
  }
};

} // namespace
