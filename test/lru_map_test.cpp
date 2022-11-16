//
// Copyright (c) 2022 Ramirisu (labyrinth dot ramirisu at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ds_test.h>

#include <ds/lru_map.hpp>

#include <functional>
#include <random>
#include <type_traits>

TEST_SUITE_BEGIN("lru_map");

TEST_CASE("basic")
{
  ds::lru_map<int, int> m(2);
  CHECK_EQ(m.size(), 0);
  CHECK_EQ(m.capacity(), 2);
  CHECK(m.try_insert(1, 10));
  CHECK(!m.empty());
  CHECK_EQ(m.size(), 1);
  CHECK_EQ(m.peek(1), ds::optional<int>(10));
  CHECK(!m.try_insert(1, 10));
  CHECK(m.try_insert(2, 20));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(2), ds::optional<int>(20));
  CHECK(!m.try_insert(2, 20));
  CHECK(m.try_insert(3, 30));
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(3), ds::optional<int>(30));
  CHECK(!m.try_insert(3, 30));
  CHECK(!m.contains(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  CHECK(!m.erase(1));
  CHECK(m.contains(2));
  CHECK(m.contains(3));
  CHECK(m.erase(2));
  CHECK(!m.contains(2));
  CHECK(m.contains(3));
  m.clear();
  CHECK_EQ(m.size(), 0);
  CHECK(m.empty());
  CHECK(!m.contains(1));
  CHECK(!m.contains(2));
  CHECK(!m.contains(3));
}

TEST_CASE("Compare")
{
  struct compare {
    bool operator()(int lhs, int rhs) const noexcept
    {
      return lhs > rhs;
    }
  };

  compare comp;
  ds::lru_map<int, int, compare> m {
    10, { { 3, 10 }, { 2, 50 }, { 4, 30 }, { 1, 20 }, { 0, 0 } }, comp
  };
  int temp = 5;
  for (const auto& kv : m) {
    CHECK(kv.first < temp);
    temp = kv.first;
  }
}

TEST_CASE("range constructor")
{
  const std::vector<std::pair<int, int>> v(
      { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
  ds::lru_map<int, int> m(2, v.begin(), v.end());
  CHECK_EQ(m.size(), 2);
  CHECK_EQ(m.peek(3), ds::optional<int>(30));
  CHECK_EQ(m.peek(4), ds::optional<int>(40));
}

TEST_CASE("copy constructor")
{
  ds::lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  ds::lru_map<int, int> c(m);
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), ds::optional<int>(10));
  CHECK_EQ(c.peek(2), ds::optional<int>(20));
}

TEST_CASE("copy assignment operator")
{
  ds::lru_map<int, int> m(2, { { 1, 10 }, { 2, 20 } });
  ds::lru_map<int, int> c(2, { { 3, 30 }, { 4, 40 } });
  c = m;
  m.clear();
  CHECK_EQ(c.capacity(), 2);
  CHECK_EQ(c.size(), 2);
  CHECK_EQ(c.peek(1), ds::optional<int>(10));
  CHECK_EQ(c.peek(2), ds::optional<int>(20));
}

TEST_CASE("peek|cpeek")
{
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    static_assert_same<decltype(m.peek(1)), ds::optional<int&>>();
    int v = 10;
    CHECK_EQ(m.peek(1), ds::optional<int&>(v));
    m.peek(1).value() = 100;
    v = 100;
    CHECK_EQ(m.peek(1), ds::optional<int&>(v));
  }
  {
    const ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    static_assert_same<decltype(m.peek(1)), ds::optional<const int&>>();
    int v = 10;
    CHECK_EQ(m.peek(1), ds::optional<const int&>(v));
  }
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
    static_assert_same<decltype(m.cpeek(1)), ds::optional<const int&>>();
    int v = 10;
    CHECK_EQ(m.cpeek(1), ds::optional<const int&>(v));
  }
}

TEST_CASE("get|cget")
{
  ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 } });
  static_assert_same<decltype(m.get(1)), ds::optional<int&>>();
  static_assert_same<decltype(m.cget(1)), ds::optional<const int&>>();
  m.get(1);
  CHECK_NE(m.peek_lru(),
           ds::optional<std::pair<int, int>>(std::pair<int, int> { 1, 10 }));
  m.get(2);
  CHECK_NE(m.peek_lru(),
           ds::optional<std::pair<int, int>>(std::pair<int, int> { 2, 20 }));
  m.cget(1);
  CHECK_NE(m.peek_lru(),
           ds::optional<std::pair<int, int>>(std::pair<int, int> { 1, 10 }));
  m.cget(2);
  CHECK_NE(m.peek_lru(),
           ds::optional<std::pair<int, int>>(std::pair<int, int> { 2, 20 }));
}

TEST_CASE("try_assign")
{
  ds::lru_map<int, int> m(2, { { 1, 10 } });
  CHECK(m.try_assign(1, 100));
  CHECK_EQ(m.peek(1), ds::optional<int>(100));
  CHECK(!m.try_assign(2, 20));
}

TEST_CASE("insert_or_assign")
{
  ds::lru_map<int, int> m(2, { { 1, 10 } });
  CHECK(!m.insert_or_assign(1, 100));
  CHECK_EQ(m.peek(1), ds::optional<int>(100));
  CHECK(m.insert_or_assign(2, 20));
  CHECK_EQ(m.peek(2), ds::optional<int>(20));
}

TEST_CASE("erase iterator")
{
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    CHECK_EQ(m.erase(m.end()), m.end());
    CHECK_EQ(m.erase(m.cend()), m.cend());
  }
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.begin(), 1);
    auto it_next = std::next(it, 1);
    CHECK_EQ(m.erase(it), it_next);
  }
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.cbegin(), 1);
    auto it_next = std::next(it, 1);
    CHECK_EQ(m.erase(it), it_next);
  }
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.begin(), 1);
    auto it_next = std::next(it, 2);
    CHECK_EQ(m.erase(it, it_next), it_next);
  }
  {
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    auto it = std::next(m.cbegin(), 1);
    auto it_next = std::next(it, 2);
    CHECK_EQ(m.erase(it, it_next), it_next);
  }
}

TEST_CASE("iterator")
{
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      static_assert_same<decltype(*it), std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const ds::lru_map<int, int> m(
        4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.begin(); it != m.end(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.cbegin(); it != m.cend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      static_assert_same<decltype(*it), std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    const ds::lru_map<int, int> m(
        4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
  {
    std::map<int, int> exp({ { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    ds::lru_map<int, int> m(4, { { 1, 10 }, { 2, 20 }, { 3, 30 }, { 4, 40 } });
    for (auto it = m.crbegin(); it != m.crend(); ++it) {
      static_assert_same<decltype(*it), const std::pair<int, int>&>();
      CHECK_EQ(exp.at(it->first), it->second);
      exp.erase(it->first);
    }
    CHECK(exp.empty());
  }
}

TEST_CASE("random test")
{
  auto m = ds::lru_map<int, int>(64);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<> dist(0, 96);
  for (int i = 0; i < 65536; ++i) {
    auto key = dist(gen);
    m.try_insert(key, i);
  }
}

TEST_SUITE_END();
