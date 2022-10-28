#pragma once

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <type_traits>

template <typename L, typename R>
struct static_assert_same {
  static_assert(std::is_same<L, R>::value, "");
};
