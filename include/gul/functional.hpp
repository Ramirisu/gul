//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <gul/config.hpp>

#include <gul/type_traits.hpp>

GUL_NAMESPACE_BEGIN

namespace detail {
template <typename C,
          typename Pointed,
          class T1,
          typename... Args,
          enable_if_t<is_base_of<C, decay_t<T1>>::value, int> = 0>
constexpr auto
invoke_memfun_impl(Pointed C::*f, T1&& t1, Args&&... args) noexcept(
    noexcept((std::forward<T1>(t1).*f)(std::forward<Args>(args)...)))
    -> decltype((std::forward<T1>(t1).*f)(std::forward<Args>(args)...))
{
  return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
}

template <typename C,
          typename Pointed,
          class T1,
          typename... Args,
          enable_if_t<!is_base_of<C, decay_t<T1>>::value, int> = 0>
constexpr auto
invoke_memfun_impl(Pointed C::*f, T1&& t1, Args&&... args) noexcept(
    noexcept(((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...)))
    -> decltype(((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...))
{
  return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
}

template <typename C,
          typename Pointed,
          class T1,
          enable_if_t<is_base_of<C, decay_t<T1>>::value, int> = 0>
constexpr auto invoke_memobj_impl(Pointed C::*f, T1&& t1) noexcept(
    noexcept(std::forward<T1>(t1).*f)) -> decltype(std::forward<T1>(t1).*f)
{
  return std::forward<T1>(t1).*f;
}

template <typename C,
          typename Pointed,
          class T1,
          enable_if_t<!is_base_of<C, decay_t<T1>>::value, int> = 0>
constexpr auto
invoke_memobj_impl(Pointed C::*f,
                   T1&& t1) noexcept(noexcept((*std::forward<T1>(t1)).*f))
    -> decltype((*std::forward<T1>(t1)).*f)
{
  return (*std::forward<T1>(t1)).*f;
}

template <typename C,
          typename Pointed,
          class T1,
          typename... Args,
          enable_if_t<is_function<Pointed>::value, int> = 0>
constexpr auto
invoke_memptr_impl(Pointed C::*f, T1&& t1, Args&&... args) noexcept(noexcept(
    invoke_memfun_impl(f, std::forward<T1>(t1), std::forward<Args>(args)...)))
    -> decltype(invoke_memfun_impl(f,
                                   std::forward<T1>(t1),
                                   std::forward<Args>(args)...))
{
  return invoke_memfun_impl(f, std::forward<T1>(t1),
                            std::forward<Args>(args)...);
}

template <typename C,
          typename Pointed,
          class T1,
          typename... Args,
          enable_if_t<is_object<Pointed>::value, int> = 0>
constexpr auto invoke_memptr_impl(Pointed C::*f, T1&& t1, Args&&...) noexcept(
    noexcept(invoke_memobj_impl(f, std::forward<T1>(t1))))
    -> decltype(invoke_memobj_impl(f, std::forward<T1>(t1)))
{
  static_assert(sizeof...(Args) == 0,
                "member object should be called with no argument");
  return invoke_memobj_impl(f, std::forward<T1>(t1));
}

template <typename C, typename Pointed, class T1, typename... Args>
constexpr auto
invoke_memptr(Pointed C::*f, T1&& t1, Args&&... args) noexcept(noexcept(
    invoke_memptr_impl(f, std::forward<T1>(t1), std::forward<Args>(args)...)))
    -> decltype(invoke_memptr_impl(f,
                                   std::forward<T1>(t1),
                                   std::forward<Args>(args)...))
{
  return invoke_memptr_impl(f, std::forward<T1>(t1),
                            std::forward<Args>(args)...);
}

template <typename F,
          typename... Args,
          enable_if_t<is_member_pointer<decay_t<F>>::value, int> = 0>
constexpr auto invoke_impl(F&& f, Args&&... args) noexcept(
    noexcept(invoke_memptr(f, std::forward<Args>(args)...)))
    -> decltype(invoke_memptr(f, std::forward<Args>(args)...))
{
  return invoke_memptr(f, std::forward<Args>(args)...);
}

template <typename F,
          typename... Args,
          enable_if_t<!is_member_pointer<decay_t<F>>::value, int> = 0>
constexpr auto invoke_impl(F&& f, Args&&... args) noexcept(
    noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
    -> decltype(std::forward<F>(f)(std::forward<Args>(args)...))
{
  return std::forward<F>(f)(std::forward<Args>(args)...);
}
}

template <typename F,
          typename... Args,
          enable_if_t<is_invocable<F, Args...>::value, int> = 0>
constexpr auto invoke(F&& f, Args&&... args) noexcept(noexcept(
    detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...)))
    -> decltype(detail::invoke_impl(std::forward<F>(f),
                                    std::forward<Args>(args)...))
{
  return detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename R,
          typename F,
          typename... Args,
          enable_if_t<is_invocable_r<R, F, Args...>::value, int> = 0>
constexpr auto invoke_r(F&& f, Args&&... args) noexcept(noexcept(
    detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...))) -> R
{
  return detail::invoke_impl(std::forward<F>(f), std::forward<Args>(args)...);
}

GUL_NAMESPACE_END
