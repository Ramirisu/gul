#pragma once

#include <ds/config.hpp>

#include <type_traits>

namespace ds {
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
using std::is_constructible;
using std::is_convertible;
using std::is_copy_constructible;
using std::is_default_constructible;
using std::is_destructible;
using std::is_lvalue_reference;
using std::is_member_pointer;
using std::is_move_assignable;
using std::is_move_constructible;
using std::is_nothrow_move_assignable;
using std::is_nothrow_move_constructible;
using std::is_reference;
using std::is_same;
using std::is_scalar;
using std::is_trivially_copy_assignable;
using std::is_trivially_copy_constructible;
using std::is_trivially_destructible;
using std::is_trivially_move_assignable;
using std::is_trivially_move_constructible;
using std::is_void;

#ifndef DS_HAS_CXX14

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
using remove_reference_t = typename remove_reference<T>::type;

#else

using std::add_lvalue_reference_t;
using std::add_pointer_t;
using std::conditional_t;
using std::decay_t;
using std::enable_if_t;
using std::remove_reference_t;

#endif

#ifndef DS_HAS_CXX17

template <bool B>
using bool_constant = integral_constant<bool, B>;

template <typename F, typename... Args>
struct invoke_result : std::result_of<F(Args...)> { };

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

template <typename T>
struct negation : bool_constant<!T::value> { };

template <typename... Ts>
struct conjunction : std::true_type { };

template <typename T>
struct conjunction<T> : T { };

template <typename T, typename... Ts>
struct conjunction<T, Ts...>
    : conditional<T::value, conjunction<Ts...>, T>::type { };

template <typename... Ts>
struct disjunction : std::false_type { };

template <typename T>
struct disjunction<T> : T { };

template <typename T, typename... Ts>
struct disjunction<T, Ts...>
    : conditional<T::value, T, disjunction<Ts...>>::type { };

namespace detail {
  using std::swap;

  template <typename T, typename U, typename = void>
  struct can_swap_with_impl : std::false_type { };

  template <typename T, typename U>
  struct can_swap_with_impl<
      T,
      U,
      void_t<decltype(swap(std::declval<T>(), std::declval<U>()))>>
      : std::true_type { };

  template <typename T, typename U>
  struct can_swap_with : can_swap_with_impl<T, U> { };
}

template <typename T, typename U>
struct is_swappable_with
    : integral_constant<bool,
                        conjunction<detail::can_swap_with<T, U>,
                                    detail::can_swap_with<U, T>>::value> { };

template <typename T>
struct is_swappable
    : integral_constant<bool,
                        conjunction<detail::can_swap_with<
                            add_lvalue_reference_t<T>,
                            add_lvalue_reference_t<T>>>::value> { };

namespace detail {
  using std::swap;

  template <typename T, typename U, bool = is_swappable_with<T, U>::value>
  struct can_nothrow_swap_with_impl : std::false_type { };

  template <typename T, typename U>
  struct can_nothrow_swap_with_impl<T, U, true> {
    static constexpr bool value
        = noexcept(swap(std::declval<T>(), std::declval<U>()));
  };

  template <typename T, typename U>
  struct can_nothrow_swap_with : can_nothrow_swap_with_impl<T, U> { };
}

template <typename T, typename U>
struct is_nothrow_swappable_with
    : integral_constant<
          bool,
          conjunction<detail::can_nothrow_swap_with<T, U>,
                      detail::can_nothrow_swap_with<U, T>>::value> { };

template <typename T>
struct is_nothrow_swappable
    : integral_constant<bool,
                        conjunction<detail::can_nothrow_swap_with<
                            add_lvalue_reference_t<T>,
                            add_lvalue_reference_t<T>>>::value> { };

#else

using std::invoke_result;
using std::invoke_result_t;

using std::conjunction;
using std::disjunction;
using std::is_nothrow_swappable;
using std::is_nothrow_swappable_with;
using std::is_swappable;
using std::is_swappable_with;
using std::negation;

#endif

#ifndef DS_HAS_CXX20

template <typename T>
struct remove_cvref : remove_cv<remove_reference_t<T>> { };

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

#else

using std::remove_cvref;
using std::remove_cvref_t;

#endif

template <typename T, template <typename...> class U>
struct is_specialization_of : std::false_type { };

template <template <typename...> class U, typename... Args>
struct is_specialization_of<U<Args...>, U> : std::true_type { };
}
