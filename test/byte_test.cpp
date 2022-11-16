//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ds_test.h>

#include <ds/byte.hpp>

using namespace ds;

TEST_SUITE_BEGIN("byte");

TEST_CASE("basic")
{
  {
    auto b = to_byte(0b01010101);
    CHECK_EQ(to_integer<int>(b), 85);
    CHECK_EQ((b >> 1), to_byte(0b00101010));
    CHECK_EQ((b << 1), to_byte(0b10101010));
    CHECK_EQ((b >>= 1), to_byte(0b00101010));
    CHECK_EQ((b), to_byte(0b00101010));
    CHECK_EQ((b <<= 1), to_byte(0b01010100));
    CHECK_EQ((b), to_byte(0b01010100));
    CHECK_EQ(~b, to_byte(0b10101011));
  }
  {
    auto lhs = to_byte(0b11000011);
    const auto rhs = to_byte(0b10101010);
    CHECK_EQ((lhs & rhs), to_byte(0b10000010));
    CHECK_EQ((lhs | rhs), to_byte(0b11101011));
    CHECK_EQ((lhs ^ rhs), to_byte(0b01101001));
    CHECK_EQ((lhs &= rhs), to_byte(0b10000010));
    CHECK_EQ(lhs, to_byte(0b10000010));
    lhs = to_byte(0b11000011);
    CHECK_EQ((lhs |= rhs), to_byte(0b11101011));
    CHECK_EQ(lhs, to_byte(0b11101011));
    lhs = to_byte(0b11000011);
    CHECK_EQ((lhs ^= rhs), to_byte(0b01101001));
    CHECK_EQ(lhs, to_byte(0b01101001));
  }
}

TEST_SUITE_END();
