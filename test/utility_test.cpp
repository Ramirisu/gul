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
    STATIC_ASSERT(noexcept(exchange(x, movable<true, true>(1))));
    CHECK_EQ(exchange(x, movable<true, true>(1)), movable<true, true>(0));
    CHECK_EQ(x, movable<true, true>(1));
  }
  {
    auto x = movable<false, true>(0);
    STATIC_ASSERT(!noexcept(exchange(x, movable<false, true>(1))));
    CHECK_EQ(exchange(x, movable<false, true>(1)), movable<false, true>(0));
    CHECK_EQ(x, movable<false, true>(1));
  }
  {
    auto x = movable<true, false>(0);
    STATIC_ASSERT(!noexcept(exchange(x, movable<true, false>(1))));
    CHECK_EQ(exchange(x, movable<true, false>(1)), movable<true, false>(0));
    CHECK_EQ(x, movable<true, false>(1));
  }
  {
    auto x = movable<false, false>(0);
    STATIC_ASSERT(!noexcept(exchange(x, movable<false, false>(1))));
    CHECK_EQ(exchange(x, movable<false, false>(1)), movable<false, false>(0));
    CHECK_EQ(x, movable<false, false>(1));
  }
}

TEST_CASE("as_const")
{
  int x = 1;
  STATIC_ASSERT_SAME(decltype(as_const(x)), const int&);
  CHECK_EQ(x, 1);
}

TEST_CASE("to_underlying")
{
  enum class e : unsigned char {
    v = 1,
  };
  STATIC_ASSERT_SAME(decltype(to_underlying(e::v)), unsigned char);
  CHECK_EQ(to_underlying(e::v), 1);
}

