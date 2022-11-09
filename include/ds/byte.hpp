#pragma once

#include <ds/config.hpp>

#include <ds/type_traits.hpp>

#include <cstddef>

namespace ds {
#ifdef DS_HAS_CXX17

using std::byte;
using std::to_integer;

#else

enum class byte : unsigned char {};

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
constexpr Integral to_integer(byte b) noexcept
{
  return static_cast<Integral>(b);
}

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
constexpr byte operator<<(byte b, Integral shift) noexcept
{
  return static_cast<byte>(static_cast<unsigned int>(b) << shift);
}

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
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

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
constexpr byte& operator<<=(byte& b, Integral shift) noexcept
{
  return b = b << shift;
}

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
constexpr byte& operator>>=(byte& b, Integral shift) noexcept
{
  return b = b >> shift;
}

#if defined(DS_COMPILER_GCC) && defined(DS_HAS_CXX14)
constexpr
#endif
    byte&
    operator&=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs & rhs;
}

#if defined(DS_COMPILER_GCC) && defined(DS_HAS_CXX14)
constexpr
#endif
    byte&
    operator|=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs | rhs;
}

#if defined(DS_COMPILER_GCC) && defined(DS_HAS_CXX14)
constexpr
#endif
    byte&
    operator^=(byte& lhs, byte rhs) noexcept
{
  return lhs = lhs ^ rhs;
}

#endif

template <typename Integral, enable_if_t<is_integral<Integral>::value, int> = 0>
constexpr byte to_byte(Integral i) noexcept
{
  return static_cast<byte>(i);
}
}
