//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/byte.hpp>

using namespace gul;

TEST_SUITE_BEGIN("byte");

TEST_CASE("basic")
{
  {
    auto b = to_byte(0x55);
    CHECK_EQ(to_integer<int>(b), 85);
    CHECK_EQ((b >> 1), to_byte(0x2a));
    CHECK_EQ((b << 1), to_byte(0xaa));
    CHECK_EQ((b >>= 1), to_byte(0x2a));
    CHECK_EQ((b), to_byte(0x2a));
    CHECK_EQ((b <<= 1), to_byte(0x54));
    CHECK_EQ((b), to_byte(0x54));
    CHECK_EQ(~b, to_byte(0xab));
  }
  {
    auto lhs = to_byte(0xc3);
    const auto rhs = to_byte(0xaa);
    CHECK_EQ((lhs & rhs), to_byte(0x82));
    CHECK_EQ((lhs | rhs), to_byte(0xeb));
    CHECK_EQ((lhs ^ rhs), to_byte(0x69));
    CHECK_EQ((lhs &= rhs), to_byte(0x82));
    CHECK_EQ(lhs, to_byte(0x82));
    lhs = to_byte(0xc3);
    CHECK_EQ((lhs |= rhs), to_byte(0xeb));
    CHECK_EQ(lhs, to_byte(0xeb));
    lhs = to_byte(0xc3);
    CHECK_EQ((lhs ^= rhs), to_byte(0x69));
    CHECK_EQ(lhs, to_byte(0x69));
  }
}

TEST_SUITE_END();
