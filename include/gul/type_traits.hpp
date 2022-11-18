//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <type_traits>

GUL_NAMESPACE_BEGIN

namespace detail {
template <typename... Ts>
struct make_void {
  typedef void type;
};
}
template <typename... Ts>
using void_t = typename detail::make_void<Ts...>::type;

using std::add_lvalue_reference;
using std::add_pointer;
using std::conditional;
using std::decay;
using std::enable_if;
using std::integral_constant;
using std::remove_cv;
using std::remove_reference;

using std::is_array;
using std::is_assignable;
using std::is_base_of;
using std::is_constructible;
using std::is_convertible;
using std::is_copy_constructible;
using std::is_default_constructible;
using std::is_destructible;
using std::is_function;
using std::is_integral;
using std::is_lvalue_reference;
using std::is_member_function_pointer;
using std::is_member_object_pointer;
using std::is_member_pointer;
using std::is_move_assignable;
using std::is_move_constructible;
using std::is_nothrow_move_assignable;
using std::is_nothrow_move_constructible;
using std::is_object;
using std::is_reference;
using std::is_same;
using std::is_scalar;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_destructible;
using std::is_trivially_move_assignable;
using std::is_trivially_move_constructible;
using std::is_void;

#ifndef GUL_HAS_CXX14

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <typename T>
using decay_t = typename decay<T>::type;

template <bool B, typename T>
using enable_if_t = typename enable_if<B, T>::type;

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

#else

using std::add_lvalue_reference_t;
using std::add_pointer_t;
using std::conditional_t;
using std::decay_t;
using std::enable_if_t;
using std::remove_cv_t;
using std::remove_reference_t;

#endif

template <typename T>
struct negation : integral_constant<bool, !static_cast<bool>(T::value)> { };

template <typename... Ts>
struct conjunction : std::true_type { };

template <typename T>
struct conjunction<T> : T { };

template <typename T, typename... Ts>
struct conjunction<T, Ts...>
    : conditional<static_cast<bool>(T::value), conjunction<Ts...>, T>::type { };

template <typename... Ts>
struct disjunction : std::false_type { };

template <typename T>
struct disjunction<T> : T { };

template <typename T, typename... Ts>
struct disjunction<T, Ts...>
    : conditional<static_cast<bool>(T::value), T, disjunction<Ts...>>::type { };

#ifndef GUL_HAS_CXX17

template <typename F, typename... Args>
struct invoke_result : std::result_of<F(Args...)> { };

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

namespace detail {
using std::swap;

template <typename T, typename U, typename = void>
struct is_swappable_with_impl : std::false_type { };

template <typename T, typename U>
struct is_swappable_with_impl<
    T,
    U,
    void_t<decltype(swap(std::declval<T>(), std::declval<U>()))>>
    : std::true_type { };
}

template <typename T, typename U>
struct is_swappable_with : conjunction<detail::is_swappable_with_impl<T, U>,
                                       detail::is_swappable_with_impl<U, T>> {
};

template <typename T>
struct is_swappable
    : conjunction<detail::is_swappable_with_impl<add_lvalue_reference_t<T>,
                                                 add_lvalue_reference_t<T>>> {
};

namespace detail {
using std::swap;

template <typename T, typename U, bool = is_swappable_with<T, U>::value>
struct is_nothrow_swappable_with_impl : std::false_type { };

template <typename T, typename U>
struct is_nothrow_swappable_with_impl<T, U, true>
    : integral_constant<bool,
                        noexcept(swap(std::declval<T>(), std::declval<U>()))> {
};
}

template <typename T, typename U>
struct is_nothrow_swappable_with
    : conjunction<detail::is_nothrow_swappable_with_impl<T, U>,
                  detail::is_nothrow_swappable_with_impl<U, T>> { };

template <typename T>
struct is_nothrow_swappable
    : conjunction<
          detail::is_nothrow_swappable_with_impl<add_lvalue_reference_t<T>,
                                                 add_lvalue_reference_t<T>>> {
};

#else

using std::invoke_result;
using std::invoke_result_t;

using std::is_nothrow_swappable;
using std::is_nothrow_swappable_with;
using std::is_swappable;
using std::is_swappable_with;

#endif

#ifndef GUL_HAS_CXX20

template <typename T>
struct remove_cvref : remove_cv<remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

template <typename T>
struct type_identity {
  using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

#else

using std::remove_cvref;
using std::remove_cvref_t;
using std::type_identity;
using std::type_identity_t;

#endif

namespace detail {
template <typename T, typename U, typename = void>
struct is_eq_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_eq_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() == std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              == std::declval<const U&>()),
                     bool> { };

template <typename T, typename U, typename = void>
struct is_ne_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_ne_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() != std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              != std::declval<const U&>()),
                     bool> { };

template <typename T, typename U, typename = void>
struct is_lt_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_lt_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() < std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              < std::declval<const U&>()),
                     bool> { };

template <typename T, typename U, typename = void>
struct is_le_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_le_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() <= std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              <= std::declval<const U&>()),
                     bool> { };

template <typename T, typename U, typename = void>
struct is_gt_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_gt_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() > std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              > std::declval<const U&>()),
                     bool> { };

template <typename T, typename U, typename = void>
struct is_ge_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_ge_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() >= std::declval<const U&>())>>
    : is_convertible<decltype(std::declval<const T&>()
                              >= std::declval<const U&>()),
                     bool> { };
}

template <typename T, template <typename...> class U>
struct is_specialization_of : std::false_type { };