TEST_CASE("cmp_equal")
{
  CHECK(cmp_equal(std::int32_t(1), std::int32_t(1)));
  CHECK(cmp_equal(std::int32_t(0), std::int32_t(0)));
  CHECK(cmp_equal(std::int32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_equal(std::int32_t(1), std::int32_t(0)));
  CHECK(!cmp_equal(std::int32_t(0), std::int32_t(1)));
  CHECK(!cmp_equal(std::int32_t(0), std::int32_t(-1)));
  CHECK(!cmp_equal(std::int32_t(-1), std::int32_t(0)));
  CHECK(!cmp_equal(std::int32_t(-1), std::int32_t(1)));
  CHECK(!cmp_equal(std::int32_t(1), std::int32_t(-1)));

  CHECK(cmp_equal(std::uint32_t(1), std::uint32_t(1)));
  CHECK(cmp_equal(std::uint32_t(0), std::uint32_t(0)));
  CHECK(cmp_equal(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_equal(std::uint32_t(1), std::uint32_t(0)));
  CHECK(!cmp_equal(std::uint32_t(0), std::uint32_t(1)));
  CHECK(!cmp_equal(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_equal(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_equal(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_equal(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(cmp_equal(std::int32_t(1), std::uint32_t(1)));
  CHECK(cmp_equal(std::int32_t(0), std::uint32_t(0)));
  CHECK(!cmp_equal(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_equal(std::int32_t(1), std::uint32_t(0)));
  CHECK(!cmp_equal(std::int32_t(0), std::uint32_t(1)));
  CHECK(!cmp_equal(std::int32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_equal(std::int32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_equal(std::int32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_equal(std::int32_t(1), std::uint32_t(-1)));

  CHECK(cmp_equal(std::uint32_t(1), std::int32_t(1)));
  CHECK(cmp_equal(std::uint32_t(0), std::int32_t(0)));
  CHECK(!cmp_equal(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_equal(std::uint32_t(1), std::int32_t(0)));
  CHECK(!cmp_equal(std::uint32_t(0), std::int32_t(1)));
  CHECK(!cmp_equal(std::uint32_t(0), std::int32_t(-1)));
  CHECK(!cmp_equal(std::uint32_t(-1), std::int32_t(0)));
  CHECK(!cmp_equal(std::uint32_t(-1), std::int32_t(1)));
  CHECK(!cmp_equal(std::uint32_t(1), std::int32_t(-1)));
}

TEST_CASE("cmp_not_equal")
{
  CHECK(!cmp_not_equal(std::int32_t(1), std::int32_t(1)));
  CHECK(!cmp_not_equal(std::int32_t(0), std::int32_t(0)));
  CHECK(!cmp_not_equal(std::int32_t(-1), std::int32_t(-1)));
  CHECK(cmp_not_equal(std::int32_t(1), std::int32_t(0)));
  CHECK(cmp_not_equal(std::int32_t(0), std::int32_t(1)));
  CHECK(cmp_not_equal(std::int32_t(0), std::int32_t(-1)));
  CHECK(cmp_not_equal(std::int32_t(-1), std::int32_t(0)));
  CHECK(cmp_not_equal(std::int32_t(-1), std::int32_t(1)));
  CHECK(cmp_not_equal(std::int32_t(1), std::int32_t(-1)));

  CHECK(!cmp_not_equal(std::uint32_t(1), std::uint32_t(1)));
  CHECK(!cmp_not_equal(std::uint32_t(0), std::uint32_t(0)));
  CHECK(!cmp_not_equal(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_not_equal(std::uint32_t(1), std::uint32_t(0)));
  CHECK(cmp_not_equal(std::uint32_t(0), std::uint32_t(1)));
  CHECK(cmp_not_equal(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(cmp_not_equal(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(cmp_not_equal(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(cmp_not_equal(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_not_equal(std::int32_t(1), std::uint32_t(1)));
  CHECK(!cmp_not_equal(std::int32_t(0), std::uint32_t(0)));
  CHECK(cmp_not_equal(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_not_equal(std::int32_t(1), std::uint32_t(0)));
  CHECK(cmp_not_equal(std::int32_t(0), std::uint32_t(1)));
  CHECK(cmp_not_equal(std::int32_t(0), std::uint32_t(-1)));
  CHECK(cmp_not_equal(std::int32_t(-1), std::uint32_t(0)));
  CHECK(cmp_not_equal(std::int32_t(-1), std::uint32_t(1)));
  CHECK(cmp_not_equal(std::int32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_not_equal(std::uint32_t(1), std::int32_t(1)));
  CHECK(!cmp_not_equal(std::uint32_t(0), std::int32_t(0)));
  CHECK(cmp_not_equal(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(cmp_not_equal(std::uint32_t(1), std::int32_t(0)));
  CHECK(cmp_not_equal(std::uint32_t(0), std::int32_t(1)));
  CHECK(cmp_not_equal(std::uint32_t(0), std::int32_t(-1)));
  CHECK(cmp_not_equal(std::uint32_t(-1), std::int32_t(0)));
  CHECK(cmp_not_equal(std::uint32_t(-1), std::int32_t(1)));
  CHECK(cmp_not_equal(std::uint32_t(1), std::int32_t(-1)));
}

TEST_CASE("cmp_less")
{
  CHECK(!cmp_less(std::int32_t(1), std::int32_t(1)));
  CHECK(!cmp_less(std::int32_t(0), std::int32_t(0)));
  CHECK(!cmp_less(std::int32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_less(std::int32_t(1), std::int32_t(0)));
  CHECK(cmp_less(std::int32_t(0), std::int32_t(1)));
  CHECK(!cmp_less(std::int32_t(0), std::int32_t(-1)));
  CHECK(cmp_less(std::int32_t(-1), std::int32_t(0)));
  CHECK(cmp_less(std::int32_t(-1), std::int32_t(1)));
  CHECK(!cmp_less(std::int32_t(1), std::int32_t(-1)));

  CHECK(!cmp_less(std::uint32_t(1), std::uint32_t(1)));
  CHECK(!cmp_less(std::uint32_t(0), std::uint32_t(0)));
  CHECK(!cmp_less(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_less(std::uint32_t(1), std::uint32_t(0)));
  CHECK(cmp_less(std::uint32_t(0), std::uint32_t(1)));
  CHECK(cmp_less(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_less(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_less(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(cmp_less(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_less(std::int32_t(1), std::uint32_t(1)));
  CHECK(!cmp_less(std::int32_t(0), std::uint32_t(0)));
  CHECK(cmp_less(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_less(std::int32_t(1), std::uint32_t(0)));
  CHECK(cmp_less(std::int32_t(0), std::uint32_t(1)));
  CHECK(cmp_less(std::int32_t(0), std::uint32_t(-1)));
  CHECK(cmp_less(std::int32_t(-1), std::uint32_t(0)));
  CHECK(cmp_less(std::int32_t(-1), std::uint32_t(1)));
  CHECK(cmp_less(std::int32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_less(std::uint32_t(1), std::int32_t(1)));
  CHECK(!cmp_less(std::uint32_t(0), std::int32_t(0)));
  CHECK(!cmp_less(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_less(std::uint32_t(1), std::int32_t(0)));
  CHECK(cmp_less(std::uint32_t(0), std::int32_t(1)));
  CHECK(!cmp_less(std::uint32_t(0), std::int32_t(-1)));
  CHECK(!cmp_less(std::uint32_t(-1), std::int32_t(0)));
  CHECK(!cmp_less(std::uint32_t(-1), std::int32_t(1)));
  CHECK(!cmp_less(std::uint32_t(1), std::int32_t(-1)));
}

TEST_CASE("cmp_greater")
{
  CHECK(!cmp_greater(std::int32_t(1), std::int32_t(1)));
  CHECK(!cmp_greater(std::int32_t(0), std::int32_t(0)));
  CHECK(!cmp_greater(std::int32_t(-1), std::int32_t(-1)));
  CHECK(cmp_greater(std::int32_t(1), std::int32_t(0)));
  CHECK(!cmp_greater(std::int32_t(0), std::int32_t(1)));
  CHECK(cmp_greater(std::int32_t(0), std::int32_t(-1)));
  CHECK(!cmp_greater(std::int32_t(-1), std::int32_t(0)));
  CHECK(!cmp_greater(std::int32_t(-1), std::int32_t(1)));
  CHECK(cmp_greater(std::int32_t(1), std::int32_t(-1)));

  CHECK(!cmp_greater(std::uint32_t(1), std::uint32_t(1)));
  CHECK(!cmp_greater(std::uint32_t(0), std::uint32_t(0)));
  CHECK(!cmp_greater(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_greater(std::uint32_t(1), std::uint32_t(0)));
  CHECK(!cmp_greater(std::uint32_t(0), std::uint32_t(1)));
  CHECK(!cmp_greater(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(cmp_greater(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(cmp_greater(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_greater(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_greater(std::int32_t(1), std::uint32_t(1)));
  CHECK(!cmp_greater(std::int32_t(0), std::uint32_t(0)));
  CHECK(!cmp_greater(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_greater(std::int32_t(1), std::uint32_t(0)));
  CHECK(!cmp_greater(std::int32_t(0), std::uint32_t(1)));
  CHECK(!cmp_greater(std::int32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_greater(std::int32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_greater(std::int32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_greater(std::int32_t(1), std::uint32_t(-1)));

  CHECK(!cmp_greater(std::uint32_t(1), std::int32_t(1)));
  CHECK(!cmp_greater(std::uint32_t(0), std::int32_t(0)));
  CHECK(cmp_greater(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(cmp_greater(std::uint32_t(1), std::int32_t(0)));
  CHECK(!cmp_greater(std::uint32_t(0), std::int32_t(1)));
  CHECK(cmp_greater(std::uint32_t(0), std::int32_t(-1)));
  CHECK(cmp_greater(std::uint32_t(-1), std::int32_t(0)));
  CHECK(cmp_greater(std::uint32_t(-1), std::int32_t(1)));
  CHECK(cmp_greater(std::uint32_t(1), std::int32_t(-1)));
}

TEST_CASE("cmp_less_equal")
{
  CHECK(cmp_less_equal(std::int32_t(1), std::int32_t(1)));
  CHECK(cmp_less_equal(std::int32_t(0), std::int32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_less_equal(std::int32_t(1), std::int32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(0), std::int32_t(1)));
  CHECK(!cmp_less_equal(std::int32_t(0), std::int32_t(-1)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::int32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::int32_t(1)));
  CHECK(!cmp_less_equal(std::int32_t(1), std::int32_t(-1)));

  CHECK(cmp_less_equal(std::uint32_t(1), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::uint32_t(0), std::uint32_t(0)));
  CHECK(cmp_less_equal(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_less_equal(std::uint32_t(1), std::uint32_t(0)));
  CHECK(cmp_less_equal(std::uint32_t(0), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_less_equal(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_less_equal(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(cmp_less_equal(std::int32_t(1), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::int32_t(0), std::uint32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(!cmp_less_equal(std::int32_t(1), std::uint32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(0), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::int32_t(0), std::uint32_t(-1)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::uint32_t(0)));
  CHECK(cmp_less_equal(std::int32_t(-1), std::uint32_t(1)));
  CHECK(cmp_less_equal(std::int32_t(1), std::uint32_t(-1)));

  CHECK(cmp_less_equal(std::uint32_t(1), std::int32_t(1)));
  CHECK(cmp_less_equal(std::uint32_t(0), std::int32_t(0)));
  CHECK(!cmp_less_equal(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(!cmp_less_equal(std::uint32_t(1), std::int32_t(0)));
  CHECK(cmp_less_equal(std::uint32_t(0), std::int32_t(1)));
  CHECK(!cmp_less_equal(std::uint32_t(0), std::int32_t(-1)));
  CHECK(!cmp_less_equal(std::uint32_t(-1), std::int32_t(0)));
  CHECK(!cmp_less_equal(std::uint32_t(-1), std::int32_t(1)));
  CHECK(!cmp_less_equal(std::uint32_t(1), std::int32_t(-1)));
}

TEST_CASE("cmp_greater_equal")
{
  CHECK(cmp_greater_equal(std::int32_t(1), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::int32_t(0), std::int32_t(0)));
  CHECK(cmp_greater_equal(std::int32_t(-1), std::int32_t(-1)));
  CHECK(cmp_greater_equal(std::int32_t(1), std::int32_t(0)));
  CHECK(!cmp_greater_equal(std::int32_t(0), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::int32_t(0), std::int32_t(-1)));
  CHECK(!cmp_greater_equal(std::int32_t(-1), std::int32_t(0)));
  CHECK(!cmp_greater_equal(std::int32_t(-1), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::int32_t(1), std::int32_t(-1)));

  CHECK(cmp_greater_equal(std::uint32_t(1), std::uint32_t(1)));
  CHECK(cmp_greater_equal(std::uint32_t(0), std::uint32_t(0)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_greater_equal(std::uint32_t(1), std::uint32_t(0)));
  CHECK(!cmp_greater_equal(std::uint32_t(0), std::uint32_t(1)));
  CHECK(!cmp_greater_equal(std::uint32_t(0), std::uint32_t(-1)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::uint32_t(0)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_greater_equal(std::uint32_t(1), std::uint32_t(-1)));

  CHECK(cmp_greater_equal(std::int32_t(1), std::uint32_t(1)));
  CHECK(cmp_greater_equal(std::int32_t(0), std::uint32_t(0)));
  CHECK(!cmp_greater_equal(std::int32_t(-1), std::uint32_t(-1)));
  CHECK(cmp_greater_equal(std::int32_t(1), std::uint32_t(0)));
  CHECK(!cmp_greater_equal(std::int32_t(0), std::uint32_t(1)));
  CHECK(!cmp_greater_equal(std::int32_t(0), std::uint32_t(-1)));
  CHECK(!cmp_greater_equal(std::int32_t(-1), std::uint32_t(0)));
  CHECK(!cmp_greater_equal(std::int32_t(-1), std::uint32_t(1)));
  CHECK(!cmp_greater_equal(std::int32_t(1), std::uint32_t(-1)));

  CHECK(cmp_greater_equal(std::uint32_t(1), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::uint32_t(0), std::int32_t(0)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::int32_t(-1)));
  CHECK(cmp_greater_equal(std::uint32_t(1), std::int32_t(0)));
  CHECK(!cmp_greater_equal(std::uint32_t(0), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::uint32_t(0), std::int32_t(-1)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::int32_t(0)));
  CHECK(cmp_greater_equal(std::uint32_t(-1), std::int32_t(1)));
  CHECK(cmp_greater_equal(std::uint32_t(1), std::int32_t(-1)));
}

TEST_SUITE_END();
