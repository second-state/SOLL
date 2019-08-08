// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#pragma once

#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>
#include <memory>

namespace soll {

template <typename T, typename... Args>
std::vector<std::unique_ptr<T>> make_unique_vector(Args &&... args) {
  std::vector<std::unique_ptr<T>> result;
  result.reserve(sizeof...(args));
  (result.emplace_back(std::forward<Args>(args)), ...);
  return result;
}

/// Conditional const
/// cond_const<true,  Type>: const Type
/// cond_const<false, Type>: Type
template <bool Const, class Type>
struct cond_const {
  typedef const Type type;
};

template <class Type> struct cond_const<false, Type> { typedef Type type; };

} // namespace soll
