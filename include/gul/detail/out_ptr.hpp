//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/type_traits.hpp>
#include <gul/detail/utility.hpp>

#include <tuple>

GUL_NAMESPACE_BEGIN

namespace detail {
namespace out_ptr_detail {

  template <typename T, typename = void>
  struct has_member_pointer : std::false_type { };

  template <typename T>
  struct has_member_pointer<T, void_t<typename T::pointer>> : std::true_type {
  };

  template <typename T, typename = void>
  struct has_member_element_type : std::false_type { };

  template <typename T>
  struct has_member_element_type<T, void_t<typename T::element_type>>
      : std::true_type { };

  template <typename T,
            bool = has_member_pointer<T>::value,
            bool = has_member_element_type<T>::value>
  struct pointer_of_impl;

  template <typename T, bool Unused>
  struct pointer_of_impl<T, true, Unused> {
    using type = typename T::pointer;
  };

  template <typename T>
  struct pointer_of_impl<T, false, true> {
    using type = typename T::element_type*;
  };

  template <typename T>
  struct pointer_of_impl<T, false, false> {
    using type = typename std::pointer_traits<T>::element_type*;
  };

  template <typename T>
  using pointer_of = typename pointer_of_impl<T>::type;

  template <typename T, typename U, typename = void>
  struct pointer_of_or_impl {
    using type = U;
  };

  template <typename T, typename U>
  struct pointer_of_or_impl<T, U, void_t<pointer_of<T>>> {
    using type = pointer_of<T>;
  };

  template <typename T, typename U>
  using pointer_of_or = typename pointer_of_or_impl<T, U>::type;

  template <typename Void, typename Smart, typename Pointer, typename... Args>
  struct is_resettable : std::false_type { };

  template <typename Smart, typename Pointer, typename... Args>
  struct is_resettable<void_t<decltype(std::declval<Smart&>().reset(
                           std::declval<pointer_of_or<Smart, Pointer>>(),
                           std::declval<Args>()...))>,
                       Smart,
                       Pointer,
                       Args...> : std::true_type { };

  template <typename Smart>
  auto get_pointer(Smart& sptr)
      -> enable_if_t<std::is_pointer<Smart>::value, Smart>
  {
    return sptr;
  }

  template <typename Smart>
  auto get_pointer(Smart& sptr)
      -> enable_if_t<!std::is_pointer<Smart>::value, decltype(sptr.get())>
  {
    return sptr.get();
  }

}
}

GUL_NAMESPACE_END
