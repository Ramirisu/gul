//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/detail/type_traits.hpp>

GUL_NAMESPACE_BEGIN

using detail::bool_constant;

using detail::type_identity;
using detail::type_identity_t;

using detail::enable_if;
using detail::enable_if_t;

using detail::void_t;

using detail::add_lvalue_reference;
using detail::add_lvalue_reference_t;

using detail::add_pointer;
using detail::add_pointer_t;

using detail::conditional;
using detail::conditional_t;

using detail::decay;
using detail::decay_t;

using detail::remove_cv;
using detail::remove_cv_t;

using detail::remove_reference;
using detail::remove_reference_t;

using detail::remove_cvref;
using detail::remove_cvref_t;

using detail::negation;
using detail::negation_t;

using detail::conjunction;
using detail::conjunction_t;

using detail::disjunction;
using detail::disjunction_t;

using detail::invoke_result;
using detail::invoke_result_t;

using detail::function_traits;

using detail::is_specialization_of;

template <typename T>
struct is_null_pointer : std::is_same<std::nullptr_t, remove_cv_t<T>> { };

using detail::is_nothrow_convertible;

using detail::is_swappable;
using detail::is_swappable_with;

using detail::is_nothrow_swappable;
using detail::is_nothrow_swappable_with;

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

template <typename T>
struct is_bounded_array : std::false_type { };

template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type { };

template <typename T>
struct is_unbounded_array : std::false_type { };

template <typename T>
struct is_unbounded_array<T[]> : std::true_type { };

template <typename E, bool = std::is_enum<E>::value>
struct is_scoped_enum : std::false_type { };

template <typename E>
struct is_scoped_enum<E, true>
    : negation<std::is_convertible<E, typename std::underlying_type<E>::type>> {
};

GUL_NAMESPACE_END
