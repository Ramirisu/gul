//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/utility.hpp>

using namespace gul;

TEST_SUITE_BEGIN("utility");

namespace {
template <bool IsNothrowMoveConstructible, bool IsNothrowMoveAssignable>
struct movable {
  movable(int x) noexcept
      : x(x)
  {
  }
  movable(movable&& other) noexcept(IsNothrowMoveConstructible)
      : x(other.x)
  {
  }
  movable& operator=(movable&& other) noexcept(IsNothrowMoveAssignable)
  {
    x = other.x;
    return *this;
  }
  int x;
  friend bool operator==(const movable& lhs, const movable& rhs) noexcept
  {
    return lhs.x == rhs.x;
  }
};
}

TEST_CASE("exchange")
{
  {
    auto x = movable<true, true>(0);
    static_assert(noexcept(exchange(x, movable<true, true>(1))), "");
    CHECK_EQ(exchange(x, movable<true, true>(1)), movable<true, true>(0));
    CHECK_EQ(x, movable<true, true>(1));
  }
  {
    auto x = movable<false, true>(0);
    static_assert(!noexcept(exchange(x, movable<false, true>(1))), "");
    CHECK_EQ(exchange(x, movable<false, true>(1)), movable<false, true>(0));
    CHECK_EQ(x, movable<false, true>(1));
  }
  {
    auto x = movable<true, false>(0);
    static_assert(!noexcept(exchange(x, movable<true, false>(1))), "");
    CHECK_EQ(exchange(x, movable<true, false>(1)), movable<true, false>(0));
    CHECK_EQ(x, movable<true, false>(1));
  }
  {
    auto x = movable<false, false>(0);
    static_assert(!noexcept(exchange(x, movable<false, false>(1))), "");
    CHECK_EQ(exchange(x, movable<false, false>(1)), movable<false, false>(0));
    CHECK_EQ(x, movable<false, false>(1));
  }
}

TEST_CASE("as_const")
{
  int x = 1;
  static_assert_same<decltype(as_const(x)), const int&>();
  CHECK_EQ(x, 1);
}

TEST_CASE("to_underlying")
{
  enum class e : unsigned char {
    v = 1,
  };
  static_assert_same<decltype(to_underlying(e::v)), unsigned char>();
  CHECK_EQ(to_underlying(e::v), 1);
}

TEST_SUITE_END();
