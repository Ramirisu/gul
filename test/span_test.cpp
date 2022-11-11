#include <ds_test.h>

#include <ds/span.hpp>

#include <array>

using namespace ds;

TEST_SUITE_BEGIN("span");

TEST_CASE("basic")
{
  static_assert(sizeof(span<int>) == 2 * sizeof(std::size_t), "");
  static_assert(sizeof(span<int, 10>) == sizeof(std::size_t), "");
  {
    auto s = span<int>();
    CHECK(s.empty());
    CHECK_EQ(s.data(), nullptr);
    CHECK_EQ(s.size(), 0);
    CHECK_EQ(s.size_bytes(), 0);
    CHECK(s.template first<0>().empty());
    CHECK(s.first(0).empty());
  }
  {
    int arr[] = { 0, 1, 2, 3 };
    auto s = span<int>(arr);
    CHECK(!s.empty());
    CHECK_EQ(s.data(), arr);
    CHECK_EQ(s.size(), 4);
    CHECK_EQ(s.size_bytes(), 16);
    CHECK_EQ(s.front(), 0);
    CHECK_EQ(s.back(), 3);
    CHECK_EQ(s[1], 1);
    CHECK_EQ(s[2], 2);
    {
      auto sub = s.template first<2>();
      CHECK_EQ(sub.data(), arr);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 0);
      CHECK_EQ(sub.back(), 1);
      CHECK_EQ(sub[1], 1);
    }
    {
      auto sub = s.first(2);
      CHECK_EQ(sub.data(), arr);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 0);
      CHECK_EQ(sub.back(), 1);
      CHECK_EQ(sub[1], 1);
    }
    {
      auto sub = s.template last<2>();
      CHECK_EQ(sub.data(), arr + 2);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 2);
      CHECK_EQ(sub.back(), 3);
      CHECK_EQ(sub[1], 3);
    }
    {
      auto sub = s.last(2);
      CHECK_EQ(sub.data(), arr + 2);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 2);
      CHECK_EQ(sub.back(), 3);
      CHECK_EQ(sub[1], 3);
    }
    {
      auto sub = s.template subspan<1, 2>();
      CHECK_EQ(sub.data(), arr + 1);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 1);
      CHECK_EQ(sub.back(), 2);
      CHECK_EQ(sub[1], 2);
    }
    {
      auto sub = s.subspan(1, 2);
      CHECK_EQ(sub.data(), arr + 1);
      CHECK_EQ(sub.size(), 2);
      CHECK_EQ(sub.size_bytes(), 8);
      CHECK_EQ(sub.front(), 1);
      CHECK_EQ(sub.back(), 2);
      CHECK_EQ(sub[1], 2);
    }
  }
  {
    std::array<int, 4> arr { 0, 1, 2, 3 };
    auto s = span<int>(arr);
    CHECK(!s.empty());
    CHECK_EQ(s.data(), arr.data());
    CHECK_EQ(s.size(), 4);
    CHECK_EQ(s.size_bytes(), 16);
    CHECK_EQ(s.front(), 0);
    CHECK_EQ(s.back(), 3);
    CHECK_EQ(s[1], 1);
    CHECK_EQ(s[2], 2);
  }
  {
    const std::array<int, 4> arr { 0, 1, 2, 3 };
    auto s = span<const int>(arr);
    CHECK(!s.empty());
    CHECK_EQ(s.data(), arr.data());
    CHECK_EQ(s.size(), 4);
    CHECK_EQ(s.size_bytes(), 16);
    CHECK_EQ(s.front(), 0);
    CHECK_EQ(s.back(), 3);
    CHECK_EQ(s[1], 1);
    CHECK_EQ(s[2], 2);
  }
}

TEST_SUITE_END();