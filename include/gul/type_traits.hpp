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
template <typename T>
struct type_identity {
  using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

template <bool B, typename T>
struct enable_if { };

template <typename T>
struct enable_if<true, T> : type_identity<T> { };

template <bool B, typename T>
using enable_if_t = typename enable_if<B, T>::type;

template <typename... Ts>
struct make_void {
  typedef void type;
};

template <typename... Ts>
using void_t = typename detail::make_void<Ts...>::type;

using std::add_lvalue_reference;
using std::add_pointer;
using std::decay;
using std::integral_constant;
using std::remove_cv;
using std::remove_reference;

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

template <bool B, typename T, typename F>
struct conditional : type_identity<T> { };

template <typename T, typename F>
struct conditional<false, T, F> : type_identity<F> { };

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <typename T>
using decay_t = typename decay<T>::type;

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T>
struct remove_cvref : remove_cv<remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

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

#ifdef GUL_HAS_CXX17

template <typename F, typename... Args>
using invoke_result_t = typename std::invoke_result<F, Args...>::type;

#else

template <typename F, typename... Args>
using invoke_result_t = typename std::result_of<F(Args...)>::type;

#endif

template <typename T, template <typename...> class U>
struct is_specialization_of : std::false_type { };

template <template <typename...> class U, typename... Args>
struct is_specialization_of<U<Args...>, U> : std::true_type { };

namespace swap_detail {
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
struct is_swappable_with
    : conjunction<swap_detail::is_swappable_with_impl<T, U>,
                  swap_detail::is_swappable_with_impl<U, T>> { };

template <typename T>
struct is_swappable
    : conjunction<
          swap_detail::is_swappable_with_impl<add_lvalue_reference_t<T>,
                                              add_lvalue_reference_t<T>>> { };

namespace swap_detail {
  using std::swap;

  template <typename T, typename U, bool = is_swappable_with<T, U>::value>
  struct is_nothrow_swappable_with_impl : std::false_type { };

  template <typename T, typename U>
  struct is_nothrow_swappable_with_impl<T, U, true>
      : integral_constant<bool,
                          noexcept(
                              swap(std::declval<T>(), std::declval<U>()))> { };
}

template <typename T, typename U>
struct is_nothrow_swappable_with
    : conjunction<swap_detail::is_nothrow_swappable_with_impl<T, U>,
                  swap_detail::is_nothrow_swappable_with_impl<U, T>> { };

template <typename T>
struct is_nothrow_swappable
    : conjunction<swap_detail::is_nothrow_swappable_with_impl<
          add_lvalue_reference_t<T>,
          add_lvalue_reference_t<T>>> { };

template <typename T, typename U, typename = void>
struct is_eq_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_eq_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() == std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   == std::declval<const U&>()),
                          bool> { };

template <typename T, typename U, typename = void>
struct is_ne_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_ne_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() != std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   != std::declval<const U&>()),
                          bool> { };

template <typename T, typename U, typename = void>
struct is_lt_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_lt_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() < std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   < std::declval<const U&>()),
                          bool> { };

template <typename T, typename U, typename = void>
struct is_le_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_le_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() <= std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   <= std::declval<const U&>()),
                          bool> { };

template <typename T, typename U, typename = void>
struct is_gt_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_gt_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() > std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   > std::declval<const U&>()),
                          bool> { };

template <typename T, typename U, typename = void>
struct is_ge_comparable_with : std::false_type { };

template <typename T, typename U>
struct is_ge_comparable_with<
    T,
    U,
    void_t<decltype(std::declval<const T&>() >= std::declval<const U&>())>>
    : std::is_convertible<decltype(std::declval<const T&>()
                                   >= std::declval<const U&>()),
                          bool> { };

namespace is_nothrow_convertible_detail {
  struct is_nothrow_convertible_helper {
    template <typename To>
    static constexpr void test(To) noexcept;
  };
}

template <typename From,
          typename To,
          bool = std::is_convertible<From, To>::value>
struct is_nothrow_convertible
    : integral_constant<bool,
                        noexcept(is_nothrow_convertible_detail::
                                     is_nothrow_convertible_helper::test<To>(
                                         std::declval<From>()))> { };

template <typename From, typename To>
struct is_nothrow_convertible<From, To, false> : std::false_type { };

template <typename From>
struct is_nothrow_convertible<From, void, true> : std::true_type { };

namespace is_invocable_detail {
  template <typename T,
            bool = std::is_member_function_pointer<T>::value,
            bool = std::is_member_object_pointer<T>::value>
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
            -> enable_if_t<std::is_base_of<C, remove_reference_t<Arg1>>::value,
                           decltype((std::forward<Arg1>(arg1)
                                     .*f)(std::forward<Args>(args)...))>;

    template <typename C, typename Pointed, typename Arg1, typename... Args>
    static constexpr auto test(Pointed C::*f, Arg1&& arg1, Args&&... args) //
        noexcept(noexcept(((*std::forward<Arg1>(arg1))
                           .*f)(std::forward<Args>(args)...)))
            -> enable_if_t<!std::is_base_of<C, remove_reference_t<Arg1>>::value,
                           decltype(((*std::forward<Arg1>(arg1))
                                     .*f)(std::forward<Args>(args)...))>;
  };

  template <typename T>
  struct is_invocable_get_ret_type_impl<T, false, true> {
    template <typename C, typename Pointed, typename Arg1>
    static constexpr auto test(Pointed C::*o, Arg1&& arg1) noexcept
        -> enable_if_t<std::is_base_of<C, remove_reference_t<Arg1>>::value,
                       decltype(std::forward<Arg1>(arg1).*o)>;

    template <typename C, typename Pointed, typename Arg1>
    static constexpr auto test(Pointed C::*o,
                               Arg1&& arg1) //
        noexcept(noexcept((*std::forward<Arg1>(arg1)).*o))
            -> enable_if_t<!std::is_base_of<C, remove_reference_t<Arg1>>::value,
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
    using is_invocable_r
        = disjunction<std::is_void<R2>, std::is_convertible<R, R2>>;
    template <typename R2>
    using is_nothrow_invocable_r = conjunction<
        NoExcept,
        disjunction<std::is_void<R2>, is_nothrow_convertible<R, R2>>>;
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
    : is_invocable_detail::is_invocable_impl<void, F, Args...>::is_invocable {
};

#ifdef GUL_HAS_CXX17
// requires noexcept-specification as a part of function type
template <typename F, typename... Args>
struct is_nothrow_invocable
    : is_invocable_detail::is_invocable_impl<void, F, Args...>::
          is_nothrow_invocable { };
#endif

template <typename R, typename F, typename... Args>
struct is_invocable_r
    : is_invocable_detail::is_invocable_impl<void, F, Args...>::
          template is_invocable_r<R> { };

#ifdef GUL_HAS_CXX17
// requires noexcept-specification as a part of function type
template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r
    : is_invocable_detail::is_invocable_impl<void, F, Args...>::
          template is_nothrow_invocable_r<R> { };
#endif
}

using detail::add_lvalue_reference_t;
using detail::add_pointer_t;
using detail::conditional_t;
using detail::conjunction;
using detail::decay_t;
using detail::disjunction;
using detail::invoke_result_t;
using detail::negation;
using detail::remove_cv_t;
using detail::remove_cvref_t;
using detail::remove_reference_t;
using detail::type_identity_t;

#define GUL_REQUIRES(...) detail::enable_if_t<__VA_ARGS__, int> = 0

GUL_NAMESPACE_END
