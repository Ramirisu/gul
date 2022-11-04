#pragma once

#include <ds/config.hpp>

#include <ds/type_traits.hpp>

namespace ds {
template <typename C, typename Pointed, class T1, typename... Args>
constexpr auto __invoke_memptr(Pointed C::*f, T1&& t1, Args&&... args) noexcept(
    noexcept((std::forward<T1>(t1).*f)(std::forward<Args>(args)...)))
    -> decltype((std::forward<T1>(t1).*f)(std::forward<Args>(args)...))
{
  // TODO:
  return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
}

template <typename F,
          typename... Args,
          enable_if_t<is_member_pointer<typename decay<F>::type>::value, int>
          = 0>
constexpr auto invoke(F&& f, Args&&... args) noexcept(
    noexcept(__invoke_memptr(std::forward<F>(f), std::forward<Args>(args)...)))
    -> decltype(__invoke_memptr(std::forward<F>(f),
                                std::forward<Args>(args)...))
{
  return __invoke_memptr(std::forward<F>(f), std::forward<Args>(args)...);
}

template <typename F,
          typename... Args,
          enable_if_t<!is_member_pointer<typename decay<F>::type>::value, int>
          = 0>
constexpr auto invoke(F&& f, Args&&... args) noexcept(
    noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
    -> invoke_result_t<F, Args...>
{
  return std::forward<F>(f)(std::forward<Args>(args)...);
}
}
