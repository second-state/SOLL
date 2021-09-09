// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace soll {

template <typename T, typename... Args>
std::vector<std::unique_ptr<T>> make_unique_vector(Args &&... As) {
  std::vector<std::unique_ptr<T>> result;
  result.reserve(sizeof...(As));
  (result.emplace_back(std::forward<Args>(As)), ...);
  return result;
}

/// Conditional const
/// cond_const<true,  Type>: const Type
/// cond_const<false, Type>: Type
template <bool Const, class Type> struct cond_const {
  typedef const Type type;
};

template <class Type> struct cond_const<false, Type> { typedef Type type; };

/// Concatenate the contents of a container onto a vector
template <class T, class U>
std::vector<T> &operator+=(std::vector<T> &A, U &B) {
  for (auto const &I : B) {
    A.push_back(T(I));
  }
  return A;
}
/// Concatenate the contents of a container onto a vector, move variant.
template <class T, class U>
std::vector<T> &operator+=(std::vector<T> &A, U &&B) {
  std::move(B.begin(), B.end(), std::back_inserter(A));
  return A;
}

} // namespace soll
