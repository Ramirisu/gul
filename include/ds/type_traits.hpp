#pragma once

#include <ds/config.hpp>

#include <type_traits>

namespace ds {

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
using std::is_move_constructible;
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

#else

using std::invoke_result;
using std::invoke_result_t;

using std::conjunction;
using std::disjunction;
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
