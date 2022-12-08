//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/tuple.hpp>

TEST_SUITE_BEGIN("tuple");

TEST_CASE("apply")
{
  auto t = std::tuple<char, int, double> { 'x', 1, 0.1 };
  {
    auto f = [](char c, int i, double d) { return c + i + d; };
    STATIC_ASSERT(!noexcept(gul::apply(f, t)));
    auto val = gul::apply(f, t);
    CHECK_EQ(doctest::Approx(val), 121.1);
  }
  {
    auto f = [](char c, int i, double d) noexcept { return c + i + d; };
    STATIC_ASSERT(noexcept(gul::apply(f, t)));
    auto val = gul::apply(f, t);
    CHECK_EQ(doctest::Approx(val), 121.1);
  }
}

TEST_SUITE_END();
