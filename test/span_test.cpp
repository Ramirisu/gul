//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <gul_test.h>

#include <gul/span.hpp>

#include <array>

using namespace gul;

TEST_SUITE_BEGIN("span");

TEST_CASE("basic")
{
  static_assert(sizeof(span<int>) == 2 * sizeof(std::size_t), "");
  static_assert(sizeof(span<int, 10>) == sizeof(std::size_t), "");
  static_assert(std::is_default_constructible<span<int, dynamic_extent>>::value,
                "");
  static_assert(std::is_default_constructible<span<int, 0>>::value, "");
  static_assert(!std::is_default_constructible<span<int, 1>>::value, "");
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
    std::array<int, 4> arr { 0, 1, 2, 3 };
    auto s = span<int>(arr.begin(), arr.size());
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
    std::array<int, 4> arr { 0, 1, 2, 3 };
    auto s = span<int>(arr.begin(), arr.end());
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
  {
    auto s = span<int>();
    CHECK_EQ(s.begin(), s.end());
  }
  {
    int arr[] = { 0, 1, 2, 3 };
    auto s = span<int>(arr);
    auto it = s.begin();
    CHECK_EQ(*it++, 0);
    CHECK_EQ(*it++, 1);
    CHECK_EQ(*it--, 2);
    CHECK_EQ(*it++, 1);
    CHECK_EQ(*it++, 2);
    CHECK_EQ(*it++, 3);
    CHECK_EQ(it, s.end());
    it = s.begin();
    CHECK_EQ(*(it + 2), 2);
    CHECK_EQ(*(2 + it), 2);
    CHECK_EQ(*(it += 2), 2);
    CHECK_EQ(it[1], 3);
    CHECK_EQ(*(it - 1), 1);
    CHECK_EQ(*(it -= 1), 1);
    CHECK_EQ(s.end() - it, 3);
    CHECK(s.begin() != s.end());
    CHECK(s.begin() < s.end());
    CHECK(s.begin() <= s.begin());
    CHECK(s.begin() <= s.end());
    CHECK_FALSE(s.end() > s.end());
    CHECK(s.end() > s.begin());
    CHECK(s.end() >= s.end());
    CHECK(s.end() >= s.begin());
  }
  {
    int arr[] = { 0, 1, 2, 3 };
    auto s = span<int>(arr);
    auto it = s.rbegin();
    CHECK_EQ(*it++, 3);
    CHECK_EQ(*it++, 2);
    CHECK_EQ(*it++, 1);
    CHECK_EQ(*it++, 0);
    CHECK_EQ(it, s.rend());
    CHECK(s.rbegin() != s.rend());
  }
  {
    int arr[] = { 0, 1, 2, 3 };
    auto s = span<int>(arr);
    auto rs = as_bytes(s);
    static_assert_same<decltype(rs.front()), const byte&>();
    CHECK_EQ(rs.data(), reinterpret_cast<const byte*>(arr));
    CHECK_EQ(rs.size(), 16);
    auto ws = as_writable_bytes(s);
    static_assert_same<decltype(ws.front()), byte&>();
    CHECK_EQ(ws.data(), reinterpret_cast<byte*>(arr));
    CHECK_EQ(ws.size(), 16);
  }
}

#ifdef GUL_HAS_CXX17

TEST_CASE("deduction guides")
{
  {
    auto arr = std::array<int, 4>();
    static_assert_same<decltype(span(arr.begin(), arr.end())), span<int>>();
    static_assert_same<decltype(span(arr.begin(), 3)), span<int>>();
  }
  {
    const auto arr = std::array<int, 4>();
    static_assert_same<decltype(span(arr.begin(), arr.end())),
                       span<const int>>();
    static_assert_same<decltype(span(arr.begin(), 3)), span<const int>>();
  }
  {
    int arr[] = { 0, 1, 2, 3 };
    static_assert_same<decltype(span(arr)), span<int, 4>>();
  }
  {
    auto arr = std::array<int, 4>();
    static_assert_same<decltype(span(arr)), span<int, 4>>();
  }
  {
    const auto arr = std::array<int, 4>();
    static_assert_same<decltype(span(arr)), span<const int, 4>>();
  }
}

#endif

TEST_SUITE_END();
