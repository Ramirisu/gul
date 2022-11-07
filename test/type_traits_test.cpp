#include <ds_test.h>

#include <ds/type_traits.hpp>

using namespace ds;

TEST_SUITE_BEGIN("type_traits");

namespace std {
struct std_swappable {
  friend void swap(std_swappable&, std_swappable&) { }
};
struct std_nothrow_swappable {
  friend void swap(std_nothrow_swappable&, std_nothrow_swappable&) noexcept { }
};
}

struct non_std_swappable {
  friend void swap(non_std_swappable&, non_std_swappable&) { }
  friend void swap(non_std_swappable&, int&) { }
};

struct non_std_nothrow_swappable {
  friend void swap(non_std_nothrow_swappable&,
                   non_std_nothrow_swappable&) noexcept
  {
  }
};

struct non_swappable {
  friend void swap(non_swappable&, non_swappable&) = delete;
};

struct nothrow_non_swappable {
  friend void swap(nothrow_non_swappable&, nothrow_non_swappable&) noexcept
      = delete;
};

TEST_CASE("is_swappable_with")
{
  static_assert(!is_swappable_with<int, int>::value, "");
  static_assert(is_swappable_with<int&, int&>::value, "");
  static_assert(!is_swappable_with<int[1], int[1]>::value, "");
  static_assert(is_swappable_with<int(&)[1], int(&)[1]>::value, "");
  static_assert(
      is_swappable_with<std::std_swappable&, std::std_swappable&>::value, "");
  static_assert(
      is_swappable_with<non_std_swappable&, non_std_swappable&>::value, "");
  static_assert(!is_swappable_with<non_swappable&, non_swappable&>::value, "");

  static_assert(!is_swappable_with<int&, double&>::value, "");
}

TEST_CASE("is_swappable")
{
  static_assert(is_swappable<int>::value, "");
  static_assert(is_swappable<int[1]>::value, "");
  static_assert(is_swappable<std::std_swappable>::value, "");
  static_assert(is_swappable<non_std_swappable>::value, "");
  static_assert(!is_swappable<non_swappable>::value, "");
}

TEST_CASE("is_nothrow_swappable_with")
{
  static_assert(!is_nothrow_swappable_with<int, int>::value, "");
  static_assert(is_nothrow_swappable_with<int&, int&>::value, "");
  static_assert(!is_nothrow_swappable_with<int[1], int[1]>::value, "");
  static_assert(is_nothrow_swappable_with<int(&)[1], int(&)[1]>::value, "");
  static_assert(!is_nothrow_swappable_with<std::std_swappable&,
                                           std::std_swappable&>::value,
                "");
  static_assert(
      !is_nothrow_swappable_with<non_std_swappable&, non_std_swappable&>::value,
      "");
  static_assert(
      !is_nothrow_swappable_with<non_swappable&, non_swappable&>::value, "");
  static_assert(is_nothrow_swappable_with<std::std_nothrow_swappable&,
                                          std::std_nothrow_swappable&>::value,
                "");
  static_assert(is_nothrow_swappable_with<non_std_nothrow_swappable&,
                                          non_std_nothrow_swappable&>::value,
                "");
  static_assert(!is_nothrow_swappable_with<nothrow_non_swappable&,
                                           nothrow_non_swappable&>::value,
                "");
  static_assert(!is_nothrow_swappable_with<non_std_swappable&, int&>::value,
                "");
}

TEST_CASE("is_nothrow_swappable")
{
  static_assert(is_nothrow_swappable<int>::value, "");
  static_assert(is_nothrow_swappable<int[1]>::value, "");
  static_assert(!is_nothrow_swappable<std::std_swappable>::value, "");
  static_assert(!is_nothrow_swappable<non_std_swappable>::value, "");
  static_assert(!is_nothrow_swappable<non_swappable>::value, "");
  static_assert(is_nothrow_swappable<std::std_nothrow_swappable>::value, "");
  static_assert(is_nothrow_swappable<non_std_nothrow_swappable>::value, "");
  static_assert(!is_nothrow_swappable<nothrow_non_swappable>::value, "");
}

TEST_SUITE_END();
