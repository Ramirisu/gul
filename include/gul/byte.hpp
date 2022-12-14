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

enum class byte : unsigned char {};

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr Integral to_integer(byte b) noexcept
{
  return static_cast<Integral>(b);
}

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr byte operator<<(byte b, Integral shift) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(b) << shift);
}

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr byte operator>>(byte b, Integral shift) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(b) >> shift);
}

constexpr byte operator&(byte lhs, byte rhs) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(lhs)
                           & static_cast<unsigned int>(rhs));
}

constexpr byte operator|(byte lhs, byte rhs) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(lhs)
                           | static_cast<unsigned int>(rhs));
}

constexpr byte operator^(byte lhs, byte rhs) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(lhs)
                           ^ static_cast<unsigned int>(rhs));
}

constexpr byte operator~(byte b) noexcept
{
  return static_cast<byte>(~static_cast<unsigned int>(b));
}

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr byte& operator<<=(byte& b, Integral shift) noexcept
{
  return b = b << shift;
}

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr byte& operator>>=(byte& b, Integral shift) noexcept
{
  return b = b >> shift;
}

#if !defined(__GNUC__) || defined(GUL_HAS_CXX14)
constexpr
#else
inline
#endif
    byte&
    operator&=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs & rhs;
}

#if !defined(__GNUC__) || defined(GUL_HAS_CXX14)
constexpr
#else
inline
#endif
    byte&
    operator|=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs | rhs;
}

#if !defined(__GNUC__) || defined(GUL_HAS_CXX14)
constexpr
#else
inline
#endif
    byte&
    operator^=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs ^ rhs;
}

template <typename Integral, GUL_REQUIRES(std::is_integral<Integral>::value)>
constexpr byte to_byte(Integral i) noexcept
{
  return static_cast<byte>(i);
}

GUL_NAMESPACE_END
