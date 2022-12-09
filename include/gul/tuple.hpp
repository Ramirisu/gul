//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/invoke.hpp>
#include <gul/type_traits.hpp>
#include <gul/utility.hpp>

#include <tuple>

GUL_NAMESPACE_BEGIN

namespace detail {
template <typename F, typename Tuple, std::size_t... Is>
constexpr auto apply_impl(F&& f, Tuple&& tuple, index_sequence<Is...>) noexcept(
    noexcept(invoke(std::forward<F>(f),
                    std::get<Is>(std::forward<Tuple>(tuple))...)))
    -> decltype(invoke(std::forward<F>(f),
                       std::get<Is>(std::forward<Tuple>(tuple))...))
{
  return invoke(std::forward<F>(f),
                std::get<Is>(std::forward<Tuple>(tuple))...);
}
}

template <typename F, typename Tuple>
constexpr auto apply(F&& f, Tuple&& tuple) noexcept(noexcept(detail::apply_impl(
    std::forward<F>(f),
    std::forward<Tuple>(tuple),
    make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value> {})))
    -> decltype(detail::apply_impl(
        std::forward<F>(f),
        std::forward<Tuple>(tuple),
        make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value> {}))
{
  return detail::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(tuple),
      make_index_sequence<std::tuple_size<remove_cvref_t<Tuple>>::value> {});
}

GUL_NAMESPACE_END