template <template <typename...> class U, typename... Args>
struct is_specialization_of<U<Args...>, U> : std::true_type { };

namespace detail {
struct is_nothrow_convertible_helper {
  template <typename To>
  static constexpr void test(To) noexcept;
};
}

template <typename From, typename To, bool = is_convertible<From, To>::value>
struct is_nothrow_convertible
    : integral_constant<bool,
                        noexcept(detail::is_nothrow_convertible_helper::test<
                                 To>(std::declval<From>()))> { };

template <typename From, typename To>
struct is_nothrow_convertible<From, To, false> : std::false_type { };

template <typename From>
struct is_nothrow_convertible<From, void, true> : std::true_type { };

namespace detail {
template <typename T,
          bool = is_member_function_pointer<T>::value,
          bool = is_member_object_pointer<T>::value>
struct is_invocable_get_ret_type_impl;

template <typename T>
struct is_invocable_get_ret_type_impl<T, false, false> {
  template <typename F, typename... Args>
  static constexpr auto test(F&& f, Args&&... args) //
      noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
          -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
};

template <typename T>
struct is_invocable_get_ret_type_impl<T, true, false> {
  template <typename C, typename Pointed, typename Arg1, typename... Args>
  static constexpr auto test(Pointed C::*f, Arg1&& arg1, Args&&... args) //
      noexcept(noexcept((std::forward<Arg1>(arg1)
                         .*f)(std::forward<Args>(args)...)))
          -> enable_if_t<is_base_of<C, remove_reference_t<Arg1>>::value,
                         decltype((std::forward<Arg1>(arg1)
                                   .*f)(std::forward<Args>(args)...))>;

  template <typename C, typename Pointed, typename Arg1, typename... Args>
  static constexpr auto test(Pointed C::*f, Arg1&& arg1, Args&&... args) //
      noexcept(noexcept(((*std::forward<Arg1>(arg1))
                         .*f)(std::forward<Args>(args)...)))
          -> enable_if_t<!is_base_of<C, remove_reference_t<Arg1>>::value,
                         decltype(((*std::forward<Arg1>(arg1))
                                   .*f)(std::forward<Args>(args)...))>;
};

template <typename T>
struct is_invocable_get_ret_type_impl<T, false, true> {
  template <typename C, typename Pointed, typename Arg1>
  static constexpr auto test(Pointed C::*o, Arg1&& arg1) noexcept
      -> enable_if_t<is_base_of<C, remove_reference_t<Arg1>>::value,
                     decltype(std::forward<Arg1>(arg1).*o)>;

  template <typename C, typename Pointed, typename Arg1>
  static constexpr auto test(Pointed C::*o,
                             Arg1&& arg1) //
      noexcept(noexcept((*std::forward<Arg1>(arg1)).*o))
          -> enable_if_t<!is_base_of<C, remove_reference_t<Arg1>>::value,
                         decltype((*std::forward<Arg1>(arg1)).*o)>;
};

template <typename F, typename... Args>
using is_invocable_get_ret_type
    = decltype(is_invocable_get_ret_type_impl<F>::test(
        std::declval<F>(), std::declval<Args>()...));

template <typename F, typename... Args>
struct is_invocable_noexcept
    : integral_constant<bool,
                        noexcept(is_invocable_get_ret_type_impl<F>::test(
                            std::declval<F>(), std::declval<Args>()...))> { };

#if !defined(_MSC_VER) || _MSC_VER < 1920
template <typename F>
struct is_invocable_noexcept<F, void>
    : integral_constant<bool,
                        noexcept(is_invocable_get_ret_type_impl<F>::test(
                            std::declval<F>()))> { };
#endif

template <typename R, typename NoExcept>
struct is_invocable_traits {
  using is_invocable = std::true_type;
  using is_nothrow_invocable = NoExcept;
  template <typename R2>
  using is_invocable_r = disjunction<is_void<R2>, is_convertible<R, R2>>;
  template <typename R2>
  using is_nothrow_invocable_r
      = conjunction<NoExcept,
                    disjunction<is_void<R2>, is_nothrow_convertible<R, R2>>>;
};

template <typename Void, typename F, typename... Args>
struct is_invocable_impl {
  using is_invocable = std::false_type;
  using is_nothrow_invocable = std::false_type;
  template <typename R2>
  using is_invocable_r = std::false_type;
  template <typename R2>
  using is_nothrow_invocable_r = std::false_type;
};

template <typename F, typename... Args>
struct is_invocable_impl<void_t<is_invocable_get_ret_type<F, Args...>>,
                         F,
                         Args...>
    : is_invocable_traits<is_invocable_get_ret_type<F, Args...>,
                          is_invocable_noexcept<F, Args...>> { };
}

template <typename F, typename... Args>
struct is_invocable
    : detail::is_invocable_impl<void, F, Args...>::is_invocable { };

#ifdef GUL_HAS_CXX17
// requires noexcept-specification as a part of function type
template <typename F, typename... Args>
struct is_nothrow_invocable
    : detail::is_invocable_impl<void, F, Args...>::is_nothrow_invocable { };
#endif

template <typename R, typename F, typename... Args>
struct is_invocable_r
    : detail::is_invocable_impl<void, F, Args...>::template is_invocable_r<R> {
};

#ifdef GUL_HAS_CXX17
// requires noexcept-specification as a part of function type
template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r : detail::is_invocable_impl<void, F, Args...>::
                                    template is_nothrow_invocable_r<R> { };
#endif

GUL_NAMESPACE_END
