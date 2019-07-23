#pragma once

namespace soll {

/// Conditional const
/// cond_const<true,  Type>: const Type
/// cond_const<false, Type>: Type
template <bool Const, class Type> struct cond_const {
  typedef const Type type;
};

template <class Type> struct cond_const<false, Type> { typedef Type type; };

} // namespace
